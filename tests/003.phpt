--TEST--
Check decrypt test
--SKIPIF--
<?php
if ( ! extension_loaded ('screwim') ) {
    print 'skip';
}
?>
--POST--
--GET--
--INI--
--FILE--
<?php
$conf = file ('./config.h');
$support_decrypt = 0;

foreach ( $conf as $line ) {
	if ( preg_match ('/^#define SCREWIM_DECRYPT/', $line) ) {
		$support_decrypt = 1;
	} else if ( preg_match ('/^#define SCREWIM_ENC_DATA /', $line) ) {
		$ckey = trim (preg_replace ('/#define SCREWIM_ENC_DATA\s+/', '', $line));
	}
}

if ( ! $support_decrypt ) {
	if ( ! function_exists ('screwim_decrypt') ) {
		echo "I'm OK";
		exit;
	}

	echo "Important ERROR: compiled unsupport decrypt, but support decrypt now!!";
	exit;
}

if ( ! function_exists ('screwim_decrypt') ) {
    echo "ERROR: unsupport screwim_decrypt";
    exit;
}

if ( ! file_exists ('tests/001-sub.php') ) {
	echo '001-sub.php not found';
	exit;
}

$data = file_get_contents ('tests/001-sub.php');

ini_set ('screwim.enable', true);
$code = screwim_decrypt ($data);
$code = preg_replace ('/^\s*(<\?(php)?|\?>)\s*$/m', '', $code);
eval ($code);

@unlink ('tests/001-sub.php');
?>
--EXPECT--
I'm OK
