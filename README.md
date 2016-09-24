# Feed Plus Plus
This is a C++ feed parser library. It is originally extracted from
[Newsbeuter](https://github.com/akrennmair/newsbeuter)
project.
## Build and Install

```
mkdir release
cd release
../configure
make
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
