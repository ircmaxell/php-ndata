NData
=====
A Native Data extension for PHP

Currently, a Nieve Native Array class is implemented.
This class stores values of a specified type in a native format.

The future of this library will attempt to provide tools to support working with native C data structures and data representations in a clean and easy to use manner. 

Benchmarks
==========

Compared to a regular PHP array (see benchmark.php), NArray appears to be significantly faster, and FAR more memory efficient. 

Using an array of 100,000 integers, building the array happens in about 1/3 of the time using 1/100th the memory.

Using an array of 100,000 booleans, building the array takes 1/2 the time, but uses 1/1000th the memory.

Usage
=====

The constructor takes two parameters, a type and a size:

    new NData\NArray($type, $size)

The type parameter is specified by one of the following constants:

 * **NData\NArray::TYPE_LONG** - For storing integers
 * **NData\NArray::TYPE_DOUBLE** - For storing floating point numbers
 * **NData\NArray::TYPE_BOOL** - For storing boolean values

The size parameter is the starting size of the memory block (in items). A size value of 0 will automatically re-size itself as the array grows. It will allocate dynamically in power-of-two increments (so the first resize will be from 1 to 2, the second 2 to 4, the third 4 to 8, etc). A negative number will allocate in increments of that number (the positive version). A positive number will fix and throw an exception if you attempt to add more items (like SPLFixedArray).

TODO
====

A **LOT**. Right now, only array access (read/write) works, and count is implemented. The rest, is to come...

Ideas
=====

Implement a hook into PDO/MySQLi to allow returning a NArray from a query (so working with large numeric data sets is far easier)...
