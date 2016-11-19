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
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef unsigned long  ULong; /* 32 bits or more */

#include "../php_screwim.h"
#include "../my_screw.h"

int main (int argc, char ** argv) {
	FILE   * fp;
	struct   stat stat_buf;
	char   * datap, * newdatap;
	ULong    datalen, newdatalen;
	int      cryptkey_len = sizeof screwim_mycryptkey / 2;
	char     newfilename[256];
	int      i;

	if ( argc != 2 ) {
		fprintf (stderr, "Usage: filename.\n");
		return 1;
	}
	fp = fopen (argv[1], "r");
	if ( fp == NULL ) {
		fprintf (stderr, "File not found(%s)\n", argv[1]);
		return 1;
	}

	fstat (fileno (fp), &stat_buf);
	datalen = stat_buf.st_size;
	datap = (char *) malloc (datalen + SCREWIM_LEN);
	fread (datap, datalen, 1, fp);
	fclose (fp);

	sprintf (newfilename, "%s.screw", argv[1]);

	if ( memcmp (datap, SCREWIM, SCREWIM_LEN) == 0 ) {
		fprintf (stderr, "Already Crypted(%s)\n", argv[1]);
		return 0;
	}

	newdatap = zencode (datap, datalen, &newdatalen);

	for( i=0; i<newdatalen; i++ ) {
		newdatap[i] = (char) screwim_mycryptkey[(newdatalen - i) % cryptkey_len] ^ (~(newdatap[i]));
	}

	fp = fopen (newfilename, "wb");
	if ( fp == NULL ) {
		fprintf (stderr, "Can not create crypt file(%s)\n", argv[1]);
		return 1;
	}
	fwrite (SCREWIM, SCREWIM_LEN, 1, fp);
	fwrite (newdatap, newdatalen, 1, fp);
	fclose (fp);
	fprintf (stderr, "Success Crypting(%s)\n", newfilename);
	free (newdatap);
	free (datap);

	return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
