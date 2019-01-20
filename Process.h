#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <stdint.h>
#include <vector>

class Process {
public:
  Process(const std::string &file_name);
  // TODO DESTRUCTOR
  Process(const Process &orig) = delete;
  Process(Process &&orig) = delete;
  Process operator=(const Process &orig) = delete;
  Process operator=(Process && orig) = delete;

  void Exec();
private:
  std::vector<uint8_t> mem_ref;

}

#endif
