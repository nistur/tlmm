#define TLMM_HAS_IO
#include "tlmm.h"
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv)
{
    if(argc == 1)
	return 1;
    tlmmProgram* prog = tlmmInitProgram();
    tlmmLoadProgramFile(prog, argv[1]);

    while(true)
    {
	std::cout << "x=";
	std::string in;
	std::cin >> in;
	if(in == "q")
	    break;
	float val = tlmmGetValue(prog, atof(in.c_str()));
	std::cout << "y=" << val << std::endl;
    }

    tlmmTerminateProgram(prog);
    return 0;
}
