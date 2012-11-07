TINY LITTLE MATHS MACHINE
=========================

ttml is a tiny little maths virtual machine written in C and C++. The reason behind this madness is that I wanted to be able to evaluate arbitrary expressions at run time, potentially graph them, and use them to tweak parameters in my projects.

Principles of tlmm
------------------

There are a few principles behind the development of tlmm:

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

Using tlmm within a program is easy, and an interface has been provided for both C and C++.

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

As I wrote this in a couple of hours and it is far from completed. The code within tlmm.cpp isn't neat. It isn't tidy. It's not optimised. The main thing at this stage is that it works. The rest can come later. The nice interface was the main goal.

Currently tlmm uses stl containers internally, so the size of tlmm-lite is a little larger than ideal (currently 12kb when building with config=release), however at this stage replacing the std::map and std::vector, and compiling tlmm-lite using C seems like too much hassle for a few kb gain.

How does tlmm work?
-------------------

Evaluating equations is a pain in the neck. Operator precedence complicates a lot of things and makes writing new operators painful. RPN is nice. RPN is your friend. tlmm first of all symbolises the equation as best it can using the supported symbols, then converts the symbols to RPN format. This means that not only can we just step through the instructions in order, but the operators are incredibly simple to write. The equation is compiled down to a list of 8 bit instructions (currently overkill for the few operations supported) and a list of 32 bit floating point constants, in order of their use in the equation. This means that the entire equation is a linear list of instructions at maximum a couple of bytes long.

Why not use __Insert script language here__
-------------------------------------------

The project this is originally written for has lua integrated already. Initially I was going to do a similar thing in lua instead, but plugging this into the existing lua state would have been, at best, a bit of a dirty hack. The other option is so create an entirely new instance of a scripting language to interpret equations for me. But that seems a bit like overkill, surely, as even pretty lean languages support more than arithmatric and trigonometric functions I need.

What bits of tlmm do I need
---------------------------

There are 4 components of the tlmm system that are built.

* tlmc - The tlmm compiler. This will read an equation and create a binary version. It can also produce a C header and header/source combination to compile into code.
* tlmm - The tlmm runtime. A standalone console application that can read and evaluate expressions.
* tlmm.a - The tlmm library. This contains code to evaluate expressions as well as compilation and file I/O
* tlmm-lite.a - Lightweight tlmm library. Doesn't support raw equations or file I/O. Lean and mean. Also C only
* tlmm.h - The C/C++ interface to tlmm. Everything you need is here.

Testing
-------

I've added a test suite (built, unsurprisingly as 'tests') This will run through a series of tests to check that tlmm is functioning properly. The tests are grouped by area. They are as follows:

* Basic - basic tlmm functionality, initialisation, parsing, numerical ranges etc
* Arithmetic - simple arithmetic operators, +-*/^() and checking that the precedence is respected
* Trigonometry - trigonometric functions such as sin() cos() tan()
* Equations - tests that sample equations will give back correct values. Stress tests

As well as the simple pass/fail the test suite is set up to profile each test, with an optional timeout for each function. It is worth noting that certain standard functions will do some internal initialisation during the first time they are called (eg pow()) so the first test might spike unnaturally. For this reason, a dummy test is carried out first before beginning any timed tests.

Porting
-------

Something I've become interested in with this project, with regards to it's size and it's reasonable performance, is porting it to other platforms. I have therefore begun experimenting with getting this working on anything possible. So far, this branch supports compilation on Windows, Linux and Mac OS X.

On top of this, and as a result of, among other things, premake4 not supporting changing compilers properly, I have created the branch `psp` which builds with the homebrew pspsdk, which can be installed part of devkitPro. I have tested it on a PSP-2000 and found the performance to be acceptable (40-60us for most tests)

`mtheall` also forked tlmm and created a Nintendo DS application using devkitARM. I will also add a DS branch to this repository once I can get devkitARM to work properly. I will also look into integrating mtheall's changes.

Any other platforms will be supported on a whim basis...

Oh, and the library code shouldn't change at all between the branches, just the build scripts (most consoles won't use premake4) and test suite.

Why do I actually need this?
----------------------------

To be honest, I don't really. I was wanting to create a tweak variable graph type for game tweaks, initially for use for audio manipulation. I have a direct mapping of points, with linear interpolation between them, and decided it would be useful to evaluate arbitrary expressions, so I could use more natural things like curved sine waves and bell curves to represent data. This will be integrated with an editor system that will render the equations as curves (probably by polling various values) and will support live changing of values, which will require it to be fast to evaluate.

Also, just because.