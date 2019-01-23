#ifndef PROCESS_H
#define PROCESS_H

#include <cstring>
#include <stdint.h>
#include <vector>
#include <fstream>
#include <functional>

class Process {
public:
  Process(const std::string &file_name);
  // TODO DESTRUCTOR
  Process(const Process &orig) = delete;
  Process(Process &&orig) = delete;
  Process operator=(const Process &orig) = delete;
  Process operator=(Process && orig) = delete;

  void Exec();
  bool isCode(char* firstCharacter);

  std::vector<std::string> getArguments(std::string command);
  //std::function<bool(std::vector<std::string>)> getCommandFunction(std::string command_type);
  auto getCommandFunction(std::string command_type);

private:
  std::vector<uint8_t> mem_ref;
  std::ifstream process_file;
  void setMemRef(const uint32_t memsize);

};

#endif
