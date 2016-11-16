/*
 * mod_screwim
 * (C) 2016, JoungKyun.Kim <http://oops.org>
 * see file LICENSE for license details
 */

#include "php.h"
#include "php_ini.h"
#include "ext/standard/file.h"
#include "ext/standard/info.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef unsigned long  ULong; /* 32 bits or more */

#include "php_screwim.h"
#include "my_screw.h"

PHP_MINIT_FUNCTION(screwim);
PHP_MSHUTDOWN_FUNCTION(screwim);
PHP_MINFO_FUNCTION(screwim);

typedef struct screw_data {
	char * buf;
	size_t len;
} SCREWData;

SCREWData screwdata_init (void) {
	SCREWData data;

	data.buf = NULL;
	data.len = 0;

	return data;
}

SCREWData screwim_ext_fopen(FILE *fp)
{
	struct	stat	stat_buf;
	SCREWData sdata;
	char	*datap = NULL;
	ULong datalen;
	int	cryptkey_len = sizeof screwim_mycryptkey / 2;
	int	i;

	fstat(fileno(fp), &stat_buf);
	datalen = stat_buf.st_size - SCREWIM_LEN;
	datap = (char*)malloc(datalen);
	memset (datap, 0, datalen);
	fread(datap, datalen, 1, fp);
	fclose(fp);

	for(i=0; i<datalen; i++) {
		datap[i] = (char)screwim_mycryptkey[(datalen - i) % cryptkey_len] ^ (~(datap[i]));
	}

	sdata = screwdata_init ();
	sdata.buf = zdecode(datap, datalen, &sdata.len);

	free(datap);
	return sdata;
}

ZEND_API zend_op_array *(*org_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);

ZEND_API zend_op_array *screwim_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
	FILE	*fp;
	char	buf[SCREWIM_LEN + 1] = { 0, };
	char	fname[32] = { 0, };
	SCREWData sdata, tmp;

	if (zend_is_executing(TSRMLS_C)) {
		if (get_active_function_name(TSRMLS_C)) {
			strncpy(fname, get_active_function_name(TSRMLS_C), sizeof fname - 2);
		}
	}
	if (fname[0]) {
		if ( strcasecmp(fname, "show_source") == 0
		  || strcasecmp(fname, "highlight_file") == 0) {
			return NULL;
		}
	}

	fp = fopen(file_handle->filename, "rb");
	if (!fp) {
		return org_compile_file(file_handle, type);
	}

	fread(buf, SCREWIM_LEN, 1, fp);
	if (memcmp(buf, SCREWIM, SCREWIM_LEN) != 0) {
		fclose(fp);
		return org_compile_file(file_handle, type);
	}

	sdata = screwim_ext_fopen(fp);
	tmp = screwdata_init ();

	if ( zend_stream_fixup(file_handle, &tmp.buf, &tmp.len TSRMLS_CC) == FAILURE ) {
		return NULL;
	}

	file_handle->handle.stream.mmap.buf = sdata.buf;
	file_handle->handle.stream.mmap.len = sdata.len;
	file_handle->handle.stream.closer = NULL;

	return org_compile_file(file_handle, type TSRMLS_CC);
}

zend_module_entry screwim_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"screwim",
	NULL,
	PHP_MINIT(screwim),
	PHP_MSHUTDOWN(screwim),
	NULL,
	NULL,
	PHP_MINFO(screwim),
#if ZEND_MODULE_API_NO >= 20010901
	"1.5.0", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(screwim);

PHP_MINFO_FUNCTION(screwim)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PHP SCREW Imporved support", "enabled");
	php_info_print_table_end();
}

PHP_MINIT_FUNCTION(screwim)
{
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;

	org_compile_file = zend_compile_file;
	zend_compile_file = screwim_compile_file;
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(screwim)
{
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;

	zend_compile_file = org_compile_file;
	return SUCCESS;
}
