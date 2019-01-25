#include "Process.h"

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <functional>
#include <map>
#include <algorithm>
#include <iomanip>
#include <stdint.h>

//Opens the file name passed to it
/**
 * Constructor
 * 
 * Reads trace file and throws error if it fails. 
 * 
 * @param file_name input file
 * @throws std::runtime_error if file not found or error reading file
*/
Process::Process(const std::string &file_name){
  process_file.open(file_name);
  if (process_file.fail()){
    throw std::runtime_error("ERROR: file not found: "+ file_name);
  }
}

/**
 * getDecimal
 * 
 * Gets string hex_string & converts it to integer decimal.
 * 
 * @param string hex_string
 * @returns integer value
*/
uint32_t Process::getDecimal(std::string hex_string){
  return std::stoul(hex_string, nullptr, 16);
}

// Get a functional of the respective command
//std::function<bool(std::vector<std::string>)>
/**
 * getCommandFunction
 * 
 * Get a functional of the command that is represented by
 * the command_type string.
 * 
 * @param string hex_string
 * @returns sequence of integer values
*/

auto Process::getCommandFunction(std::string command_type){

  // Helper lambdas to process the required command

  /**
  * memsize
  * 
  *  Sets the size of the virtual memmory to be used
  * 
  * @param string vector
  * @returns boolean
  */
  auto memsize = [this](std::vector<std::string> args) -> bool{
                   uint32_t mem_amount = getDecimal(args[0]);
                   if (mem_amount > max_memory){
                       return(false);
                   }
                   std::vector<uint8_t> newMem(mem_amount,0);
                   this->mem_ref=newMem;
                   return(true);
                 };

  /**
  * cmp
  * 
  *  Compares a given number of bytes starting at two different addresses
  * 
  * @param string vector
  * @returns boolean
  */
  auto cmp = [this](std::vector<std::string> args) -> bool{
                   uint32_t addr1 = getDecimal(args[0]);
                   uint32_t addr2 = getDecimal(args[2]);
                   uint32_t count = getDecimal(args[3]);
                   uint8_t addr1_value = mem_ref[addr1];
                   uint8_t addr2_value = mem_ref[addr2];

                   uint32_t whole_uint8_t = count/8;
                   uint8_t bits_remaining = count%8;

                   std::vector<bool> comparison_result(whole_uint8_t,false);

                   // compares incomplete uint8_t records for equality
                   auto comapare_remaining_bits =
                     [addr1, addr2, whole_uint8_t, bits_remaining]
                     (std::vector<uint8_t> mem_ref){
                       uint32_t index1 = addr1+whole_uint8_t+1;
                       uint32_t index2 = addr2+whole_uint8_t+1;
                       auto get_remainder_bits =
                         [bits_remaining, mem_ref](uint32_t index)
                         {return(mem_ref[index] >> (8-bits_remaining));};
                       uint8_t remainder_bits1 = get_remainder_bits(index1);
                       uint8_t remainder_bits2 = get_remainder_bits(index2);
                       return (remainder_bits1^remainder_bits2);
                     };

                   std::transform(mem_ref.begin()+addr1,
                                  mem_ref.begin()+addr1+count,
                                  mem_ref.begin()+addr2,
                                  comparison_result.begin(),
                                  [](uint8_t val1, uint8_t val2){
                                    return bool(val1^val2);
                                  });

                   if(bits_remaining){
                     comparison_result.push_back(comapare_remaining_bits(mem_ref));
                   }

                   // Really tried to get the CMP failure output out correctly,
                   // not sure what went wrong with the code but it kept dumping the
                   // core / memory map & failing... :(

                   // uint32_t index1 = addr1;
                   // uint32_t index2 = addr2;
                   // uint32_t comp_index = 0;
                   // // trivial function for getting correct 0 padded hex string from
                   // // decimal input.
                   // auto getHexString =
                   //   [](uint32_t width, uint32_t value){
                   //     std::stringstream stream;
                   //     stream << std::setfill('0') << std::setw(width)
                   //            << std::hex << unsigned(value);
                   //     return stream.str();
                   //   };

                   // for(uint32_t comp_index = 0;
                   //     comp_index < comparison_result.size();
                   //     comp_index++, index1++, index2++
                   //     ){
                   //   std::cout<< double(comp_index)/comparison_result.size() << " ";

                   //   if(comparison_result[comp_index]){
                   //     std::cout << "HERE!\n\n";
                   //     std::cerr << "cmp error, addr1 = "
                   //               << getHexString(7, index1) << ", value = "
                   //               << getHexString(2, mem_ref[index1]) << ", addr2 = "
                   //               << getHexString(7, index2) << ", value = "
                   //               << getHexString(2, mem_ref[index2]);
                   //   };
                   // }

                   std::for_each(comparison_result.begin(),comparison_result.end(),
                                 [](bool comp){
                                   if(comp){std::cerr << "CMP CHECK FAIL\n";};
                                 });

                   return true;
                 };

  /**
  * set
  * 
  *  Assigns given values starting at a given address
  * 
  * @param string vector
  * @returns boolean
  */
  auto set = [this](std::vector<std::string> args) -> bool{
                   uint32_t addr = getDecimal(args[0]);
                   std::vector<uint8_t> values_decimal(args.size()-2);
                   std::transform(args.begin()+2, args.end(),
                           values_decimal.begin(),
                           [this](std::string value_hex){
                                      return getDecimal(value_hex);
                                  });

                   std::transform(values_decimal.begin(), values_decimal.end(),
                                  mem_ref.begin()+addr, [](
                                  uint8_t value_decimal){
                                      return value_decimal;
                                  });
                   return(true);
                 };

  /**
  * fill
  * 
  *  Assigns given value to a given number of bytes starting at a given address
  * 
  * @param string vector
  * @returns boolean
  */
  auto fill = [this](std::vector<std::string> args) -> bool{
                   uint32_t addr = getDecimal(args[0]);
                   uint8_t value = getDecimal(args[2]);
                   uint32_t count = getDecimal(args[3]);

                   std::transform(mem_ref.begin()+addr,
                                  mem_ref.begin()+addr+count,
                                  mem_ref.begin()+addr,
                                  [value](uint8_t old){return value;});

                   return(true);
                 };

  /**
  * dup_
  * 
  *  Duplicates given number of values at a source address to a destination
  *  address
  * 
  * @param string vector
  * @returns boolean
  */
  // dup is already a command in the namespace, changed to dup_
  auto dup_ = [this](std::vector<std::string> args) -> bool{
                   uint32_t src_addr = getDecimal(args[0]);
                   uint32_t dest_addr = getDecimal(args[2]);
                   uint32_t count = getDecimal(args[3]);

                   std::transform(mem_ref.begin()+src_addr,
                                  mem_ref.begin()+src_addr+count,
                                  mem_ref.begin()+dest_addr,
                                  [](uint8_t value_decimal){return value_decimal;});

                   return(true);
                 };
  /**
  * print
  * 
  *  Prints a given number of bytes at a given address
  * 
  * @param string vector
  * @returns boolean
  */
  auto print = [this](std::vector<std::string> args) -> bool{
                   uint32_t addr = getDecimal(args[0]);
                   uint32_t count = getDecimal(args[2]);
                   uint32_t elements_printed = 0;
                   // records if a newline needs to be created after
                   // print_mem.
                   bool newline_state = false;

                   // helper lambda to print
                   auto print_mem =
                     [&elements_printed, &newline_state, addr, count]
                     (uint8_t value){
                       // Create the stream to print
                       std::stringstream stream;
                       // print the address part if a new line
                       if(!(elements_printed%16)){
                         newline_state = true;
                         stream << std::setfill('0') << std::setw(7)
                                << std::hex << unsigned(addr+elements_printed)
                                << ": ";
                       };
                       // print the value from memory in hex string
                       stream << std::setfill('0') << std::setw(2)
                              << std::hex << unsigned(value);
                       elements_printed ++;
                       // print the stream & a newline if appropriate
                       std::cout << stream.str()
                                 << ((elements_printed%16 ) ? " " : "");
                       // print newline, and tell outer function no new line
                       // needs to be printed if this is the last value
                       if(!(elements_printed%16)){
                         newline_state = false;
                         std::cout << "\n";
                       };
                     };

                   std::for_each(mem_ref.begin()+addr,
                                 mem_ref.begin()+addr+count,
                                 print_mem
                                 );

                   // finish newline if necessary
                   if(newline_state){std::cout << "\n";};
                   return(true);
                 };

  std::map<std::string, std::function<bool(std::vector<std::string>)>> functMap =
    {{"memsize", memsize},
     {"cmp", cmp},
     {"set", set},
     {"fill", fill},
     {"dup", dup_},
     {"print", print}
    };

  return(functMap[command_type]);
}

/**
  * Exec()
  * 
  *  Uses class variable file_name, reads each line and executes the appropriate
  *  commands. Writes commands and output to file.
  * 
  */
void Process::Exec(){

  std::string              command_string;
  std::vector<std::string> args;
  std::string              command_type;
  uint32_t                 line_number = 0;

  while (std::getline(process_file, command_string)) {
    line_number ++;
    std::cout << line_number << ":";
    // Only parse commands greater than 0
    if(command_string.length() != 0){
      std::cout << command_string << "\n";
      // Check if line is code
      if(isCode(&command_string.at(0))){
        args = getArguments(command_string);
        std::string command_type = args[1];
        auto commandFunction = getCommandFunction(command_type);
        commandFunction(args);
      }
    }else{
      std::cout << "\n";
    }
  }
}

/**
  * getArguments
  * 
  *  Parses the lines read from file and extracts the different tokens that 
  *  compose it.
  * 
  * @param string
  * @returns string vector
  */
std::vector<std::string> Process::getArguments(std::string command){
  std::vector<std::string> tokens;
  std::vector<std::string> clean_tokens;
  std::string token;

  while(token != command){
    token = command.substr(0,command.find_first_of(" "));
    command = command.substr(command.find_first_of(" ") + 1);
    if(!token.empty()){clean_tokens.push_back(token);};
  }

  return clean_tokens;
}

/**
  * Destructor
  * 
  *  Closes the file at the end of the program
  * 
  */
Process::~Process() {
    process_file.close();
}

/**
  * isCode
  * 
  *  Checks the first character in a line to verify is it is an empty line,
  *  a comment or an instruction.
  * 
  * @param pointer
  * @returns boolean
  */
// Check for comments or empty lines
bool Process::isCode(char *firstCharacter){
  if(!std::strncmp("*", firstCharacter, 1) ||
     !std::strncmp(" ", firstCharacter, 1)){
    return false;
  }
  return true;
}



