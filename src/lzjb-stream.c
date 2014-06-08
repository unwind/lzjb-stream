/*
 *
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lzjb-stream.h"

/* ----------------------------------------------------------------- */

#define	BITS_PER_BYTE	8

#define	SIZE_BITS	7
#define	SIZE_MASK	((1 << SIZE_BITS) - 1)
#define	SIZE_LAST	(SIZE_MASK + 1)

#define	MATCH_BITS	6
#define	MATCH_MIN	3
#define	MATCH_MAX	((1 << MATCH_BITS) + (MATCH_MIN - 1))
#define	OFFSET_MASK	((1 << (16 - MATCH_BITS)) - 1)
#define	LEMPEL_SIZE	1024

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

static uint8_t memory_getc(size_t offset, void *user)
{
	return ((uint8_t *) user)[offset];
}

static void memory_putc(size_t offset, uint8_t byte, void *user)
{
	((uint8_t *) user)[offset] = byte;
}

bool lzjbstream_init_memory(LZJBStream *stream, void *dst, size_t dst_size)
{
	return lzjbstream_init_file(stream, dst_size, memory_getc, memory_putc, dst);
}

/* ----------------------------------------------------------------- */

bool lzjbstream_init_file(LZJBStream *stream, size_t dst_size, LZJBStreamGetC file_getc, LZJBStreamPutC file_putc, void *user)
{
	if(stream == NULL || dst_size < 1 || getc == NULL || putc == NULL)
		return false;

	stream->dst_pos = 0;
	stream->dst_size = dst_size;
	stream->f_getc = file_getc;
	stream->f_putc = file_putc;
	stream->user = user;

	stream->copymask = 0;
	stream->copyshift = 0;
	stream->copy_now = false;

	return true;
}

/* ----------------------------------------------------------------- */

bool lzjbstream_is_finished(const LZJBStream *stream)
{
	if(stream != NULL)
		return stream->dst_pos >= stream->dst_size;
	return false;
}

/* ----------------------------------------------------------------- */

/* Execute a copy, which is when new output bytes are "created" by re-using existing ones. */
static void do_copy(LZJBStream *stream, uint8_t get0, uint8_t get1)
{
	int mlen = (get0 >> (BITS_PER_BYTE - MATCH_BITS)) + MATCH_MIN;
	const int offset = ((get0 << BITS_PER_BYTE) | get1) & OFFSET_MASK;
	size_t copy_from = stream->dst_pos - offset;

//	printf(" doing a %d-byte copy from offset %d\n", mlen, offset);

	for(; mlen > 0; --mlen)
	{
		const uint8_t tmp = stream->f_getc(copy_from++, stream->user);
//		printf("  copying %x ('%c')\n", tmp, tmp);
		stream->f_putc(stream->dst_pos++, tmp, stream->user);
	}
}

bool lzjbstream_decompress(LZJBStream *stream, const void *src, size_t src_size)
{
	const uint8_t	*get = src, * const get_end = get + src_size;

	if(stream == NULL || src == NULL || src_size == 0)
		return false;
	if(stream->dst_pos >= stream->dst_size)
		return false;

	/* If a previous call failed to do a copy due to lack of data, complete it now that we have at least 1 more byte. */
	if(stream->copy_now)
	{
		do_copy(stream, stream->copy_first, get[0]);
		++get;
		stream->copy_now = false;
		stream->copyshift = 1;
	}

	while(get < get_end)
	{
//		printf("%zu bytes of input left: copymap=0x%02x, copymask=0x%02x, shift=%u, copy_now=%s\n",
//			get_end - get, stream->copymap, stream->copymask, stream->copyshift, stream->copy_now ? "true" : "false");
		stream->copymask <<= stream->copyshift;
		if(stream->copymask == 0)
		{
			stream->copymap = *get++;
			stream->copymask = 1;
			stream->copyshift = 0;
//			printf("loaded copymap 0x%02x\n", stream->copymap);
		}
		if(get >= get_end)
			break;
		if(stream->copymap & stream->copymask)
		{
			if(get_end - get >= 2)	/* Bytes available to read mlen and offset? */
			{
				do_copy(stream, get[0], get[1]);
				get += 2;
			}
			else
			{
//				printf(" deferring copy, insufficient input bytes\n");
				stream->copy_first = *get++;
				stream->copy_now = true;
				stream->copyshift = 0;
				break;		/* Exit, finish this next time. */
			}
		}
		else
		{
//			printf("doing 1-byte write to %zu: 0x%02x\n", stream->dst_pos, *get);
			stream->f_putc(stream->dst_pos++, *get++, stream->user);
		}
		stream->copyshift = 1;
	}
	return stream->dst_pos < stream->dst_size;
}
