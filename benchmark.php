<?php

use NData\NArray;

function benchmark($typeName, callable $typeFunc, $type) {
    $a = function() use ($type) {
        return new NArray($type, 100000);
    };
    $b = function() { return array(); };
    $c = function() { 
        return new SplFixedArray(100000);
    };
    loop("$typeName NArray", $a, $typeFunc);
    loop("$typeName Array", $b, $typeFunc);
    loop("$typeName FixedArray", $c, $typeFunc);
    echo "\n";
}

function loop($name, $arrayFunc, $typeFunc) {
    $st = microtime(true);
    $s = memory_get_usage();
    $array = $arrayFunc();
    for ($i = 0; $i < 100000; $i++) {
        $array[$i] = $typeFunc($i);
    }
    $et = microtime(true);
    $e = memory_get_usage();
    echo "$name: " . count($array) . " In " . ($et - $st) . " Seconds Using " . ($e - $s) . " Memory\n";
}

benchmark("Long", function($i) { return $i; }, NArray::TYPE_LONG); 
benchmark("Double", function($i) { return (double) $i; }, NArray::TYPE_DOUBLE);
benchmark("Bool", function($i) { return (bool) $i % 2; }, NArray::TYPE_BOOL);
