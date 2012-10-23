#include "tlmm.h"

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <stack>

struct _stack
{
public:
    void push(float val) { m_Stack.push(val); }
    float pop()
    {
	float rtn = m_Stack.top();
	m_Stack.pop();
	return rtn;
    }
private:
    std::stack<float> m_Stack;
};

typedef void(*instruction)(_stack*, float, _stack*);
typedef unsigned char code;
typedef std::map<code, instruction> codeMap;
typedef codeMap::iterator           codeMapIter;
codeMap g_Instructions;

#ifdef TLMM_COMPILE
#include <string>
typedef std::map<std::string, code> symMap;
typedef symMap::iterator            symMapIter;
symMap g_Symbols;
#endif/*TLMM_COMPILE*/

struct tlmmProgram
{
    int codeSize;
    int dataSize;
    code*  codeSeg;
    float* dataSeg;
};

tlmmProgram* tlmmInitProgram()
{
    tlmmProgram* prog = new tlmmProgram;
    return prog;
}

void tlmmTerminateProgram(tlmmProgram* prog)
{
    delete prog;
}

#ifdef TLMM_HAS_IO
void tlmmLoadProgramFile(tlmmProgram* prog, const char* filename)
{
    FILE* fp = fopen(filename, "rb+");
    if(fp == 0)
	return;
    tlmmLoadProgram(prog, fp);
    fclose(fp);
}

void tlmmLoadProgram(tlmmProgram* prog, FILE* fp)
{
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);
    char* data = new char[size + 1];
    fread(data, 1, size, fp);
#ifdef TLMM_COMPILE
    tlmmParseProgram(prog, data);
#endif/*TLMM_COMPILE*/
    delete [] data;
}

void tlmmSaveProgramBinary(tlmmProgram* prog, const char* filename)
{
}
#endif/*TLMM_HAS_IO*/

void tlmmLoadProgramBinary(tlmmProgram* prog, void* data, unsigned int size)
{
}

#ifdef TLMM_COMPILE
#include <string.h>
#include <vector>
void tlmmConvertRPN(std::vector<code>* symbols)
{
    std::vector<code> out;
    std::vector<code> stack;
    
    for(std::vector<code>::iterator iSym = symbols->begin();
	iSym != symbols->end();
	iSym++)
    {
	if(*iSym == g_Symbols["("]) // open bracket
	    stack.push_back(*iSym);
	else if(*iSym == g_Symbols["x"] || // x or a literan
		*iSym == 255)
	    out.push_back(*iSym);
	else if(*iSym != g_Symbols[")"]) // any symbol other than (, ), x or values
	{
	    if(stack.empty() ||
	       stack.back() == g_Symbols["("] ||
	       stack.back() > *iSym)
		stack.push_back(*iSym);
	    else
	    {
		do
		{
		    out.push_back(stack.back());
		    stack.pop_back();
		} while (!stack.empty() &&
			 stack.back() != g_Symbols["("] &&
			 stack.back() < *iSym);
		stack.push_back(*iSym);
	    }
	}
	else // close bracket
	{
	    while( stack.back() != g_Symbols["("] && // collapse until the last
		   !stack.empty() )                  // open bracket
	    {
		out.push_back(stack.back());
		stack.pop_back();
	    }
	}
    }

    while(!stack.empty())
    {
	out.push_back(stack.back());
	stack.pop_back();
    }

    symbols->clear();
    for(std::vector<code>::iterator iSym = out.begin();
	iSym != out.end();
	iSym++)
	symbols->push_back(*iSym);
}

void tlmmParseProgram(tlmmProgram* prog, const char* program)
{
    const char* p = program;
    std::vector<code> syms;
    std::vector<float> regs;
    bool found;
    while(*p)
    {
	found = false;
	for(symMapIter iSym = g_Symbols.begin();
	    iSym != g_Symbols.end();
	    iSym++)
	{
	    if(strstr(p, iSym->first.c_str()) == p)
	    {
		syms.push_back(iSym->second);
		p += iSym->first.size();
		found = true;
		break;
	    }
	}
	if(found)
	    continue;	
	
	if(*p >= '0' && *p <= '9')
	{
	    syms.push_back(255);
	    regs.push_back(atof(p));
	    while( (*p >= '0' && *p <= '9') ||
		   *p == '.') ++p;
	}
	else
	{
	    // error and return
	    return;
	}
    }

    // if we've got here, we've successfully parsed the program
    // now we have to sort the symbols
    tlmmConvertRPN(&syms);
    prog->codeSize = syms.size();
    prog->codeSeg = new code[syms.size()];
    int i = 0;
    for(std::vector<code>::iterator iSym = syms.begin();
	iSym != syms.end();
	iSym++)
	prog->codeSeg[i++] = *iSym;
    
    prog->dataSize = regs.size();
    prog->dataSeg = new float[regs.size()];
    i = 0;
    for(std::vector<float>::iterator iReg = regs.begin();
	iReg != regs.end();
	iReg++)
	prog->dataSeg[i++] = *iReg;
    
}
#endif/*TLMM_COMPILE*/

float tlmmGetValue(tlmmProgram* prog, float ref)
{
    if(prog == 0)
	return 0.0f;
    _stack stack;
    _stack regs;
    for(int i = prog->dataSize - 1; i >= 0; --i)
	regs.push(prog->dataSeg[i]);
    for(int i = 0; i < prog->codeSize; ++i)
	g_Instructions[prog->codeSeg[i]](&stack, ref, &regs);
    return stack.pop();
}

#define INSTRUCTION(fn)						\
    void tlmmFunc##fn(_stack* stack, float x, _stack* reg)
#define REGISTER_FUNCS()			\
    int cnt = 1;				\
    int scnt = 0;
#ifdef TLMM_COMPILE
# define FUNC(fn, sym)				\
    g_Instructions[cnt] = tlmmFunc##fn;		\
    g_Symbols[sym] = cnt;			\
    cnt++;
# define SYM(sym)				\
    g_Symbols[sym] = 254 - scnt;		\
    scnt ++;
#else
# define FUNC(fn, sym)				\
    g_Instructions[cnt] = tlmmFunc##fn;		\
    cnt++;
# define SYM(sym) {}
#endif/*TLMM_COMPILE*/
#define ACCESS(fn)				\
    g_Instructions[255] = tlmmFunc##fn;

#include <math.h>
INSTRUCTION(Add){ stack->push(stack->pop() + stack->pop()); }
INSTRUCTION(Del){ stack->push(stack->pop() - stack->pop()); }
INSTRUCTION(Mul){ stack->push(stack->pop() * stack->pop()); }
INSTRUCTION(Div){ stack->push(stack->pop() / stack->pop()); }
INSTRUCTION(Pow){ stack->push(pow(stack->pop(), stack->pop())); }
INSTRUCTION(Reg){ stack->push(reg->pop()); }
INSTRUCTION(X)  { stack->push(x); }

class tlmmInit
{
public:
tlmmInit()
{
    REGISTER_FUNCS();
    SYM("(");
    SYM(")");
    FUNC(Pow, "^");
    FUNC(Mul, "*");
    FUNC(Div, "/");
    FUNC(Add, "+");
    FUNC(Del, "-");
    FUNC(X,   "x");
    ACCESS(Reg);
}
};
tlmmInit __init;

namespace tlmm
{
    Program::Program()
    {
	m_prog  = tlmmInitProgram();
    }

    Program::~Program()
    {
	tlmmTerminateProgram(m_prog);
    }

    void Program::Load(std::string filename)
    {
#ifdef TLMM_HAS_IO
	tlmmLoadProgramFile(m_prog, filename.c_str());
#endif/*TLMM_HAS_IO*/
    }

    void Program::Save(std::string filename)
    {
#ifdef TLMM_HAS_IO
	tlmmSaveProgramBinary(m_prog, filename.c_str());
#endif/*TLMM_HAS_IO*/
    }

    void Program::Parse(std::string data)
    {
	tlmmParseProgram(m_prog, data.c_str());
    }

    float Program::GetValue(float ref)
    {
	tlmmGetValue(m_prog, ref);
    }
}
