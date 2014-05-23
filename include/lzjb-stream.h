/*
 *
*/

/* ----------------------------------------------------------------- */

typedef struct {
	void	*src;
	size_t	src_size;
} LZJBStream;

/* ----------------------------------------------------------------- */

/** @brief Encodes a size using a variable-length 7-bit format.
 *
 * The encoded format is endian-invariant, i.e. it will work the same
 * on all architectures.
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

/** @brief Initializes a stream for full streaming of both compressed and uncompressed data.
 *
 * After this call, the working buffer is assumed to be empty.
 *
 * @param stream	The stream to initialize.
 * @param dst_size	Number of destination (uncompressed) bytes that are going to be generated.
 * @param buf		Working buffer where compressed data will be held.
 * @param buf_size	Size of the working buffer.
*/
int lzjbstream_init(LZJBStream *stream, size_t dst_size, void *buf, size_t buf_size);


/** @brief Initializes a stream for reading, assuming all the compressed data is already available.
 *
 * Use @ref lzjbstream_read() to read uncompressed data from the stream.
 *
 * @param stream	The stream to initialize.
 * @param dst_size	Number of destination (uncompressed) bytes that are going to be generated.
 * @param src		Pointer the compressed data.
 * @param src_size	Number of bytes of compressed data.
*/
int lzjbstream_init_static(LZJBStream *stream, size_t dst_size, const void *src, size_t src_size);


int lzjbstream_buffer(LZJBStream *stream, const void *data, size_t data_size);


/** @brief Read (decompress) data from the stream.
 *
 * @param stream	The stream to read from.
 * @param buf		Buffer tor read into, will be filled with decompressed data.
 * @param buf_size	Maximum number of bytes that @ref buf will hold.
*/
int lzjbstream_read(LZJBStream *stream, void *buf, size_t buf_max);
