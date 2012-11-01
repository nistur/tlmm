#include "tlmm-tests.h"
#include "tlmm.h"

#define SIN2  0.909297407f
#define COS2 -0.416146845f
#define TAN2 -2.18503976f

TEST_TLMM(Sin, Trigonometry, 0.0f, "sin(x)",
	  ASSERT(tlmmGetValue(m_data.program, 2) == SIN2);
    );
 
TEST_TLMM(Cos, Trigonometry, 0.0f, "cos(x)",
	  ASSERT(tlmmGetValue(m_data.program, 2) == COS2);
    );

TEST_TLMM(Tan, Trigonometry, 0.0f, "tan(x)",
	  ASSERT(tlmmGetValue(m_data.program, 2) == TAN2);
    );

TEST_TLMM(Asin, Trigonometry, 0.0f, "asin(x)",
	  ASSERT(tlmmGetValue(m_data.program, SIN2) == 2);
    );

TEST_TLMM(Acos, Trigonometry, 0.0f, "acos(x)",
	  ASSERT(tlmmGetValue(m_data.program, COS2) == 2);
    );

TEST_TLMM(Atan, Trigonometry, 0.0f, "atan(x)",
	  ASSERT(tlmmGetValue(m_data.program, TAN2) == 2);
    );
 
