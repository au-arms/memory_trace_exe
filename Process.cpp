#include "Process.h"

#include <sstream>
#include <fstream>
#include <cstring>
#include <iostream>
#include <typeinfo>

Process::Process(const std::string &file_name){
  process_file.open(file_name);
  if (process_file.fail()){
    throw std::runtime_error("ERROR: file not found: "+ file_name);
  }
}

void Process::Exec(){
  std::string command;
  while (std::getline(process_file, command)) {
    if(command.length() != 0){
      char* command_char = &command.at(0);

      // Check if command is a comment
      if(!std::strncmp("*", command_char,1)){
        std::cout << command << "\n";
      }


    }
    // if(std::strncmp (command.at(0), "*", 1) == 0){
    //   continue;
    // }
  }
}


