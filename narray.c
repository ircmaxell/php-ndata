#include <php.h>
#include "php_ndata.h"
#include "zend_exceptions.h"
#include "spl_exceptions.h"
a
extern zend_object_handlers narray_handlers;

zend_class_entry *ndata_ce_NArray;

ZEND_BEGIN_ARG_INFO_EX(arginfo___construct, 2, ZEND_RETURN_VALUE, 0)
    ZEND_ARG_INFO(0, size)
    ZEND_ARG_INFO(0, type)
    ZEND_ARG_ARRAY_INFO(0, data, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_count, 0, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

static zend_function_entry narray_methods[] = {
    PHP_ME(NArray, __construct, arginfo___construct, ZEND_ACC_PUBLIC)
    PHP_ME(NArray, count, arginfo_count, ZEND_ACC_PUBLIC)
    { NULL, NULL, NULL }
};

void ndata_init_NArray(TSRMLS_D)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY_NS(ce, "NArray", "NData", narray_methods);
    ce.create_object = php_ndata_narray_new;
    ndata_ce_NArray = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

}

PHP_METHOD(NArray, __construct)
{
    php_ndata_narray_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

PHP_METHOD(NArray, count)
{
    ndata_array *data = PHP_NDATA_GET_DATA(getThis());

    RETURN_LONG(data->count);
}

static inline void* ndata_narray_allocate(ndata_type_t type, long size)
{
    switch (type) {
        case N_TYPE_LONG:
            return (void *) safe_emalloc(sizeof(long), size, 0);
        case N_TYPE_DOUBLE:
            return (void *) safe_emalloc(sizeof(double), size, 0);
        case N_TYPE_BOOL:
            return (void *) safe_emalloc(sizeof(char), ceil((double) size / 8.0), 0);
    }
}

void php_ndata_narray_ctor(INTERNAL_FUNCTION_PARAMETERS, int bc)
{
    ndata_array *link;
    long type = 0, size = 0;
    zval *options = 0;
    ndata_type_t parsed_type;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, 'll|a', &type, &size, &data) == FAILURE) {
        zval *object = getThis();
        ZVAL_NULL(object);
        return;
    }

    if (type <= N_TYPE_MIN || type >= N_TYPE_MAX) {
        zend_throw_exception(spl_ce_InvalidArgumentException, "Invalid type specified", 0 TSRMLS_CC);
        return;
    }

    parsed_type = (ndata_type_t) type;

    link = (ndata_array*) zend_object_store_get_object(getThis() TSRMLS_CC);
    link->size = size;
    link->type = parsed_type;
    link->count = 0;
    
    if (size > 0) {
        link->data = ndata_narray_allocate(ndata_narray_getTypeSize(parsed_type), size);
        link->current_size = size;
    } else {
        link->data = ndata_narray_allocate(ndata_narray_getTypeSize(parsed_type), 1);
        link->current_size = 1;
    }

}

static void php_ndata_narray_free(void *object TSRMLS_DC)
{
    ndata_array *link = (ndata_array*)object;
    if (!link) {
        return;
    }
    zend_object_std_dtor(&link->std TSRMLS_CC);
    if (link->data) {
        efree(link->data);
    }
    efree(link);
}

zend_object_value php_ndata_narray_new(zend_class_entry *class_type TSRMLS_DC)
{
    zend_object_value retval;
    ndata_array *intern;

    intern = (ndata_array*)emalloc(sizeof(ndata_array));
    memset(intern, 0, sizeof(ndata_array));
    zend_object_std_init(&intern->std, class_type TSRMLS_CC);
    init_properties(intern);

    retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_ndata_narray_free, NULL TSRMLS_CC);
    retval.handlers = &ndata_narray_handlers;

    return retval;
}
