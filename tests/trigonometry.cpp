#include "tlmm-tests.h"
#include "tlmm.h"

#define SIN2  0.909297407f
#define COS2 -0.416146845f
#define TAN2 -2.18503976f

TEST(Sin, Trigonometry, 0.0f,
     // initialisation
     {
	 m_data.program = tlmmInitProgram();
	 tlmmParseProgram(m_data.program, "sin(x)");
     },
     // cleanup
     {
	 tlmmTerminateProgram(&m_data.program);
     },
     // test
     {
	 ASSERT(tlmmGetValue(m_data.program, 2) == SIN2);
     },
     // data
     {
	 tlmmProgram* program;
     }
    );

TEST(Cos, Trigonometry, 0.0f,
     // initialisation
     {
	 m_data.program = tlmmInitProgram();
	 tlmmParseProgram(m_data.program, "cos(x)");
     },
     // cleanup
     {
	 tlmmTerminateProgram(&m_data.program);
     },
     // test
     {
	 ASSERT(tlmmGetValue(m_data.program, 2) == COS2);
     },
     // data
     {
	 tlmmProgram* program;
     }
    );

TEST(Tan, Trigonometry, 0.0f,
     // initialisation
     {
	 m_data.program = tlmmInitProgram();
	 tlmmParseProgram(m_data.program, "tan(x)");
     },
     // cleanup
     {
	 tlmmTerminateProgram(&m_data.program);
     },
     // test
     {
	 ASSERT(tlmmGetValue(m_data.program, 2) == TAN2);
     },
     // data
     {
	 tlmmProgram* program;
     }
    );
