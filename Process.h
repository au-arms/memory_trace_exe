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
  auto getCommandFunction(std::string command_type);

  uint32_t getDecimal(std::string hex_string);
private:
  std::vector<uint8_t> mem_ref;
  std::ifstream process_file;
  void setMemRef(const uint32_t memsize);

};

#endif
