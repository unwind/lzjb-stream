/**
 * @file lzjb-stream-config.h
 *
 * Configuration header for the lzjb-stream library.
 * This file lets you do compile-time adaptation of how the library works.
 *
 * This file is in the public domain, it is considered part of your
 * application and might be moved out of the lzjb-stream distribution.
 *
*/

/** Undefine this and provide your own typedef or @c #define for the
 * booleans (type: @c bool, literals: @c true and @c false), if desired.
 *
 * The idea is to support compiling lzjb-stream on non-C99 compilers.
*/
#define LZJBSTREAM_WITH_STDBOOL
/*#undef LZJBSTREAM_WITH_STDBOOL*/


#if !defined LZJBSTREAM_WITH_STDBOOL
/* This is an example only. Feel free to delete and provide your own. */
typedef int bool;
#define true	1
#define	false	0
#endif
