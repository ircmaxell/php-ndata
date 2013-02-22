<?php

use NData\NArray;

$st = microtime(true);
$s = memory_get_usage();

/**
 * Constructor takes two parameters.
 *
 * A type specifier (LONG, DOUBLE or BOOL)
 *   - Right now, ALL values MUST be of said type otherwise an exception is raised
 * A size specifier
 *   - A size of -1 will auto-resize, doubling the size each time
 */
$a = new NArray(NArray::TYPE_BOOL, -100000);
for ($i = 0; $i < 100000; $i++) {
    $a[] = (bool) $i % 2;
}
$et = microtime(true);
$e = memory_get_usage();

echo "Native: " . count($a) . " In " . ($et - $st) . " Using " . ($e - $s) . " Memory\n";

$st = microtime(true);
$s = memory_get_usage();
$b = array();
for ($i = 0; $i < 100000; $i++) {
    $b[] = (bool) $i % 2;
}
$et = microtime(true);
$e = memory_get_usage();

echo "Array: " . count($b) . " In " . ($et - $st) . " Using " . ($e - $s) . " Memory\n";
