/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Process.cpp
 * Author: daniel
 * 
 * Created on January 22, 2019, 10:29 PM
 */

#include "Process.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


Process::Process(const string &file_name) {
  // Open the input file
  std::ifstream text_file;
  text_file.open(file_name);
  
  // If there is a problem with the file, throw error
  if (text_file.fail()) {
    throw std::runtime_error("ERROR: file not found: " + file_name);
  }
}


Process::Process(const Process& orig) {
}

Process::~Process() {
    text_file.close();
}

