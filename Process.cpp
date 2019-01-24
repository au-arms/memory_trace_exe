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

  // TODO finish implementation
  auto cmp     = [this](std::vector<std::string> args) -> bool{
                   std::cout << "cmp\n"; return(true);
                   uint32_t addr1 = getDecimal(args[0]);
                   uint32_t addr2 = getDecimal(args[2]);
                   uint32_t count = getDecimal(args[3]);
                   uint8_t addr1_value = mem_ref[addr1];
                   uint8_t addr2_value = mem_ref[addr2];
                   return true;
                 };

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

  auto fill    = [this](std::vector<std::string> args) -> bool
                 {std::cout << "fill\n"; return(true);};

  // dup is already a command in the namespace
  auto dup_    = [this](std::vector<std::string> args) -> bool
                 {std::cout << "dup\n"; return(true);};

  auto print   = [this](std::vector<std::string> args) -> bool{
                   std::cout << "print\n";
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

  std::string command_string;
  std::vector<std::string> args;
  std::string command_type;

  while (std::getline(process_file, command_string)) {
    // Only parse commands greater than
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



