/*
 *
*/

#include <stdarg.h>
#include <stdio.h>

#include "lzjb-stream.h"

static struct {
	size_t	count;		/* Number of tests we've done. */
	size_t	pass_count;
	char	fail[256];
} test_state;

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
}

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
	printf("%zu/%zu tests passed\n", test_state.pass_count, test_state.count);

	return EXIT_SUCCESS;
}
