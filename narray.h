#include "php_ndata.h"
void ndata_init_NArray(TSRMLS_D);

PHP_METHOD(NArray, __construct);
PHP_METHOD(NArray, count);
PHP_METHOD(NArray, current);
PHP_METHOD(NArray, key);
PHP_METHOD(NArray, next);
PHP_METHOD(NArray, rewind);
PHP_METHOD(NArray, valid);


typedef struct {
    zend_object std;
    ndata_type_t type;
    int count;
    int size;
    int current_size;
    int offset;
    void *data;
} ndata_array;
