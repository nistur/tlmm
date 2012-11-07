#ifndef __TLMM_TESTS_H__
#define __TLMM_TESTS_H__

#include "testsuite/tests.h"

#define TEST_TLMM(x, grp, time, fn, test)				\
    TEST(x, grp, time,							\
	 { m_data.program = tlmmInitProgram(); tlmmParseProgram(m_data.program, fn); }, \
	 {tlmmTerminateProgram(&m_data.program);},			\
	 test,								\
	 { tlmmProgram* program; });

#endif/*__TLMM_TESTS_H__*/
