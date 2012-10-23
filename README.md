TINY LITTLE MATHS MACHINE
=========================

ttml is a tiny little maths virtual machine written in C and C++. The reason behind this madness is that I wanted to be able to evaluate arbitrary expressions at run time, potentially graph them, and use them to tweak parameters in my projects.

Principles of ttml
------------------

There are a few principles behind the development of ttml:

* The API should be simple, clean and easy to use (in both C and C++)
* The library should be small
* The expressions should be quick to evaluate
* Expressions and programs should be able to be changed dynamically for easier tweaking.

On top of these, a few more things came up.

* It would be nice to have compiled equations to allow faster loading.
* The system should behave identically regardless of whether the input equation was compiled or raw.

Having compiled equations makes it seem a little redundant to have a non-code based solution for variable tweaking. The idea would be that they are interchangable and if a production system required the marginally faster initialisation time, could use the compiled binary data whereas the development system would load raw equations.

Using tlmm
----------

~~~~~~ C
    tlmmProgram* prog = tlmmInitProgram();
    tlmmParseProgram(prog, "x^2+x+12");
    float y = tlmmGetValue(prog, 2);
    tlmmTerminateProgram(prog);
~~~~~~

~~~~~~ C++
    tlmm::Program prog;
    prog.Parse("x+2");
    float y = prog.GetValue(2);
~~~~~~

Notes
-----

tlmm is in development and is by no means complete. It's released on Github simply so people can look and give their feedback, it doesn't mean that I believe it's a completed product that people can widely use yet.

Firstly, there are no trigonometric functions implemented yet. sin, cos, tan etc are to come soon. sqrt will be implemented also, with the compile time choice betweenstandard sqrt and fast inverse sqrt. At this point in time, parentheses do not work as intended.

tlmc and tlmm currently have placeholder programs to test and demonstrate the tlmm library and don't function properly.

As I wrote this in a couple of hours and it is far from completed. The code within tlmm.cpp isn't neat. It isn't tidy. It's not optimised. The main thing at this stage is that it works. The rest can come later. The nice interface was the main goal.

How does tlmm work?
-------------------

Evaluating equations is a pain in the neck. Operator precedence complicates a lot of things and makes writing new operators painful. RPN is nice. RPN is your friend. tlmm first of all symbolises the equation as best it can using the supported symbols, then converts the symbols to RPN format. This means that not only can we just step through the instructions in order, but the operators are incredibly simple to write. The equation is compiled down to a list of 8 bit instructions (currently overkill for the few operations supported) and a list of 32 bit floating point constants, in order of their use in the equation. This means that the entire equation is a linear list of instructions at maximum a couple of bytes long.

Why not use /Insert script language here/
-----------------------------------------

The project this is originally written for has lua integrated already. Initially I was going to do a similar thing in lua instead, but plugging this into the existing lua state would have been, at best, a bit of a dirty hack. The other option is so create an entirely new instance of a scripting language to interpret equations for me. But that seems a bit like overkill, surely, as even pretty lean languages support more than arithmatric and trigonometric functions I need.

What bits of tlmm do I need
---------------------------

There are 4 components of the tlmm system that are built.

* tlmc - The tlmm compiler. This will read an equation and create a binary version. It can also produce a C header and header/source combination to compile into code.
* tlmm - The tlmm runtime. A standalone console application that can read and evaluate expressions.
* tlmm.a - The tlmm library. This contains code to evaluate expressions as well as compilation and file I/O
* tlmm-lite.a - Lightweight tlmm library. Doesn't support raw equations or file I/O. Lean and mean.
* tlmm.h - The C/C++ interface to tlmm. Everything you need is here.

Why do I actually need this?
----------------------------

To be honest, I don't really. I was wanting to create a tweak variable graph type for game tweaks, initially for use for audio manipulation. I have a direct mapping of points, with linear interpolation between them, and decided it would be useful to evaluate arbitrary expressions, so I could use more natural things like curved sine waves and bell curves to represent data. This will be integrated with an editor system that will render the equations as curves (probably by polling various values) and will support live changing of values, which will require it to be fast to evaluate.

Also, just because.