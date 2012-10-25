#include "tlmm-tests.h"
#include "tlmm.h"

#define SQRT2 1.41421354f

TEST_TLMM(Addition, Arithmetic, 0.0f, "x+2",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 2) == 4);
	  }
    );

TEST_TLMM(Subtraction, Arithmetic, 0.0f, "x-2",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 2) == 0);
	  }
    );

TEST_TLMM(Multiplication, Arithmetic, 0.0f, "x*2",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 2) == 4);
	  }
    );

TEST_TLMM(Division, Arithmetic, 0.0f, "x/2",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 2) == 1);
	  }
    );

TEST_TLMM(Square, Arithmetic, 0.01f, "x^2",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 2) == 4);
	  }
    );

TEST_TLMM(Cube, Arithmetic, 0.01f, "x^3",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 2) == 8);
	  }
    );

TEST_TLMM(Sqrt, Arithmetic, 0.01f, "sqrt(x)",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 2) == SQRT2);
	  }
    );

TEST_TLMM(Parentheses, Arithmetic, 0.0f, "(x+2)*4",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 3) == 20);
	  }
    );
