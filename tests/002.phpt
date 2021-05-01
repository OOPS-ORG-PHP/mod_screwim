--TEST--
Check screwim_seed test
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
	if ( ! function_exists ('screwim_seed') ) {
		echo "yes\nyes";
		exit;
	}

	echo "no\nno";
	exit;
}

if ( ! function_exists ('screwim_seed') ) {
	echo "no\nno";
	exit;
}

echo "yes\n";

if ( screwim_seed ()->keystr == $ckey )
	echo 'yes';
else
	echo 'no';

?>
--EXPECT--
yes
yes
