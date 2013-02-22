#include "php_ndata.h"
void ndata_init_NArray(TSRMLS_D);

PHP_METHOD(NArray, __construct);
PHP_METHOD(NArray, count);

typedef struct {
    zend_object std;
    ndata_type_t type;
    int count;
    int size;
    int current_size;
    void *data;
} ndata_array;
