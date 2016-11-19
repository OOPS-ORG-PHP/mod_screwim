/*
 * Copuright (c) 2016 JoungKyun.Kim <http://oops.org>. All rights reserved.
 *
 * This file is part of mod_screwim
 *
 * This program is forked from PHP screw who made by Kunimasa Noda in
 * PM9.com, Inc. and, his information is follows:
 *   http://www.pm9.com
 *   kuni@pm9.com
 *   https://sourceforge.net/projects/php-screw/
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <string.h>

// for emalloc and erealloc
#include "php.h"

#define OUTBUFSIZ  100000

typedef uLong ULong; /* 32 bits or more */

char * zcodecom (int mode, char * inbuf, ULong inbuf_len, ULong * resultbuf_len)
{
	z_stream   z;
	char       outbuf[OUTBUFSIZ];

	int        count, status;
	char     * resultbuf, * rbuf_new = NULL;
	ULong      outbuf_size = OUTBUFSIZ + 1;
	ULong      lastbuf_size = 0;

	z.zalloc   = Z_NULL;
	z.zfree    = Z_NULL;
	z.opaque   = Z_NULL;

	z.next_in  = Z_NULL;
	z.avail_in = 0;

	if ( mode == 0 ) {
		deflateInit (&z, 1);
	} else {
		inflateInit (&z);
	}

	z.next_out  = (unsigned char *) outbuf;
	z.avail_out = OUTBUFSIZ;
	z.total_out = 0;
	z.next_in   = (unsigned char *) inbuf;
	z.avail_in  = inbuf_len;

	if ( (resultbuf = emalloc (OUTBUFSIZ + 1)) == NULL ) {
		php_error_docref (
			NULL, E_COMPILE_ERROR,
			"ScrewIm %scode error. Memory allocation failed.",
			mode ? "de" : "en"
		);
		return NULL;
	}

	while ( 1 ) {
		lastbuf_size = z.total_out;
		if ( mode == 0 ) {
			status = deflate (&z, Z_FINISH);
		} else {
			status = inflate (&z, Z_NO_FLUSH);
		}
		count = OUTBUFSIZ - z.avail_out;

		if ( status == Z_STREAM_END ) {
			if ( count > 0 ) {
				if ( outbuf_size <= z.total_out ) {
					if ( (rbuf_new = erealloc (resultbuf, z.total_out + 1)) != NULL ) {
						resultbuf = rbuf_new;
					} else {
						efree (resultbuf);
						php_error_docref (
							NULL, E_COMPILE_ERROR,
							"ScrewIm %scode error. Memory allocation failed.",
							mode ? "de" : "en"
						);
						return NULL;
					}
				}
				memcpy (resultbuf + lastbuf_size, outbuf, count);
			}
			break;
		}

		if ( status != Z_OK ) {
			char * errmsg;

			if ( mode == 0 ) {
				deflateEnd (&z);
			} else {
				inflateEnd (&z);
			}

			*resultbuf_len = 0;

			if ( resultbuf != NULL )
				efree (resultbuf);

			switch (status) {
				case Z_BUF_ERROR :
					errmsg = "No progress is possible; either avail_in or avail_out was zero.";
					break;
				case Z_MEM_ERROR :
					errmsg = "Insufficient memory.";
					break;
				case Z_STREAM_ERROR :
					errmsg = "The state (as represented in stream) is inconsistent, or stream was NULL.";
					break;
				default : // Z_NEED_DICT
					errmsg = "A preset dictionary is required. The adler field shall be set to the Adler-32 checksum of the dictionary chosen by the compressor. In this case, you should suspect that ScrewIm's encode key is not correct.";
			}

			php_error_docref (NULL, E_COMPILE_ERROR, "ScrewIm %scode error. %s", mode ? "de" : "en", errmsg);
			return NULL;
		}

		if ( z.avail_out < OUTBUFSIZ ) {
			if ( outbuf_size <= z.total_out ) {
				if ( (rbuf_new = erealloc (resultbuf, z.total_out + OUTBUFSIZ + 1)) != NULL ) {
					resultbuf = rbuf_new;
				} else {
					efree (resultbuf);
					php_error_docref (
						NULL, E_COMPILE_ERROR,
						"ScrewIm %scode error. Memory allocation failed.",
						mode ? "de" : "en"
					);
					return NULL;
				}
				outbuf_size = z.total_out + OUTBUFSIZ + 1;
				//printf ("realloc %d bytes!!\n", outbuf_size + 1);
			}
			memcpy (resultbuf+lastbuf_size, outbuf, count);
			memset (outbuf, 0, OUTBUFSIZ);
			z.next_out  = (unsigned char *) outbuf;
			z.avail_out = OUTBUFSIZ;
		}
	}

	if ( mode == 0 ) {
		deflateEnd (&z);
	} else {
		inflateEnd (&z);
	}

	*resultbuf_len = z.total_out;
	resultbuf[z.total_out] = 0;

	return resultbuf;
}

char * zencode (char * inbuf, ULong inbuf_len, ULong * resultbuf_len) {
	return zcodecom (0, inbuf, inbuf_len, resultbuf_len);
}

char * zdecode (char * inbuf, ULong inbuf_len, ULong * resultbuf_len) {
	return zcodecom (1, inbuf, inbuf_len, resultbuf_len);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
