/*
 *
*/

typedef struct {
	void	*src;
	size_t	src_size;
} LZJBStream;

/* @brief Initializes a stream for full streaming of both compressed and uncompressed data.
 *
 * After this call, the working buffer is assumed to be empty.
 *
 * @param stream	The stream to initialize.
 * @param buf		Working buffer where compressed data will be held.
 * @param buf_size	Size of the working buffer.
*/
int lzjbstream_init(LZJBStream *stream, void *buf, size_t bfer_size);


/* @brief Initializes a stream for reading, assuming all the compressed data is already available.
 *
 * Use @ref lzjbstream_read() to read uncompressed data from the stream.
 *
 * @param stream	The stream to initialize.
 * @param src		Pointer the compressed data.
 * @param src_size	Number of bytes of compressed data.
*/
int lzjbstream_init_static(LZJBStream *stream, const void *src, size_t src_size);
