/*
 *
*/

#include <stdio.h>
#include <stdint.h>

#include "lzjb-stream.h"

/* ----------------------------------------------------------------- */

#define	SIZE_BITS	7
#define	SIZE_MASK	((1 << SIZE_BITS) - 1)
#define	SIZE_LAST	(SIZE_MASK + 1)

typedef enum {
	State_Empty = 0,

	State__Count
} State;

/* ----------------------------------------------------------------- */

void * lzjbstream_size_encode(void *out, size_t out_max, size_t size)
{
	uint8_t	*put = out, * const put_end  = put + out_max;

	while(put < put_end)
	{
		const uint8_t	here = size & SIZE_MASK;
		size >>= SIZE_BITS;
		if(size == 0)
		{
			*put++ = SIZE_LAST | here;
			return put;
		}
		*put++ = here;
	}
	return NULL;
}

const void * lzjbstream_size_decode(const void *in, size_t in_max, size_t *size)
{
	const uint8_t	*get = in, * const get_end = get + in_max;
	size_t			tmp = 0, mult = 1;

	while(get < get_end)
	{
		const uint8_t	here = *get++;

		if(here & SIZE_LAST)
		{
			tmp |= mult * (here & SIZE_MASK);
			if(size != NULL)
				*size = tmp;
			return get;
		}
		tmp |= mult * here;
		mult <<= SIZE_BITS;
	}
	return NULL;
}

/* ----------------------------------------------------------------- */

static size_t memory_read(size_t offset, void *out, size_t num_bytes, void *user)
{
	memcpy((uint8_t *) user + offset, out, num_bytes);
}

static size_t memory_write(size_t offset, const void *out, size_t num_bytes, void *user)
{
	memcpy((uint8_t *) user + offset, out, num_bytes);
}

bool lzjbstream_init_memory(LZJBStream *stream, void *dst, size_t dst_size);
{
	return lzjbstream_init_file(stream, dst_size, memory_read, memory_write, dst);
}

/* ----------------------------------------------------------------- */

bool lzjbstream_init_file(LZJBStream *stream, size_t dst_size, LZJBStreamRead reader, LZJBStreamWrite writer, void *user)
{
	if(stream == NULL || dst_size < 1 || reader == NULL || writer == NULL)
		return false;
	stream->dst_pos = 0;
	stream->dst_size = dst_size;
	stream->reader = reader;
	stream->writer = writer;
	stream->user = user;

	return true;
}
