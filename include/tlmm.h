/*********************
 * TLMM
 * Tiny Little Maths Machine
 * Insert zlib license
 */
#ifndef __TLMM_H__
#define __TLMM_H__

#ifdef TLMM_HAS_IO
# include <stdio.h>
#endif/*TLMM_HAS_IO*/

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

struct tlmmProgram;

// Return values
typedef int tlmmReturn;
#define TLMM_SUCCESS      0
#define TLMM_PARSE_ERROR  1
#define TLMM_OUT_OF_RANGE 2


tlmmProgram* tlmmInitProgram();
tlmmReturn   tlmmTerminateProgram(tlmmProgram** prog);

// load functions
#ifdef TLMM_HAS_IO
void         tlmmLoadProgramFile   (tlmmProgram* prog, const char* filename);
void         tlmmLoadProgram       (tlmmProgram* prog, FILE* fp);
void         tlmmSaveProgram       (tlmmProgram* prog, const char* filename);
#endif/*TLMM_HAS_IO*/
tlmmReturn   tlmmParseProgram      (tlmmProgram* prog, const char* program);
bool         tlmmLoadProgramBinary (tlmmProgram* prog, void* data, int size);
    
tlmmReturn   tlmmSetValue          (tlmmProgram* prog, unsigned char variable, float value);
float        tlmmGetValue          (tlmmProgram* prog, float ref);
float        tlmmGet               (tlmmProgram* prog);
const char*  tlmmGetEquation       (tlmmProgram* prog);

#ifdef __cplusplus
}

#ifndef TLMM_LEAN
#include <string>

namespace tlmm
{
    class Program
    {
    public:
	Program();
	~Program();

	void Load(std::string filename);
	void Save(std::string filename);
	void Parse(std::string program);

	float GetValue(float ref);

    private:
	tlmmProgram* m_prog;
    };
}
#endif/*TLMM_LEAN*/
#endif/*__cplusplus*/

#endif/*__TLMM_H__*/
