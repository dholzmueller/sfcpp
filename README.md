The sfcpp library is published under an Apache 2.0 license. If you use the sfc part of this project for research purposes, please cite one of the following publications:
David Holzmüller: Efficient Neighbor-Finding on Space-Filling Curves. Bachelor Thesis (2017)

The sfcpp library can be compiled by executing the top-level Makefile (make debug / make release). This Makefile executes cmake on CMakeLists.txt to generate a Makefile in the build folder, which is then automatically executed using make to compile the library. 
The modules img, sfc and the test code can be excluded from the build process. The test code requires all modules to be built and the img module requires the OpenCV libraries to be installed. The modules latex and sfc use the Color class of the img module.
The sfcpp library requires the libraries Eigen3 and boost. It has only been tested on an Ubuntu system, support for other systems is not guaranteed.


The sfcpp library contains the following modules:

cpp - currently unimplemented, contains some class stubs for C++ code generation

data - contains data structures

files - contains miscellaneous functions for file handling

geo - contains geometric functions:
- ConvexPolytope represents the face structure of a convex polytope
- QuickHullAlgorithm can compute such a polytope as the convex hull of a list of vertices
- coordinate system, point and vector classes for conversion between different coordinate systems and representation of 3D points and vectors

img - contains OpenCV wrappers for image processing

latex - contains classes for generation of LaTeX code and a submodule latex::tikz for generation of TikZ code

math - contains mathematical functions and classes representing mathematical objects

sfc - contains code for computations around space-filling curves:
- The KDCurveSpecification and CurveSpecification classes can be used to specify a curve
- The CurveInformation class computes informations about a CurveSpecification
- The CurveRenderer class can be used to generate TikZ graphics visualizing curves
- Several Algorithms classes provide optimized algorithms for different curves
- Some of the remaining classes are currently unimplemented because they were intended for code generation

strings - contains string helper functions

time - contains a Stopwatch class

A separate folder called "test" contains code that is compiled for execution and linked against the library. Currently, it contains code for rendering space-filling curves and making performance tests. The code there can also be used as an example of how to use the sfc module.



To run the script ./prepend_header.sh, modify it to only prepend the header in files that do not contain one and run
shopt -s globstar
beforehand


