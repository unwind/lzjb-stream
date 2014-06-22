/** @file lzjb-stream.h
 * 
 * @mainpage lzjb-stream
 *
 * This is lzjb-stream, a small library that supports streaming decompression of [LZJB-compressed](http://en.wikipedia.org/wiki/LZJB) data.
 * The library has very small memory requirements, basically on the order of 30 bytes or so.
 * The idea is that even small embedded systems shall be able to decompress LZJB data using this code.
 *
 * The name "stream" refers to the fact that the library does not assume that all compressed data is available when decompression starts.
 * Instead, you can feed the library any number of compressed bytes and it will decompress as much as possible.
 * You can feed it as little as a single byte at a time, or as much as the entire compressed file.
 *
 * ## Integration ##
 * The lzjb-stream library is written in platform-independent C.
 * It uses the @c bool datatype from C99; if your compiler does not support C99, see @ref lzjb-stream-config.h for a way to provide your own boolean type.
 *
 * ## Size encoding ##
 * The library provides convenience functions to encode and decode sizes (i.e. values of type `size_t`).
 * Please see @ref lzjbstream_size_encode() for the encoding function, and @ref lzjbstream_size_decode() for the decoder.
 *
 * ## Decompression ##
 * Decompressing data is done using the helper data structure @ref LZJBStream, which holds the state of the stream.
 * You can initialize and use any number of such streams, there is no additional memory overhead for each stream.
 *
 * Depending on your application's environment and requirements, you can use either memory-oriented or file-oriented streaming.
 * The two modes differ in how the streaming library writes its output (the decompressed data) and also how it re-reads such data.
 * Re-reading of already decompressed data is a crucial operation since the compression is achieved by back-referencing repeating data.
 *
 * @note The library does not assume any header in the compressed data, in particular you *must* know the size of the
 *       decompressed data from the beginning.
 *
 * <dl>
 * <dt>Memory-oriented streaming</dt>
 * <dd>
 * In memory-oriented streaming, the entire input (compressed data) is assumed to be available when the decompression runs.
 * To set up a memory-oriented stream, use @ref lzjbstream_init_memory().
 * </dd>
 *
 * <dt>File-oriented streaming</dt>
 * <dd>
 * In file-oriented streaming, writing output and reading already-written output is deferred to user-supplied functions.
 * To set up a file-oriented stream, use @ref lzjbstream_init_file().
 * </dd>
 * </dl>
 *
 * Once a stream has been initialized, all the application has to do is feed it compressed data to uncompress.
 * This is done using the @ref lzjbstream_decompress() function, which returns @c false when decompression is done.
 * You can also query a stream for completeness using the @ref lzjbstream_is_finished() function.
 *
*/
/* lzjb-stream.h
 *
 * Copyright (c) 2014, Emil Brink
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list
 *    of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 *    list of conditions and the following disclaimer in the documentation and/or
 *    other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
*/


#if !defined LZJBSTREAM_H_
#define	LZJBSTREAM_H_

#include "lzjb-stream-config.h"

#if defined LZJBSTREAM_WITH_STDBOOL
#include <stdbool.h>
#endif

#include <stdint.h>
#include <stdlib.h>

#define	LZJBSTREAM_VERSION	"1.0.0"		/**< Version number for lzjb-stream, as a major.minor.patch string. */

/* ----------------------------------------------------------------- */

/** @brief Function pointer for a reading function, which reads already-decompressed bytes back. */
typedef uint8_t	(*LZJBStreamGetC)(size_t offset, void *user);

/** @brief Function pointer for a writing function, used to store newly-generated decompressed bytes. */
typedef void	(*LZJBStreamPutC)(size_t offset, uint8_t byte, void *user);

/** @brief The LZJB stream decompressor's state.
 *
 * This structure has no public fields: it is declared in public only to
 * support automatic allocations. Do *not* peekery-poke this struct.
 *
*/
typedef struct {	/**  @cond INTERNAL */
	size_t		dst_pos;
	size_t		dst_size;
	LZJBStreamGetC	f_getc;
	LZJBStreamPutC	f_putc;
	void		*user;

	uint8_t		copymask;
	uint8_t		copymap;
	uint8_t		copyshift;
	uint8_t		copy0;
	bool		copynow;	/** @endcond INTERNAL */
} LZJBStream;

/* ----------------------------------------------------------------- */

/** @brief Encodes a size using a variable-length format.
 *
 * The encoded format is endian-invariant, i.e. it will work the same
 * on all architectures. Since it is variable-length, small sizes will
 * use fewer bytes when encoded.
 *
 * @param out		Buffer into which the encoded size will be written.
 * @param out_max	Maximum number of bytes available at out.
 * @param size		The size to encode.
 * @return Pointer to first byte in out after the encoded size.
 * If the given @c size doesn't fit in the buffer, the encoding fails
 * and returns @c NULL.
*/
void * lzjbstream_size_encode(void *out, size_t out_max, size_t size);


/** @brief Decodes an encoded size.
 *
 * @param in		Buffer from which the encoded size will be read.
 * @param in_max	Maximum number of bytes available to read.
 * @param size		Pointer to location where the final size will be stored.
 *
 * @return Pointer to first byte after the encoded size, in the input buffer.
*/
const void * lzjbstream_size_decode(const void *in, size_t in_max, size_t *size);

/* ----------------------------------------------------------------- */

/** @brief Initializes a stream for "memory" streaming, in which the entire destination buffer
 * is directly available as actual random-access memory.
 *
 * @param stream	The stream to initialize.
 * @param dst		Destination buffer.
 * @param dst_size	Size of destination buffer, number of uncompressed bytes to generate.
 *
 * @return @c true on success, @c false on error (one or more parameter had an invalid value).
*/
bool lzjbstream_init_memory(LZJBStream *stream, void *dst, size_t dst_size);


/** @brief Initializes a stream for "file" streaming, in which the I/O is deferred to user-supplied callbacks.
 * There is no built-in assumption that this reads or writes directly to any actual file, but the design
 * resembles standard I/O.
 *
 * Since the LZJB compressed format is very byte-oriented, the I/O functions read and write single bytes.
 * It is assumed that maximum performance is not the goal of using this library.
 *
 * @param stream	The stream to initialize.
 * @param dst_size	Number of uncompressed bytes we're going to generate.
 * @param file_getc	A pointer to a function that is used to *read back* one of the previously decompressed
 *			bytes. Note that this is *not* used to read in new compressed bytes; you are supposed
 *			to that between calls to @c lzjbstream_decompress().
 * @param file_putc	A pointer to a function that is used to write out a decompressed byte. Decompressed bytes
 *			will always be written in sequence, without gaps or jumps.
 * @param user		User-provided data pointer, which is passed to @c file_getc() and @c file_putc().
 *
 * @return @c true on success, @c false on error (one or more parameter had an invalid value).
*/
bool lzjbstream_init_file(LZJBStream *stream, size_t dst_size, LZJBStreamGetC file_getc, LZJBStreamPutC file_putc, void *user);


/** @brief Answers whether a given stream has finished decompressing.
 *
 * @param stream	The stream to query.
 *
 * @return @c true if the stream is finished, which means that the required number of destintion (output) bytes has been
 * generated. If we're not there yet, @c false is returned.
*/
bool lzjbstream_is_finished(const LZJBStream *stream);


/** @brief Decompress a stream of data.
 *
 * @param stream	The stream to decompress.
 * @param src		Compressed bytes to decompress.
 * @param src_size	Number of compressed bytes available at src. All of the provided bytes will always
 *			be fully consumed by this call.
 *
 * @return @c true if another call is needed, @c false if the requested number of output bytes has been generated.
*/
bool lzjbstream_decompress(LZJBStream *stream, const void *src, size_t src_size);

#endif		/* LZJBSTREAM_H_ */
