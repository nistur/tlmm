#include "tlmm.h"

#include <string.h>
#include <stdlib.h>

#ifndef TLMM_STACK_MAX
#define TLMM_STACK_MAX 256
#endif/*TLMM_STACK_MAX*/

/* xxHash
 * 
 */
#define XX_PRIME1 2654435761U
#define XX_PRIME2 2246822519U
#define XX_PRIME3 3266489917U
#define XX_PRIME4 668265263U
#define XX_PRIME5 0x165667b1

#define _rotl(x,r) ((x<<r)|(x>(32-r)))

typedef unsigned int _hash;

inline _hash XXH_small(const void* key, int len, _hash seed = 0)
{
    const unsigned char* p = (unsigned char*)key;
    const unsigned char* const bEnd = p + len;
    unsigned int idx = seed + XX_PRIME1;
    unsigned int crc = XX_PRIME5;
    const unsigned char* const limit = bEnd - 4;
    while(p<limit)
    {
	crc += ((*(unsigned int*)p) + idx++);
	crc += _rotl(crc, 17)*XX_PRIME4;
	crc *= XX_PRIME1;
	p+= 4;
    }
    while(p<bEnd)
    {
	crc += ((*p)+idx++);
	crc *= XX_PRIME1;
	p++;
    }
    crc += len;
    crc ^= crc>>15;
    crc *= XX_PRIME2;
    crc ^= crc>>13;
    crc *= XX_PRIME3;
    crc ^= crc>>16;
    return crc;
}

#define Hash(c) XXH_small((const void*)c, strlen(c))

/*
 * end xxHash
 */
struct _stack
{
    float stack[TLMM_STACK_MAX];
    int   top;
};
inline void  _stackPush(_stack* stack, float val) { stack->stack[stack->top++] = val; }
inline float _stackPop (_stack* stack) { return stack->stack[--stack->top]; }

#define PUSH(x) _stackPush(stack, x)
#define POP()   _stackPop(stack)
#define CONST()   _stackPop(consts)

struct tlmmHeader
{
    char id[4];
    char codeSize;
    char dataSize;
};

const char* ID = "TLMM";

typedef void(*_instruction)(_stack*, _stack*, float*);
typedef unsigned char _code;
_instruction* g_Instructions;

#ifdef TLMM_COMPILE
#include <string>
typedef struct __sym {_hash hash; _code code; const char* sym; } _sym;
_sym* g_Symbols;
inline _sym& _symbol(const char* sym)
{
    _hash hash = Hash(sym);
    for(int i=0; i<256; ++i)
	if(g_Symbols[i].hash == hash)
	    return g_Symbols[i];
    for(int i=0; i<256; ++i)
	if(g_Symbols[i].hash == 0)
	{
	    g_Symbols[i].hash = hash;
	    g_Symbols[i].sym = sym;
	    return g_Symbols[i];
	}
    return g_Symbols[0];
}
inline _code _symbolCode(const char* sym)
{
    _hash hash = Hash(sym);
    for(int i=0; i<256; ++i)
	if(g_Symbols[i].hash == hash)
	    return g_Symbols[i].code;
    return g_Symbols[0].code;
}
#endif/*TLMM_COMPILE*/

#define SAFE_DELETE(x) if(x) { delete x; x = 0; }
#define SAFE_DELETE_ARRAY(x) if(x) { delete [] x; x = 0; }

struct tlmmProgram
{
    int codeSize;
    int dataSize;
#ifdef DEBUG
    _code*  codeSeg;
#else
    _instruction* codeSeg;
#endif/*DEBUG*/
    float* dataSeg;
    char*  eqn;
    float  registers[26];
};

inline void tlmmClearProgram(tlmmProgram* prog)
{
    prog->codeSize = 0;
    prog->dataSize = 0;
    SAFE_DELETE_ARRAY(prog->codeSeg);
    SAFE_DELETE_ARRAY(prog->dataSeg);
    SAFE_DELETE_ARRAY(prog->eqn);
    memset(prog->registers, 0, sizeof(float) * 26);
}

tlmmProgram* tlmmInitProgram()
{
    tlmmProgram* prog = new tlmmProgram;
    prog->codeSeg = 0;
    prog->dataSeg = 0;
    prog->eqn = 0;
    return prog;
}

tlmmReturn tlmmTerminateProgram(tlmmProgram** prog)
{
    SAFE_DELETE(*prog);
    return TLMM_SUCCESS;
}
const char* tlmmGetEquation(tlmmProgram* prog)
{
    return prog ? prog->eqn : 0;
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
    if(tlmmLoadProgramBinary(prog, data, size) == false)
    {
#ifdef TLMM_COMPILE
	tlmmParseProgram(prog, data);
#endif/*TLMM_COMPILE*/
    }
    delete [] data;
}

#ifndef DEBUG
_code tlmmLookupCode(_instruction instruction)
{
    for(int i=0; i < 256; ++i)
	if(g_Instructions[i] == instruction)
	    return (_code)i;
    return 0;
}
#endif/*DEBUG*/

void tlmmSaveProgram(tlmmProgram* prog, const char* filename)
{
    static tlmmHeader header;
    memcpy(header.id, ID, 4);
    header.codeSize = (char) prog->codeSize;
    header.dataSize = (char) prog->dataSize;

    FILE* fp = fopen(filename, "wb");
    fwrite(&header, sizeof(header), 1, fp);
    
#ifdef DEBUG
    fwrite(prog->codeSeg, sizeof(_code), prog->codeSize, fp);
#else
    // lookup the code to write
    _code* codeSeg = new _code[prog->codeSize];
    for(int i=0; i<prog->codeSize; ++i)
	codeSeg[i] = tlmmLookupCode(prog->codeSeg[i]);
    fwrite(codeSeg, sizeof(_code), prog->codeSize, fp);
    delete [] codeSeg;
#endif
    fwrite(prog->dataSeg, sizeof(float), prog->dataSize, fp);

    fclose(fp);
}
#endif/*TLMM_HAS_IO*/

bool tlmmLoadProgramBinary(tlmmProgram* prog, void* data, int sz)
{
    char* _data = (char*)data;
    tlmmHeader* header = (tlmmHeader*) _data;
    if(memcmp(header->id, ID, 4) != 0)
	return false;
    tlmmClearProgram(prog);
    prog->codeSize = header->codeSize;
    prog->dataSize = header->dataSize;

    _data += sizeof(tlmmHeader);

    prog->dataSeg = new float[prog->dataSize];
    int size = prog->codeSize * sizeof(_code);
#ifdef DEBUG
    prog->codeSeg = new _code[prog->codeSize];
    memcpy(prog->codeSeg, _data, size);
#else
    // convert to instructions
    _code* codeSeg = new _code[prog->codeSize];
    memcpy(codeSeg, _data, size);
    prog->codeSeg = new _instruction[prog->codeSize];
    for(int i=0; i<prog->codeSize; ++i)
	prog->codeSeg[i] = g_Instructions[i];
    delete [] codeSeg;
#endif
    _data += size;
    size = prog->dataSize * sizeof(float);
    memcpy(prog->dataSeg, _data, size);
    return true;
}

#ifdef TLMM_COMPILE
#include <string.h>
#include <vector>
void tlmmConvertRPN(std::vector<_code>* symbols)
{
    std::vector<_code> out;
    std::vector<_code> stack;
    
    for(std::vector<_code>::iterator iSym = symbols->begin();
	iSym != symbols->end();
	iSym++)
    {
	if(*iSym == _symbolCode("("))
	    stack.push_back(*iSym);
	else if( (*iSym >= _symbolCode("a") && *iSym <= _symbolCode("z")) ||
		*iSym == 255)
	    out.push_back(*iSym);
	else if(*iSym != _symbolCode(")")) // any symbol other than (, ), x or values
	{
	    if(stack.empty() ||
	       stack.back() == _symbolCode("(") ||
	       stack.back() > *iSym)
		stack.push_back(*iSym);
	    else
	    {
		do
		{
		    out.push_back(stack.back());
		    stack.pop_back();
		} while (!stack.empty() &&
			 stack.back() != _symbolCode("(") &&
			 stack.back() < *iSym);
		stack.push_back(*iSym);
	    }
	}
	else // close bracket
	{
	    while( stack.back() != _symbolCode("(") && // collapse until the last
		   !stack.empty() )                  // open bracket
	    {
		out.push_back(stack.back());
		stack.pop_back();
	    }
	    stack.pop_back();
	}
    }

    while(!stack.empty())
    {
	out.push_back(stack.back());
	stack.pop_back();
    }

    symbols->clear();
    for(std::vector<_code>::iterator iSym = out.begin();
	iSym != out.end();
	iSym++)
	symbols->push_back(*iSym);
}

tlmmReturn tlmmParseProgram(tlmmProgram* prog, const char* program)
{
    const char* p = program;
    std::vector<_code> syms;
    std::vector<float> consts;
    bool found;
    while(*p)
    {
	found = false;
	for(int i=0; i<256; ++i)
	    if(g_Symbols[i].hash != 0 && strstr(p, g_Symbols[i].sym) == p)
	    {
		syms.push_back(g_Symbols[i].code);
		p += strlen(g_Symbols[i].sym);
		found = true;
		break;
	    }
	if(found)
	    continue;	
	
	if(*p >= '0' && *p <= '9')
	{
	    syms.push_back(255);
	    consts.push_back((float)atof(p));
	    while( (*p >= '0' && *p <= '9') ||
		   *p == '.') ++p;
	}
	else
	{
	    // error and return
	    return TLMM_PARSE_ERROR;
	}
    }

    // if we've got here, we've successfully parsed the program
    // now we have to sort the symbols
    tlmmConvertRPN(&syms);
    tlmmClearProgram(prog);

    prog->eqn = new char[strlen(program)+1];
    sprintf(prog->eqn, program);

    int i = 0;
    prog->codeSize = syms.size();
#ifdef DEBUG
    prog->codeSeg = new _code[syms.size()];
    for(std::vector<_code>::iterator iSym = syms.begin();
	iSym != syms.end();
	iSym++)
	prog->codeSeg[i++] = *iSym;
#else
    prog->codeSeg = new _instruction[syms.size()];
    for(std::vector<_code>::iterator iSym = syms.begin();
	iSym != syms.end();
	iSym++)
	prog->codeSeg[i++] = g_Instructions[*iSym];
#endif/*DEBUG*/
    
    prog->dataSize = consts.size();
    prog->dataSeg = new float[consts.size()];
    i = 0;
    for(std::vector<float>::iterator iReg = consts.begin();
	iReg != consts.end();
	iReg++)
	prog->dataSeg[i++] = *iReg;

    return TLMM_SUCCESS;
    
}
#endif/*TLMM_COMPILE*/

tlmmReturn tlmmSetValue(tlmmProgram* prog, unsigned char variable, float value)
{
    if(variable < 'a' || variable > 'z')
	return TLMM_OUT_OF_RANGE;
    prog->registers[variable - 'a'] = value;
    return TLMM_SUCCESS;
}

float tlmmGet(tlmmProgram* prog)
{
    if(prog == 0)
	return 0.0f;
    _stack stack;
    _stack consts;
    stack.top = 0;
    consts.top = 0;
    for(int i = prog->dataSize - 1; i >= 0; --i)
	_stackPush(&consts, prog->dataSeg[i]);
    for(int i = 0; i < prog->codeSize; ++i)
    {
#ifdef DEBUG
	g_Instructions[prog->codeSeg[i]](&stack, &consts, prog->registers);
#else
	prog->codeSeg[i](&stack, &consts, prog->registers);
#endif/*DEBUG*/
    }
    return _stackPop(&stack);
}

float tlmmGetValue(tlmmProgram* prog, float ref)
{
    if(prog == 0)
	return 0.0f;
    prog->registers['x' - 'a'] = ref;
    return tlmmGet(prog);
}

#define INSTRUCTION(fn)						\
    void tlmmFunc##fn(_stack* stack, _stack* consts, float* regs)
#ifdef TLMM_COMPILE
# define REGISTER_FUNCS()			\
    int cnt = 1;				\
    int scnt = 0;				\
    g_Instructions = new _instruction[256];	\
    g_Symbols = new _sym[256];			\
    for(int i = 0; i < 256; ++i)		\
	g_Symbols[i].hash = 0;
# define FUNC(fn, sym)				\
    g_Instructions[cnt] = tlmmFunc##fn;		\
    _symbol(sym).code = cnt;			\
    cnt++;
# define SYM(sym)				\
    _symbol(sym).code = 254 - scnt;		\
    scnt ++;
#else
# define REGISTER_FUNCS()			\
    int cnt = 1;				\
    g_Instructions = new _instruction[256];
# define FUNC(fn, sym)				\
    g_Instructions[cnt] = tlmmFunc##fn;		\
    cnt++;
# define SYM(sym) {}
#endif/*TLMM_COMPILE*/
#define ACCESS(fn)				\
    g_Instructions[255] = tlmmFunc##fn;

#include <math.h>
INSTRUCTION(Add){ PUSH(POP() + POP()); }
INSTRUCTION(Del)
{
    float op2 = POP();
    float op1 = POP();
    PUSH(op1 - op2); 
}
INSTRUCTION(Mul){ PUSH(POP() * POP()); }
INSTRUCTION(Div)
{
    float op2 = POP();
    float op1 = POP();
    PUSH(op1 / op2); 
}
INSTRUCTION(Pow)
{
    float op2 = POP();
    float op1 = POP();
    PUSH(pow(op1, op2)); 
}
INSTRUCTION(Sqrt) { PUSH(sqrt(POP())); }
INSTRUCTION(Sin){ PUSH(sin(POP())); }
INSTRUCTION(Cos){ PUSH(cos(POP())); }
INSTRUCTION(Tan){ PUSH(tan(POP())); }
INSTRUCTION(Consts){ PUSH(CONST()); }
INSTRUCTION(A)  { PUSH(regs[0]); }
INSTRUCTION(B)  { PUSH(regs[1]); }
INSTRUCTION(C)  { PUSH(regs[2]); }
INSTRUCTION(D)  { PUSH(regs[3]); }
INSTRUCTION(E)  { PUSH(regs[4]); }
INSTRUCTION(F)  { PUSH(regs[5]); }
INSTRUCTION(G)  { PUSH(regs[6]); }
INSTRUCTION(H)  { PUSH(regs[7]); }
INSTRUCTION(I)  { PUSH(regs[8]); }
INSTRUCTION(J)  { PUSH(regs[9]); }
INSTRUCTION(K)  { PUSH(regs[10]); }
INSTRUCTION(L)  { PUSH(regs[11]); }
INSTRUCTION(M)  { PUSH(regs[12]); }
INSTRUCTION(N)  { PUSH(regs[13]); }
INSTRUCTION(O)  { PUSH(regs[14]); }
INSTRUCTION(P)  { PUSH(regs[15]); }
INSTRUCTION(Q)  { PUSH(regs[16]); }
INSTRUCTION(R)  { PUSH(regs[17]); }
INSTRUCTION(S)  { PUSH(regs[18]); }
INSTRUCTION(T)  { PUSH(regs[19]); }
INSTRUCTION(U)  { PUSH(regs[20]); }
INSTRUCTION(V)  { PUSH(regs[21]); }
INSTRUCTION(W)  { PUSH(regs[22]); }
INSTRUCTION(X)  { PUSH(regs[23]); }
INSTRUCTION(Y)  { PUSH(regs[24]); }
INSTRUCTION(Z)  { PUSH(regs[25]); }

class tlmmInit
{
public:
tlmmInit()
{
    REGISTER_FUNCS();
    SYM("(");
    SYM(")");
    FUNC(Sin, "sin");
    FUNC(Cos, "cos");
    FUNC(Tan, "tan");
    FUNC(Sqrt, "sqrt");
    FUNC(Pow, "^");
    FUNC(Mul, "*");
    FUNC(Div, "/");
    FUNC(Add, "+");
    FUNC(Del, "-");
    FUNC(A,   "a");
    FUNC(B,   "b");
    FUNC(C,   "c");
    FUNC(D,   "d");
    FUNC(E,   "e");
    FUNC(F,   "f");
    FUNC(G,   "g");
    FUNC(H,   "h");
    FUNC(I,   "i");
    FUNC(J,   "j");
    FUNC(K,   "k");
    FUNC(L,   "l");
    FUNC(M,   "m");
    FUNC(N,   "n");
    FUNC(O,   "o");
    FUNC(P,   "p");
    FUNC(Q,   "q");
    FUNC(R,   "r");
    FUNC(S,   "s");
    FUNC(T,   "t");
    FUNC(U,   "u");
    FUNC(V,   "v");
    FUNC(W,   "w");
    FUNC(X,   "x");
    FUNC(Y,   "y");
    FUNC(Z,   "z");
    ACCESS(Consts);
}
};
tlmmInit __init;

#ifndef TLMM_LEAN
namespace tlmm
{
    Program::Program()
    {
	m_prog  = tlmmInitProgram();
    }

    Program::~Program()
    {
	tlmmTerminateProgram(&m_prog);
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
	tlmmSaveProgram(m_prog, filename.c_str());
#endif/*TLMM_HAS_IO*/
    }

    void Program::Parse(std::string data)
    {
	tlmmParseProgram(m_prog, data.c_str());
    }

    float Program::GetValue(float ref)
    {
	return tlmmGetValue(m_prog, ref);
    }
}
#endif/*TLMM_LEAN*/
