/** @mainpage lzjb-stream
 *
 * @file lzjb-stream.h
 *
*/

#if !defined LZJBSTREAM_H_
#define	LZJBSTREAM_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* ----------------------------------------------------------------- */

/** @brief Function pointer for a reading function, which reads already-decompressed bytes back. */
typedef uint8_t	(*LZJBStreamGetC)(size_t offset, void *user);

/** @brief Function pointer for a writing function, used to store newly-generated decompressed bytes. */
typedef void	(*LZJBStreamPutC)(size_t offset, uint8_t byte, void *user);

/** @brief The LZJB stream decompressor's state.
 *
 * This structure has no public fields: it is declared in public only to
 * support automatic allocations. Do *not* peekery-poke this struct.
*/
typedef struct {
	size_t		dst_pos;
	size_t		dst_size;
	LZJBStreamGetC	f_getc;
	LZJBStreamPutC	f_putc;
	void		*user;

	uint8_t		copymask;
	uint8_t		copymap;
	uint8_t		copyshift;
	uint8_t		copy0;
	bool		copynow;
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
