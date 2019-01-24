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

//Using boost for string splitting,
//Used for parsing input to get commands
#include <boost/algorithm/string.hpp>



//Constructor
//instantiates the process_file private variable
//Opens the file name passed to it
//TODO full doxygen comment
Process::Process(const std::string &file_name){
  process_file.open(file_name);
  if (process_file.fail()){
    throw std::runtime_error("ERROR: file not found: "+ file_name);
  }
}

uint32_t Process::getDecimal(std::string hex_string){
  return std::stoul(hex_string, nullptr, 16);
}

// Get a functional of the respective command
//std::function<bool(std::vector<std::string>)>
auto Process::getCommandFunction(std::string command_type){

  // Helper lambdas to process the required command

  // TODO implement max memory 
  auto memsize = [this](std::vector<std::string> args) -> bool{
                   uint32_t mem_amount = getDecimal(args[0]);
                   std::vector<uint8_t> newMem(mem_amount,0);
                   this->mem_ref=newMem;
                   return(true);
                 };

  // TODO finish implementation,
  // Need to validate comparison vector.
  auto cmp     = [this](std::vector<std::string> args) -> bool{
                   std::cout << "cmp!!!\n";
                   uint32_t addr1 = getDecimal(args[0]);
                   uint32_t addr2 = getDecimal(args[2]);
                   uint32_t count = getDecimal(args[3]);
                   uint8_t addr1_value = mem_ref[addr1];
                   uint8_t addr2_value = mem_ref[addr2];

                   uint32_t whole_uint8_t = count/8;
                   uint8_t bits_remaining = count%8;

                   std::vector<bool> comparison_result(whole_uint8_t,false);

                   std::transform(mem_ref.begin()+addr1,
                                  mem_ref.begin()+addr1+count,
                                  mem_ref.begin()+addr2,
                                  comparison_result.begin(),
                                  [](uint8_t val1, uint8_t val2){
                                    return bool(val1^val2);
                                  });

                   auto comapare_remaining_bits =
                     [addr1, addr2, whole_uint8_t, bits_remaining]
                     (std::vector<uint8_t> mem_ref){

                       //std::cout << "comparing " << unsigned(bits_remaining) << " bits!\n";

                       uint32_t index1 = addr1+whole_uint8_t+1;
                       uint32_t index2 = addr2+whole_uint8_t+1;
                       auto get_remainder_bits =
                         [bits_remaining, mem_ref](uint32_t index)
                         {return(mem_ref[index] >> (8-bits_remaining));};
                       uint8_t remainder_bits1 = get_remainder_bits(index1);
                       uint8_t remainder_bits2 = get_remainder_bits(index2);
                       return (remainder_bits1^remainder_bits2);
                     };

                   if(bits_remaining){
                     comparison_result.push_back(comapare_remaining_bits(mem_ref));
                   }

                   std::for_each(comparison_result.begin(),comparison_result.end(),
                                 [](bool comp){std::cout<<comp<<" ~\n";});


                   return true;
                 };

  // TODO roxygen comment
  auto set     = [this](std::vector<std::string> args) -> bool{
                   std::cout << "set\n";
                   uint32_t addr = getDecimal(args[0]);
                   std::vector<uint8_t> values_decimal(args.size()-2);
                   std::transform(args.begin()+2,
                                  args.end(),
                                  values_decimal.begin(),
                                  [this](std::string value_hex){return getDecimal(value_hex);});

                   std::transform(values_decimal.begin(),
                                  values_decimal.end(),
                                  mem_ref.begin()+addr,
                                  [](uint8_t value_decimal){return value_decimal;});
                   return(true);
                 };

  // TODO implement
  auto fill    = [this](std::vector<std::string> args) -> bool{
                   std::cout << "fill\n";
                   uint32_t addr = getDecimal(args[0]);
                   uint8_t value = getDecimal(args[2]);
                   uint32_t count = getDecimal(args[3]);

                   return(true);
                 };

  // dup is already a command in the namespace, changed to dup_
  auto dup_    = [this](std::vector<std::string> args) -> bool{
                   std::cout << "dup\n";
                   uint32_t src_addr = getDecimal(args[0]);
                   uint32_t dest_addr = getDecimal(args[2]);
                   uint32_t count = getDecimal(args[3]);

                   std::transform(mem_ref.begin()+src_addr,
                                  mem_ref.begin()+src_addr+count,
                                  mem_ref.begin()+dest_addr,
                                  [](uint8_t value_decimal){return value_decimal;});

                   return(true);
                 };

  // TODO format this to specific output
  auto print   = [this](std::vector<std::string> args) -> bool{
                   uint32_t addr = getDecimal(args[0]);
                   uint32_t count = getDecimal(args[2]);

                   std::for_each(mem_ref.begin()+addr,mem_ref.begin()+addr+count,
                                 [](uint8_t value){
                                   std::stringstream stream;
                                   stream << std::hex << unsigned(value);
                                   std::cout << stream.str() << " ";
                                 });

                   std::cout << "\n";

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
    }
  }
}


std::vector<std::string> Process::getArguments(std::string command){
  std::vector<std::string> tokens;
  std::vector<std::string> clean_tokens;
  boost::split(tokens, command, [](char c){return c == ' ';});
  for(auto const& token: tokens){
    if(!token.empty()){clean_tokens.push_back(token);};
  };
  return clean_tokens;
}

// Check for comments or empty lines
bool Process::isCode(char *firstCharacter){
  if(!std::strncmp("*", firstCharacter, 1) ||
     !std::strncmp(" ", firstCharacter, 1)){
    return false;
  }
  return true;
}



