# lzjb-stream #
This is a streaming [LZJB](http://en.wikipedia.org/wiki/LZJB) decompressor.
Streaming, here, means that it supports decompressing while reading: all of the input does not have to be be available at the same time.

Feature overview:

- Very low memory overhead.
- Does not do any heap allocations.
- Accepts any number of compressed data bytes at a time.


## API Documentation ##
The API is annotated in the source using industry-standard [Doxygen](http://www.stack.nl/~dimitri/doxygen/) comments.
After you've cloned the project, run these commands to generate the documentation:

    $ cd doc
    $ doxygen ./lzjb-stream.doxyfile

Then point your browser at the resulting `doc/html/index.html` file.
