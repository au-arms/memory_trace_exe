/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: aarmstrong
 *
 * Created on January 18, 2019, 11:02 AM
 */

#include "Process.h"
#include <cstdlib>
#include <iostream>

using namespace std;

/*
 * 
 */
int main(int argc, char* argv[]) {
  Process test(argv[1]);
  test.Exec();
  return 0;
}

