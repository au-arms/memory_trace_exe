#ifndef PROCESS_H
#define PROCESS_H

#include <cstring>
#include <stdint.h>
#include <vector>
#include <fstream>
#include <functional>

class Process {
public:
    
    /**
    * Constructor
    * 
    * Reads trace file.
    * 
    * @param file_name input file
    * @throws std::runtime_error if file not found or error reading file
    */
    Process(const std::string &file_name);
  
    // Disable copy/move as specified by assignment
    Process(const Process &orig) = delete;
    Process(Process &&orig) = delete;
    Process operator=(const Process &orig) = delete;
    Process operator=(Process && orig) = delete;

    /**
    * Destructor
    * 
    *  Closes the file at the end of the program
    * 
    */
    virtual ~Process();
    
    /**
    * Class method
    * 
    * Processes all the commands in the trace file using the memory vector 
    * mem_ref
    * 
    */
    void Exec();
    
    /**
    * isCode
    * 
    *  Checks the first character in a line to verify is it is an empty line,
    *  a comment or an instruction.
    * 
    * @param pointer
    * @returns boolean
    */
    bool isCode(char* firstCharacter);

    /**
    * getArguments
    * 
    *  Parses the lines read from file and extracts the different tokens that 
    *  compose it.
    * 
    * @param string
    * @returns string vector
    */
    std::vector<std::string> getArguments(std::string command);
    
    /**
    * getCommandFunction
    * 
    *  
    * 
    * @param string hex_string
    * @returns sequence of integer values
    */
    auto getCommandFunction(std::string command_type);

    /**
    * getDecimal
    * 
    * Gets string hex_string, discards whitespace characters until first 
    * non-whitespace character
    * 
    * @param string hex_string
    * @returns integer value
    */
    uint32_t getDecimal(std::string hex_string);

private:
    std::vector<uint8_t> mem_ref;
    std::ifstream process_file;
    std::uint32_t max_memory = 4000000;
    void setMemRef(const uint32_t memsize);

};

#endif
