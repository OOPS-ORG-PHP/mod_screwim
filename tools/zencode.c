#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include <string.h>

#define OUTBUFSIZ  100000

typedef uLong ULong; /* 32 bits or more */
typedef uInt UInt; /* 16 bits or more */

char *zcodecom(int mode, char *inbuf, ULong inbuf_len, ULong *resultbuf_len)
{
    z_stream z;
    char outbuf[OUTBUFSIZ];

    int count, status;
    char *resultbuf;
    ULong outbuf_size = OUTBUFSIZ + 1;
    ULong lastbuf_size = 0;

    z.zalloc = Z_NULL;
    z.zfree = Z_NULL;
    z.opaque = Z_NULL;

    z.next_in = Z_NULL;
    z.avail_in = 0;
    if (mode == 0) {
	deflateInit(&z, 1);
    } else {
	inflateInit(&z);
    }

    z.next_out = outbuf;
    z.avail_out = OUTBUFSIZ;
    z.total_out = 0;
    z.next_in = inbuf;
    z.avail_in = inbuf_len;

    resultbuf = malloc(OUTBUFSIZ + 1);

    while (1) {
	lastbuf_size = z.total_out;
	if (mode == 0) {
		status = deflate(&z, Z_FINISH);
	} else {
		status = inflate(&z, Z_NO_FLUSH);
	}
	count = OUTBUFSIZ - z.avail_out;

        if (status == Z_STREAM_END) {
	    if ( count > 0 ) {
		if ( outbuf_size <= z.total_out )
		    resultbuf = realloc(resultbuf, z.total_out + 1);
		memcpy(resultbuf + lastbuf_size, outbuf, count);
	    }
	    break;
        }

        if (status != Z_OK) {
	    if (mode == 0) {
		deflateEnd(&z);
	    } else {
		inflateEnd(&z);
	    }
	    *resultbuf_len = 0;
	    free (resultbuf);
	    resultbuf = NULL;
	    return resultbuf;
	}

        if ( z.avail_out < OUTBUFSIZ ) {
	    if ( outbuf_size <= z.total_out ) {
		resultbuf = realloc(resultbuf, z.total_out + OUTBUFSIZ + 1);
		outbuf_size = z.total_out + OUTBUFSIZ + 1;
		//printf ("realloc %d bytes!!\n", outbuf_size + 1);
	    }
	    memcpy(resultbuf+lastbuf_size, outbuf, count);
	    memset (outbuf, 0, OUTBUFSIZ);
	    z.next_out = outbuf;
	    z.avail_out = OUTBUFSIZ;
        }
    }

    if (mode == 0) {
	deflateEnd(&z);
    } else {
	inflateEnd(&z);
    }

    *resultbuf_len = z.total_out;
    resultbuf[z.total_out] = 0;

    return resultbuf;
}

char *zencode(char *inbuf, ULong inbuf_len, ULong *resultbuf_len)
{
	return zcodecom(0, inbuf, inbuf_len, resultbuf_len);
}

char *zdecode(char *inbuf, ULong inbuf_len, ULong *resultbuf_len)
{
	return zcodecom(1, inbuf, inbuf_len, resultbuf_len);
}
