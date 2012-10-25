#include "tlmm-tests.h"
#include "tlmm.h"

TEST(InitTerminate, Basic, 0.0f,
     // initialisation
     {
	 m_data.program = 0;
     },
     // cleanup
     {
	 tlmmTerminateProgram(&m_data.program);
     },
     // test
     {
	 ASSERT(m_data.program = tlmmInitProgram());
	 ASSERT(tlmmTerminateProgram(&m_data.program) == TLMM_SUCCESS);
     },
     // data
     {
	 tlmmProgram* program;
     }
    );

TEST(Parse, Basic, 0.0f,
     // initialisation
     {
	 m_data.program = tlmmInitProgram();
     },
     // cleanup
     {
	 tlmmTerminateProgram(&m_data.program);
     },
     // test
     {
	 ASSERT(tlmmParseProgram(m_data.program, "x") == TLMM_SUCCESS);
     },
     // data
     {
	 tlmmProgram* program;
     }
    );


TEST(ParseComplex, Basic, 0.0f,
     // initialisation
     {
	 m_data.program = tlmmInitProgram();
     },
     // cleanup
     {
	 tlmmTerminateProgram(&m_data.program);
     },
     // test
     {
	 ASSERT(tlmmParseProgram(m_data.program, "(x*2)+12^x") == TLMM_SUCCESS);
     },
     // data
     {
	 tlmmProgram* program;
     }
    );

TEST_TLMM(Evaluate, Basic, 0.0f, "x",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 2) == 2);
	  }
    );

TEST_TLMM(Negative, Basic, 0.0f, "x",
	  {
	      ASSERT(tlmmGetValue(m_data.program, -2) == -2);
	  }
    );

TEST_TLMM(Fraction, Basic, 0.0f, "x",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 0.5f) == 0.5f);
	  }
    );

TEST_TLMM(Range, Basic, 0.0f, "x*2",
	  {
	      for(float f = -5; f <= 5; f += 0.25f)
		  ASSERT(tlmmGetValue(m_data.program, f) == f*2);
	  }
    );
