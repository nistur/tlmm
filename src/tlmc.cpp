#define TLMM_HAS_IO
#include "tlmm.h"
#include <stdio.h>
#include <string.h>

struct _params
{
    char* equation;
    const char* objname;
};

void parseCommandLine(int argc, char** argv, _params& params)
{
    static const char* obj = "a.tlmp";
    params.objname = obj;
    for(int i = 1; i < argc; ++i)
    {
	if(strcmp(argv[i], "-o") == 0)
	    params.objname = argv[++i];
	else if(strcmp(argv[i], "-e") == 0)
	{
	    params.equation = argv[++i];
	}
	else if(FILE* fp = fopen(argv[i], "rb"))
	{
	    fseek(fp, 0, SEEK_END);
	    long size = ftell(fp);
	    rewind(fp);
	    params.equation = new char[size];
	    fread(params.equation, 1, size, fp);
	    fclose(fp);
	}
    }
}

int main(int argc, char** argv)
{
    _params p;
    parseCommandLine(argc, argv, p);

    tlmmProgram* prog = tlmmInitProgram();
    tlmmParseProgram(prog, p.equation);
    tlmmSaveProgram(prog, p.objname);
    
    tlmmTerminateProgram(&prog);
    return 0;
}
