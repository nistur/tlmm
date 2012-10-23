#include "tlmm.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    tlmmProgram* prog = tlmmInitProgram();
    tlmmParseProgram(prog, "x+12");
    printf("value: %f\n", tlmmGetValue(prog, 2));
    tlmmTerminateProgram(prog);
    return 0;
}
