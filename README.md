PL/0 and PL/0C

PL/0 is the original compiler/interpreter from Niklaus Wirth's Algorithms + 
Data Structures = Programs, but modified to produce a listing with the correct
level value, and not just '1'.

PL/0C is another compiler/interpreter, inspired by the original PL/0 language 
and machine (above), but written in C++ and modified to use C operators, e.g., 
check for equality with "==" and assigned with "=", but otherwise using pl/0
syntax and program nesting. The result is a dialect that I call "PL/0C".

The Makefile for pl/0 is incomplete, but p0com.p builds with Free Pascal as long
as the ISO dialect (-M ISO) is set.

The Makefile for pl/0c is complete, try "make help" for information on how to
build and generate documentation via Doxygen.

Randy Merkel, Slowly but Surly Software. 
rmerkel@acm.org

Copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
