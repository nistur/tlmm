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

struct tlmmProgram;

tlmmProgram* tlmmInitProgram();
void         tlmmTerminateProgram(tlmmProgram* prog);

// load functions
#ifdef TLMM_HAS_IO
void         tlmmLoadProgram (tlmmProgram* prog, const char* filename);
void         tlmmLoadProgram (tlmmProgram* prog, FILE* fp);
void         tlmmSaveProgram (tlmmProgram* prog, const char* filename);
#endif/*TLMM_HAS_IO*/
void         tlmmLoadProgram (tlmmProgram* prog, void* data, unsigned int size);
void         tlmmParseProgram(tlmmProgram* prog, const char* program);

float        tlmmGetValue    (tlmmProgram* prog, float ref);

#endif/*__TLMM_H__*/
