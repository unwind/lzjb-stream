/*
 *
*/

#include <stdbool.h>
#include <stdlib.h>

/* ----------------------------------------------------------------- */

/** @brief The LZJB stream decompressor's state.
 *
 * This structure has no public fields: it is declared in public only to
 * support automatic allocations. Do *not* peekery-poke this struct.
*/
typedef struct {
	size_t	dst_pos;
	size_t	dst_size;
} LZJBStream;

typedef size_t	(*LZJBStreamRead)(size_t offset, void *out, size_t num_bytes, void *user);

typedef size_t	(*LZJBStreamWrite)(size_t offset, const void *out, size_t num_bytes, void *user);

/* ----------------------------------------------------------------- */

/** @brief Encodes a size using a variable-length format.
 *
 * The encoded format is endian-invariant, i.e. it will work the same
 * on all architectures. Being variable length, small sizes will encode
 * to fewer bytes.
 *
 * @param out		Buffer into which the encoded size will be written.
 * @param out_max	Maximum number of bytes available at @ref out.
 * @param size		The size to encode.
 * @return Pointer to first byte in @ref out after the encoded size.
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
 * @return Pointer to first byte in @ref in after the encoded size.
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
 *
 * @param stream	The stream to initialize.
 * @param reader	A user function to read *from the already-decompressed data*.
 * @param writer	A user function to write newly decompressed data to the destination.
 *
 * @return @c true on success, @c false on error (one or more parameter had an invalid value).
*/
bool lzjbstream_init_file(LZJBStream *stream, LZJBStreamRead reader, LZJBStreamWrite writer, void *user);


/** @brief Decompress a stream of data.
 *
 * @param stream	The stream to read from.
 * @param src		Compressed bytes to decompress.
 * @param src_size	Number of compressed bytes available at @ref src.
 *
 * @return The number of compressed bytes at @ref src that were processed. If this value is less than
 * @ref src_size, the remainder must be kept in the buffer and included in a later call to @ref lzjbstream_decompress().
*/
size_t lzjbstream_decompress(LZJBStream *stream, const void *src, size_t src_size);
