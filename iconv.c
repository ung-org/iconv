/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2017, Jakob Kaivo <jkk@ung.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#define _XOPEN_SOURCE 700
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <unistd.h>

int convert(char *path, iconv_t conv, int skip, int suppress)
{
	(void)skip; (void)suppress;
	FILE *f = stdin;
	if (path && strcmp(path, "-")) {
		f = fopen(path, "rb");
	}

	if (f == NULL) {
		fprintf(stderr, "iconv: Couldn't open %s: %s\n", path ? path : "stdin", strerror(errno));
		return 1;
	}

	while (!feof(f)) {
		char *line = NULL;
		size_t n = 0;

		getline(&line, &n, f);
		
		char *out = calloc(n, 1);
		size_t nout = n;

		iconv(conv, &line, &n, &out, &nout);

		printf("%s", out);

		free(out);
		free(line);
	}

	if (f != stdin) {
		fclose(f);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int skipinvalid = 0;
	int list = 0;
	int suppress = 0;
	char *fromcodeset = NULL;
	char *tocodeset = NULL;

	int c;
	while ((c = getopt(argc, argv, "csf:t:l")) != -1) {
		switch (c) {
		case 'c':	/** skip invalid input characters **/
			skipinvalid = 1;
			break;

		case 'f':	/** convert from [fromcodeset] **/
			fromcodeset = optarg;
			break;

		case 'l':	/** list supported codesets **/
			list = 1;
			break;

		case 's':	/** suppress invalid character warnings **/
			suppress = 1;
			break;

		case 't':	/** convert to [tocodeset] **/
			tocodeset = optarg;
			break;

		default:
			return 1;
		}
	}

	if (list) {
		/* list all code sets */
		return 0;
	}

	iconv_t conv = iconv_open(tocodeset, fromcodeset);

	int r = 0;
	do {
		r |= convert(argv[optind++], conv, skipinvalid, suppress);
	} while (optind < argc);
}
