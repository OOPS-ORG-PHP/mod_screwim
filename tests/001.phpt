--TEST--
Check encrypt test
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
$ctx = <<<EOL
<?php
echo "I'm OK\n";
EOL;

$data = screwim_encrypt ($ctx);
$fp = fopen ('tests/001-sub.php', 'wb');
fwrite ($fp, $data, strlen ($data));
fclose ($fp);

ini_set ('screwim.enable', true);
require_once ('tests/001-sub.php');
#unlink ('tests/001-sub.php');

?>
--EXPECT--
I'm OK
