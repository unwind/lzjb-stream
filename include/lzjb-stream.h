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
 *					If @c NULL, no size is stored but the decoding still happens.
 * @return Pointer to first byte in @ref in after the encoded size.
*/
const void * lzjbstream_size_decode(const void *in, size_t in_max, size_t *size);

/* ----------------------------------------------------------------- */

/** @brief Initializes a stream for "full" streaming of both compressed and uncompressed data.
 *
 * @param stream	The stream to initialize.
 * @param dst_size	Number of destination (uncompressed) bytes that are going to be generated.
*/
bool lzjbstream_init(LZJBStream *stream, size_t dst_size);


/** @brief Decompress a stream of data.
 *
 * @param stream	The stream to read from.
 * @param buf		Buffer to read into, will be filled with decompressed data.
 * @param buf_size	Maximum number of bytes that @ref buf will hold.
*/
size_t lzjbstream_decompress(LZJBStream *stream, const void *src, size_t src_size, void *buf, size_t buf_max);
