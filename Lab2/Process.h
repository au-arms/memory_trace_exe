/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Process.h
 * Author: daniel
 *
 * Created on January 22, 2019, 10:29 PM
 */

#ifndef PROCESS_H
#define PROCESS_H

using namespace std;

class Process {
public:
    /**
    * Constructor
    * 
    * 
    * @param file_name input file
    * @throws std::runtime_error if file not found or error reading file
    */
    Process(const string &file_name);
    
    
    // Disable copy/move as specified by assignment
    Process(const Process &orig) = delete;
    Process(Process &&orig) = delete;
    Process operator=(const Process &orig) = delete;
    Process operator=(Process &&orig) = delete;
    
    // Destructor (no specific action needed)
    virtual ~Process();
    
    // Method to process the commands on the tracer file
    void Exec();
    
private:
    vector<uint8_t> memory;

};

#endif /* PROCESS_H */

