/*
 *
*/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "lzjb-stream.h"

/* ----------------------------------------------------------------- */

static struct {
	size_t	count;		/* Number of tests we've done. */
	size_t	pass_count;
	char	fail[256];
} test_state;

/* ----------------------------------------------------------------- */

static void test_passed(void)
{
	++test_state.count;
	++test_state.pass_count;
}

static void test_failed(const char *fmt, ...)
{
	++test_state.count;
	va_list args;
	va_start(args, fmt);
	vsnprintf(test_state.fail, sizeof test_state.fail, fmt, args);
	va_end(args);
	fprintf(stderr, "**%s\n", test_state.fail);
}

/* ----------------------------------------------------------------- */

/* Load the given file. Doesn't just use mmap() because reasons. */
static void * load_file(const char *filename, size_t *length)
{
	FILE	*in = fopen(filename, "rb");
	void	*buf = NULL;

	if(in != NULL)
	{
		if(fseek(in, 0, SEEK_END) >= 0)
		{
			const long size = ftell(in);
			if(size >= 0)
			{
				if(length != NULL)
					*length = (size_t) size;
				if(fseek(in, 0, SEEK_SET) == 0)
				{
					if((buf = malloc(size)) != NULL)
					{
						if(fread(buf, size, 1, in) != 1)
						{
							free(buf);
							buf = NULL;
						}
					}
				}
			}
		}
		fclose(in);
	}
	return buf;
}

/* ----------------------------------------------------------------- */

static void test_size(size_t size)
{
	char	buf[32];

	void * const encode_end = lzjbstream_size_encode(buf, sizeof buf, size);
	if(encode_end == NULL)
	{
		test_failed("Failed to encode %zu, out of buffer space\n", size);
		return;
	}
	const size_t encoded_length = (char *) encode_end - buf;
	size_t size_out;
	const void * const decode_end = lzjbstream_size_decode(buf, encoded_length, &size_out);
	if(decode_end == NULL)
	{
		test_failed("Failed to decode %zu from %zu bytes of encoded data, ran out of input\n", size, encoded_length);
		return;
	}
	if(size != size_out)
	{
		test_failed("Failed to decode %zu, got %zu instead (from %zu bytes)\n", size, size_out, encoded_length);
		return;
	}
	test_passed();
}

/* ----------------------------------------------------------------- */

static void test_decompress(void)
{
	/* Here's one I made earlier. */
	const uint8_t data[] = { 0x0, 0x5b, 0x27, 0x4c, 0x45, 0x4d, 0x50, 0x45, 0x4c, 0x0, 0x5f, 0x53, 0x49, 0x5a,
		0x45, 0x27, 0x2c, 0x20, 0x0, 0x27, 0x4d, 0x41, 0x54, 0x43, 0x48, 0x5f, 0x42, 0x88, 0x49, 0x54, 0x53,
		0x1c, 0xe, 0x4d, 0x41, 0x58, 0x20, 0xd, 0x84, 0x49, 0x4e, 0x1c, 0xd, 0x52, 0x41, 0x4e, 0x47, 0x8, 0x37,
		0x10, 0x4e, 0x42, 0x42, 0x59, 0x4, 0x17, 0x4f, 0x46, 0x46, 0x0, 0x53, 0x45, 0x54, 0x5f, 0x4d, 0x41,
		0x53, 0x4b, 0x1, 0x4, 0xf, 0x5f, 0x5f, 0x62, 0x75, 0x69, 0x6c, 0x74, 0x20, 0x69, 0x6e, 0x73, 0x5f,
		0x5f, 0xc, 0x10, 0x64, 0x6f, 0x42, 0x63, 0x14, 0xb, 0x66, 0x69, 0x6c, 0x65, 0x14, 0xc, 0x6e, 0x4,
		0x61, 0x6d, 0x18, 0xc, 0x70, 0x61, 0x63, 0x6b, 0x61, 0x0, 0x67, 0x65, 0x5f, 0x5f, 0x27, 0x2c, 0x20,
		0x27, 0x0, 0x63, 0x6f, 0x6d, 0x70, 0x72, 0x65, 0x73, 0x73, 0x0, 0x27, 0x2c, 0x20, 0x27, 0x64, 0x65,
		0x63, 0x6f, 0x0, 0x64, 0x65, 0x5f, 0x73, 0x69, 0x7a, 0x65, 0x27, 0x0, 0x2c, 0x20, 0x27, 0x64, 0x65,
		0x63, 0x6f, 0x6d, 0x0, 0x70, 0x72, 0x65, 0x73, 0x73, 0x27, 0x2c, 0x20, 0x0, 0x27, 0x65, 0x6e, 0x63,
		0x6f, 0x64, 0x65, 0x5f, 0x0, 0x73, 0x69, 0x7a, 0x65, 0x27, 0x5d
	};
	const char test_original[] = "['LEMPEL_SIZE', 'MATCH_BITS', 'MATCH_MAX', 'MATCH_MIN', 'MATCH_RANGE', 'NBBY', 'OFFSET_MASK', '__builtins__', '__doc__', '__file__', '__name__', '__package__', 'compress', 'decode_size', 'decompress', 'encode_size']";
	const size_t test_original_len = strlen(test_original);
	uint8_t tmp1[sizeof test_original] = { 0 }, tmp2[sizeof test_original] = { 0 }, tmp3[sizeof test_original] = { 0 };
	LZJBStream stream;

	/* First, decompress all at once. */
	lzjbstream_init_memory(&stream, tmp1, test_original_len);
	lzjbstream_decompress(&stream, data, sizeof data);
	const size_t len1 = strlen(tmp1);
	if(len1 == test_original_len && strcmp(tmp1, test_original) == 0)
		test_passed();
	else
		test_failed("Failed full-file streaming decompression, resulting len=%zu (expected %zu)", len1, sizeof test_original);

	/* Second, decompress a single byte (!) at a time. Mean. */
	lzjbstream_init_memory(&stream, tmp2, test_original_len);
	for(size_t i = 0; i < sizeof data; ++i)
	{
		const uint8_t	tmp = data[i];	/* Decompress ultra-slowly, feeding only a single byte at a time. */
		const size_t used = lzjbstream_decompress(&stream, &tmp, 1);
		if(used != 1)
		{
			printf("Call at %zu used %zu bytes\n", i, used);
			break;
		}
	}
	const size_t len2 = strlen(tmp2);
	if(len2 == test_original_len && strcmp(tmp2, test_original) == 0)
		test_passed();
	else
		test_failed("Failed 1-byte streaming decompression, resulting len=%zu (expected %zu)", len2, sizeof test_original);

	/* Third, decompress a random amount of bytes at a time. */
	lzjbstream_init_memory(&stream, tmp3, test_original_len);
	size_t pos = 0;
	while(!lzjbstream_is_finished(&stream))
	{
		int chunk = (rand() % 19 + 1);
		if(pos + chunk > sizeof data)
			chunk = sizeof data - pos;
		const size_t used = lzjbstream_decompress(&stream, data + pos, chunk);
		pos += used;
	}
	const size_t len3 = strlen(tmp3);
	if(len3 == test_original_len && strcmp(tmp3, test_original) == 0)
		test_passed();
	else
		test_failed("Failed random-chunked streaming decompression, resulting len=%zu (expected %zu)", len3, sizeof test_original);

}

int main(int argc, char *argv[])
{
	printf("Testing lzjb-stream's size codec API ...\n");
	/* Hard-coded edge cases. */
	test_size(0);
	test_size(~(size_t) 0);
	/* A single walking 1-bit. */
	for(size_t i = 1; i != 0; i <<= 1)
		test_size(i);
	for(size_t i = 0; i < 1000000; ++i)
	{
		test_size(rand());
	}

	printf("Testing lzjb-stream's decompression API ...\n");
	test_decompress();

	printf("%zu/%zu tests passed\n", test_state.pass_count, test_state.count);

	return EXIT_SUCCESS;
}
