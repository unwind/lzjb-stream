# lzjb-stream #
This is a streaming [LZJB](http://en.wikipedia.org/wiki/LZJB) decompressor.
"Streaming" means that it supports decompressing while reading: all of the input does not have to be be available at the same time.

Feature overview:

- Very low memory overhead: ~30 bytes when 32-bit, ~50 bytes when 64-bit.
- Does not do any heap allocations.
- Accepts any number of compressed data bytes at a time, down to single bytes.
- Written in portable C, builds as either C89 and C99.


## Licensing ##
This is open source software, and licensed under the [BSD 2-clause license](http://opensource.org/licenses/BSD-2-Clause).
Thus, lzjb-stream can be used in commercial software as long as the copyright notice is respected.


## Performance ##
The library is not optimized for performance, but for low memory overhead and simple code.
In its general mode, it does a function call per generated output byte, which of course is costly.
On the author's semi-ancient Core2 Q6600/2.4 GHz, it manages ~40 MB/s decompression speed.


## API Documentation ##
The API is annotated in the source using industry-standard [Doxygen](http://www.doxygen.org/) comments.
After you've cloned the project, run these commands to generate the documentation:

    $ cd doc
    $ doxygen ./lzjb-stream.doxyfile

Then point your browser at the resulting `doc/html/index.html` file.

In the meantime, if you're not bothered by the Doxygen markup, you can of course [read the header itself]().
