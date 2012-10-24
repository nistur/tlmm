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

tlmmProgram* tlmmInitProgram();
void         tlmmTerminateProgram(tlmmProgram* prog);

// load functions
#ifdef TLMM_HAS_IO
void         tlmmLoadProgramFile   (tlmmProgram* prog, const char* filename);
void         tlmmLoadProgram       (tlmmProgram* prog, FILE* fp);
void         tlmmSaveProgram       (tlmmProgram* prog, const char* filename);
#endif/*TLMM_HAS_IO*/
void         tlmmParseProgram      (tlmmProgram* prog, const char* program);
bool         tlmmLoadProgramBinary (tlmmProgram* prog, void* data, int size);

float        tlmmGetValue          (tlmmProgram* prog, float ref);

#ifdef __cplusplus
}

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
#endif/*__cplusplus*/

#endif/*__TLMM_H__*/
