/*
 *
*/

#include <stdio.h>

#include "lzjb-stream.h"

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

int main(int argc, char *argv[])
{
	printf("Testing lzjb-stream's size codec API ...\n");
	for(size_t i = 0; i < 10000; ++i)
	{
		char			buf[8];
		const size_t	size = rand();

		void *encode_end = lzjbstream_size_encode(buf, sizeof buf, size);
		if(encode_end == NULL)
		{
			printf("Failed to encode %zu, out of buffer space\n", size);
			return EXIT_FAILURE;
		}
		const size_t encoded_length = (char *) encode_end - buf;
		size_t size_out;
		const void *decode_end = lzjbstream_size_decode(buf, encoded_length, &size_out);
		if(decode_end == NULL)
		{
			printf("Failed to decode %zu from %zu bytes of encoded data, ran out of input\n", size, encoded_length);
			return EXIT_FAILURE;
		}
		if(size != size_out)
		{
			printf("Failed to decode %zu, got %zu instead (from %zu bytes)\n", size, size_out, encoded_length);
			return EXIT_FAILURE;
		}
		printf("%zu OK (%zu)\n", size, i);
	}
	printf("OK\n");
	return EXIT_SUCCESS;
}
