/*
 * php_screw
 * (C) 2007, Kunimasa Noda/PM9.com, Inc. <http://www.pm9.com,  kuni@pm9.com>
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

#include "php_screw.h"
#include "my_screw.h"

PHP_MINIT_FUNCTION(php_screw);
PHP_MSHUTDOWN_FUNCTION(php_screw);
PHP_MINFO_FUNCTION(php_screw);

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

SCREWData pm9screw_ext_fopen(FILE *fp)
{
	struct	stat	stat_buf;
	SCREWData sdata;
	char	*datap = NULL;
	ULong datalen;
	int	cryptkey_len = sizeof pm9screw_mycryptkey / 2;
	int	i;

	fstat(fileno(fp), &stat_buf);
	datalen = stat_buf.st_size - PM9SCREW_LEN;
	datap = (char*)malloc(datalen);
	memset (datap, 0, datalen);
	fread(datap, datalen, 1, fp);
	fclose(fp);

	for(i=0; i<datalen; i++) {
		datap[i] = (char)pm9screw_mycryptkey[(datalen - i) % cryptkey_len] ^ (~(datap[i]));
	}

	sdata = screwdata_init ();
	sdata.buf = zdecode(datap, datalen, &sdata.len);

	free(datap);
	return sdata;
}

ZEND_API zend_op_array *(*org_compile_file)(zend_file_handle *file_handle, int type TSRMLS_DC);

ZEND_API zend_op_array *pm9screw_compile_file(zend_file_handle *file_handle, int type TSRMLS_DC)
{
	FILE	*fp;
	char	buf[PM9SCREW_LEN + 1] = { 0, };
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

	fread(buf, PM9SCREW_LEN, 1, fp);
	if (memcmp(buf, PM9SCREW, PM9SCREW_LEN) != 0) {
		fclose(fp);
		return org_compile_file(file_handle, type);
	}

	sdata = pm9screw_ext_fopen(fp);
	tmp = screwdata_init ();

	if ( zend_stream_fixup(file_handle, &tmp.buf, &tmp.len TSRMLS_CC) == FAILURE ) {
		return NULL;
	}

	file_handle->handle.stream.mmap.buf = sdata.buf;
	file_handle->handle.stream.mmap.len = sdata.len;
	file_handle->handle.stream.closer = NULL;

	return org_compile_file(file_handle, type TSRMLS_CC);
}

zend_module_entry php_screw_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"php_screw",
	NULL,
	PHP_MINIT(php_screw),
	PHP_MSHUTDOWN(php_screw),
	NULL,
	NULL,
	PHP_MINFO(php_screw),
#if ZEND_MODULE_API_NO >= 20010901
	"1.5.0", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(php_screw);

PHP_MINFO_FUNCTION(php_screw)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "php_screw support", "enabled");
	php_info_print_table_end();
}

PHP_MINIT_FUNCTION(php_screw)
{
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;

	org_compile_file = zend_compile_file;
	zend_compile_file = pm9screw_compile_file;
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(php_screw)
{
	CG(compiler_options) |= ZEND_COMPILE_EXTENDED_INFO;

	zend_compile_file = org_compile_file;
	return SUCCESS;
}
