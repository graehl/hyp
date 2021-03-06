% Hypergraphs README

First, see `hyp-tutorial.pdf` provided in
this directory for your immediate gratification.

You can build the tutorial doc with
[TeX](https://www.tug.org/texlive/): `docs/hyp/hyp-tutorial.tex`.

Build with [CMake](http://www.cmake.org/download/) on the project
sources in the `sdl` directory, after setting environment variable
`SDL_EXTERNALS_PATH` to an appropriate checkout of the hypergraph
library dependencies (which you can build yourself from source if you
prefer, but you will need to use or modify our `FindXXXX.cmake`
scripts).

For example, on windows: `set SDL_EXTERNALS_PATH=c:/src/hypergraphs-externals/Windows` and then `cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 10 Win64" c:/src/hypergraphs/sdl`

Then, when you're ready to use or improve the source code for your own
projects, consider generating [Doxygen](http://www.doxygen.org/):
`doxygen doxy/xmtDoxy.conf`.

If you're willing to give us permission to use your source code, and
any patents required for it, we welcome pull requests. Keep your diffs
small, or talk to us before getting too crazy so we can save you some
wasted effort.
