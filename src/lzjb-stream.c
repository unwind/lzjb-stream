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

bool lzjbstream_init(LZJBStream *stream, size_t dst_size, void *buf, size_t buf_size)
{
	if(stream == NULL || buf == NULL || buf_size < 1)
		return false;

	stream->buf = buf;
	stream->buf_pos = 0;
	stream->buf_size = buf_size;
	stream->dst_pos = 0;
	stream->dst_size = dst_size;

	return true;
}
