#include "tlmm.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    tlmm::Program prog;
    prog.Parse("x+2");
    printf("value: %f\n", prog.GetValue(2));
    return 0;
}
