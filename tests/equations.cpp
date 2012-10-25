#include "tlmm-tests.h"
#include "tlmm.h"

TEST_TLMM(Eqn1, Equations, 0.1f, "x^2+x+34",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 2) == 40);
	  }
    );

TEST_TLMM(Eqn2, Equations, 0.1f, "(x+2)*(x-4)*(12-x)",
	  {
	      ASSERT(tlmmGetValue(m_data.program, 2) == -80);
	  }
    );
