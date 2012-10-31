#include "tlmm-tests.h"
#include "tlmm.h"

TEST_TLMM(VarParse, Variables, 0.0f, "",
	  {
	      ASSERT(tlmmParseProgram(m_data.program, "x+a") == TLMM_SUCCESS);
	  }
    );

TEST_TLMM(VarBounds, Variables, 0.0f, "",
	  {
	      
	      ASSERT(tlmmSetValue(m_data.program, 0, 2) == TLMM_OUT_OF_RANGE);
	      ASSERT(tlmmSetValue(m_data.program, 255, 2) == TLMM_OUT_OF_RANGE);
	  }
    );

TEST_TLMM(VarSet, Variables, 0.0f, "",
	  {
	      ASSERT(tlmmSetValue(m_data.program, 'a', 2) == TLMM_SUCCESS);
	  }
    );

TEST_TLMM(VarRun, Variables, 0.0f, "x+a",
	  {
	      ASSERT(tlmmSetValue(m_data.program, 'a', 2) == TLMM_SUCCESS);
	      ASSERT(tlmmGetValue(m_data.program, 3) == 5);
	  }
    );

TEST_TLMM(VarMultiple, Variables, 0, "a+b+c",
	  {
	      ASSERT(tlmmSetValue(m_data.program, 'a', 1) == TLMM_SUCCESS);
	      ASSERT(tlmmSetValue(m_data.program, 'b', 2) == TLMM_SUCCESS);
	      ASSERT(tlmmSetValue(m_data.program, 'c', 3) == TLMM_SUCCESS);
	      ASSERT(tlmmGet(m_data.program) == 6);
	  }
    );
