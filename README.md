# Feed Plus Plus [![Build Status](https://travis-ci.org/kamarya/feedpp.svg?branch=master)](https://travis-ci.org/kamarya/feedpp)
A C++ feed parser library adapted from [Newsbeuter](https://github.com/akrennmair/newsbeuter). The goal is reusing the mature feed parser code and modernize it
as a general purpose library.
## Build and Install

```
mkdir release
cd release
../configure
make
make check
```
If you do not find the configure script in the package
you may need to generate it by running ```autogen.sh```.

A set of unit tests are included in ```src/unitest.cpp``` that may
guide you as examples when using the library.

To install the library on your machine run ```make install```.
You may need to run ```ldconfig``` in order to update the shared library cache.


## License
The original and the modified works are licensed under
MIT/X Consortium License (refer to the file LICENSE
included in this software package)
