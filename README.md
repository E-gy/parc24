[![Build](https://github.com/E-gy/parc24/workflows/CMake/badge.svg?branch=master)](https://github.com/E-gy/parc24/actions?query=branch%3Amaster)
[![Coverage Status](https://coveralls.io/repos/github/E-gy/parc24/badge.svg?branch=master&t=hm2C4O)](https://coveralls.io/github/E-gy/parc24?branch=master)
# parc24
_Cross platform almost-bash-like on modern concepts_

It is 2 things
- a dynamic/static library for running commands/scripts programmatically
- an executable CLI wrapper for the wrapper (essentially a bash-like terminal)

What is *almost*-bourne-again-shell-*like* exactly?
- follow specification on the outside
- minus some features that weren't part of the scope of this project (`[`, `[[`, ...)
- minus legacy features, or them differently (aliases can alias only into command segments / other aliases for example)  
- using modern platfrom-independent concepts&features inside (single-proc, multi-threaded, fully-contextual\*,...)

---
PS: this project essentially served as a part of an assigned project within scope of validating some requirements. Therefore it has no reason to be maintained any longer. It is released in the public domain because why not. Sadly, both the outer project, and the requirements it was made for, are closed source and will remain this way.
