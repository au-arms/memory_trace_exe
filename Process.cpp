#include "Process.h"

#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <functional>
#include <map>

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

// Helper lambdas to process the required command
// TODO implement mem_ref alteration
auto memsize = [](std::vector<std::string> args) -> bool
               {std::cout << "memsize\n"; return(true);};

auto cmp     = [](std::vector<std::string> args) -> bool
               {std::cout << "cmp\n"; return(true);};

auto set     = [](std::vector<std::string> args) -> bool
               {std::cout << "set\n"; return(true);};

auto fill    = [](std::vector<std::string> args) -> bool
               {std::cout << "fill\n"; return(true);};

auto dup_    = [](std::vector<std::string> args) -> bool
               {std::cout << "dup\n"; return(true);};

auto print   = [](std::vector<std::string> args) -> bool
               {std::cout << "print\n"; return(true);};


// Get a functional of the respective command
//std::function<bool(std::vector<std::string>)>
auto Process::getCommandFunction(std::string command_type){

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



