NData
=====
A Native Data extension for PHP

Currently, a Naive Native Array class is implemented.
This class stores values of a specified type in a native format.

The future of this library will attempt to provide tools to support working with native C data structures and data representations in a clean and easy to use manner. 

Benchmarks
==========

Compared to a regular PHP array (see benchmark.php), NArray appears to be significantly faster, and FAR more memory efficient. Compared to SplFixedArray, NArray is only slightly faster, but FAR more memory efficient in all cases.

### Memory

For Long (integers) and Double (floating points), NArray uses 1/5 the memory that SplFixedArray does, and 1/14 the memory that normal Arrays do.

For Booleans, NArray uses 1/335 the memory of SplFixedArray, and 1/877 the memory of Array...

### Time

For Long (integers) and Double (floating points), NArray finishes about 29% faster than SplFixedArray, and about twice as fast as a normal Array.

For Booleans, NArray finsihes just about even with SplFixedArray, and is about 33% faster than a normal Array

### Raw benchmark Results:

    Long NArray: 100000 In 0.013518095016479 Seconds Using 800312 Memory
    Long Array: 100000 In 0.029165029525757 Seconds Using 14648928 Memory
    Long FixedArray: 100000 In 0.017427921295166 Seconds Using 5600480 Memory

    Double NArray: 100000 In 0.015454053878784 Seconds Using 800312 Memory
    Double Array: 100000 In 0.024532079696655 Seconds Using 14649000 Memory
    Double FixedArray: 100000 In 0.017278909683228 Seconds Using 5600544 Memory

    Bool NArray: 100000 In 0.019473075866699 Seconds Using 12816 Memory
    Bool Array: 100000 In 0.025925874710083 Seconds Using 14648992 Memory
    Bool FixedArray: 100000 In 0.020848989486694 Seconds Using 5600544 Memory


Usage
=====

### Constructor 

`public __construct(int $type, int $size);`

The constructor takes two parameters, a type and a size:

    new NData\NArray($type, $size)

The type parameter is specified by one of the following constants:

 * **NData\NArray::TYPE_LONG** - For storing integers
 * **NData\NArray::TYPE_DOUBLE** - For storing floating point numbers
 * **NData\NArray::TYPE_BOOL** - For storing boolean values

The size parameter is the starting size of the memory block (in items). A size value of 0 will automatically re-size itself as the array grows. It will allocate dynamically in power-of-two increments (so the first resize will be from 1 to 2, the second 2 to 4, the third 4 to 8, etc). A negative number will allocate in increments of that number (the positive version). A positive number will fix and throw an exception if you attempt to add more items (like SPLFixedArray).

### Count

`public int count();`

Returns the current number of elements set in the array

### Iterating
`public mixed current();`
`public int key();`
`public void next();`
`public void rewind();`
`public void reset();`

A normal iterator...

TODO
====

A **LOT**. Right now, only array access (read/write) works, iterating and count is implemented. The rest, is to come...

Ideas
=====

Implement a hook into PDO/MySQLi to allow returning a NArray from a query (so working with large numeric data sets is far easier)...
