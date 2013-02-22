#include <php.h>
#include "php_ndata.h"
#include "narray.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_iterators.h"

extern zend_object_handlers narray_handlers;

zend_class_entry *ndata_ce_NArray;

zend_object_handlers ndata_narray_handlers;

zend_object_value php_ndata_narray_new(zend_class_entry *class_type TSRMLS_DC);

static void php_ndata_narray_free(void *object TSRMLS_DC);
void php_ndata_narray_ctor(INTERNAL_FUNCTION_PARAMETERS);
static zval* narray_read_dimension(zval *object, zval *offset, int type TSRMLS_DC);
static void narray_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC);

static inline int narray_write_zval_to_offset(zval *value, ndata_array *link, long offset);
static inline int narray_write_offset_to_zval(zval **result, ndata_array *link, long offset, int alloc_init);

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
    INIT_NS_CLASS_ENTRY(ce, "NData", "NArray", narray_methods);
    ce.create_object = php_ndata_narray_new;
    ndata_ce_NArray = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

    zend_class_implements(ndata_ce_NArray TSRMLS_CC, 1, spl_ce_Countable);

    memcpy(&ndata_narray_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    
    ndata_narray_handlers.read_dimension = narray_read_dimension;
    ndata_narray_handlers.write_dimension = narray_write_dimension;

    zend_declare_class_constant_long(ndata_ce_NArray, "TYPE_LONG", sizeof("TYPE_LONG") - 1, (long) N_TYPE_LONG);
    zend_declare_class_constant_long(ndata_ce_NArray, "TYPE_DOUBLE", sizeof("TYPE_DOUBLE") - 1, (long) N_TYPE_DOUBLE);
    zend_declare_class_constant_long(ndata_ce_NArray, "TYPE_BOOL", sizeof("TYPE_BOOL") - 1, (long) N_TYPE_BOOL);
   
}

PHP_METHOD(NArray, __construct)
{
    php_ndata_narray_ctor(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

PHP_METHOD(NArray, count)
{
    ndata_array *data = (ndata_array*)zend_object_store_get_object(getThis() TSRMLS_CC);

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

static inline void* ndata_narray_reallocate(void *data, ndata_type_t type, long size)
{
    switch (type) {
        case N_TYPE_LONG:
            return (void *) safe_erealloc(data, sizeof(long), size, 0);
        case N_TYPE_DOUBLE:
            return (void *) safe_erealloc(data, sizeof(double), size, 0);
        case N_TYPE_BOOL:
            return (void *) safe_erealloc(data, sizeof(char), ceil((double) size / 8.0), 0);
    }
}

void php_ndata_narray_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
    ndata_array *link;
    long type = 0, size = 0;
    zval *data = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll|a", &type, &size, &data) == FAILURE) {
        zval *object = getThis();
        ZVAL_NULL(object);
        return;
    }

    if (type <= N_TYPE_MIN || type >= N_TYPE_MAX) {
        zend_throw_exception(spl_ce_InvalidArgumentException, "Invalid type specified", 0 TSRMLS_CC);
        return;
    }


    link = (ndata_array*)zend_object_store_get_object(getThis() TSRMLS_CC);
    link->size = size;
    link->type = (ndata_type_t) type;
    link->count = 0;
    
    if (size > 0) {
        link->data = ndata_narray_allocate(link->type, size);
        link->current_size = size;
    } else {
        link->data = ndata_narray_allocate(link->type, 1);
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

    retval.handle = zend_objects_store_put(intern, (zend_objects_store_dtor_t) zend_objects_destroy_object, php_ndata_narray_free, NULL TSRMLS_CC);
    retval.handlers = &ndata_narray_handlers;

    return retval;
}

static zval* narray_read_dimension(zval *object, zval *offset, int type TSRMLS_DC)
{
    long real_offset = 0;
    zval *result;
    ndata_array *link = zend_object_store_get_object(object TSRMLS_CC);
    switch (type) {
        case BP_VAR_W:
        case BP_VAR_RW:
        case BP_VAR_UNSET:
            zend_throw_exception(spl_ce_LogicException, "Using array in write context", 0 TSRMLS_CC);
            return NULL;
        default:
            break;
    }
    if (Z_TYPE_P(offset) == IS_LONG) {
        real_offset = Z_LVAL_P(offset);
    } else {
        zend_throw_exception(spl_ce_OutOfBoundsException, "Key must be an integer", 0 TSRMLS_CC);
        return NULL;
    }
    
    if (FAILURE == narray_write_offset_to_zval(&result, link, real_offset, 1)) {
        zend_throw_exception(spl_ce_OutOfRangeException, "Offset Out Of Range", 0 TSRMLS_CC);
        return NULL;
    }
    return result;
}

static void narray_write_dimension(zval *object, zval *offset, zval *value TSRMLS_DC)
{
    long real_offset = 0;
    ndata_array *link = zend_object_store_get_object(object TSRMLS_CC);
   
    if (!offset) { 
        real_offset = link->count;
    } else if (Z_TYPE_P(offset) == IS_LONG) {
        real_offset = Z_LVAL_P(offset);
    } else {
	zend_throw_exception(spl_ce_InvalidArgumentException, "Invalid Offset Provided", 0 TSRMLS_CC);
        return;
    }

    if (FAILURE == narray_write_zval_to_offset(value, link, real_offset TSRMLS_CC)) {
        // Handle error!!!
    }
}

static inline int narray_write_zval_to_offset(zval *value, ndata_array *link, long offset TSRMLS_DC)
{
    if (offset < 0 || offset > link->count) {
	zend_throw_exception(spl_ce_OutOfBoundsException, "Invalid Offset Specified", 0 TSRMLS_CC);
        return FAILURE;
    }
    if (offset >= link->current_size) {
        if (link->size == 0) {
            link->data = ndata_narray_reallocate(link->data, link->type, 2 * link->current_size);
            link->current_size = 2 * link->current_size;
        } else if (link->size < 0) {
            long size_change = abs(link->size);
            link->data = ndata_narray_reallocate(link->data, link->type, size_change + link->current_size);
            link->current_size += size_change;
        } else {
            zend_throw_exception(spl_ce_RuntimeException, "Non-Resizable Array", 0 TSRMLS_CC);
            return FAILURE;
        }
    }

    switch (link->type) {
        case N_TYPE_LONG: {
            long *data = (long*)(link->data);
            if (Z_TYPE_P(value) != IS_LONG) {
                return FAILURE;
            }
            data[offset] = Z_LVAL_P(value);
            }
            break;
        case N_TYPE_DOUBLE: {
            double *data = (double*)(link->data);
            if (Z_TYPE_P(value) != IS_DOUBLE) {
                return FAILURE;
            }
            data[offset] = Z_DVAL_P(value);
            }
            break;
        case N_TYPE_BOOL: {
            char *data = (char*)(link->data);
            long start_offset = floor((double) offset / 8.0);
            long minor_offset = offset % 8;
            if (Z_BVAL_P(value) == 1) {
                data[start_offset] |= (1 << (8 - minor_offset));
            } else {
                data[start_offset] &= ~(1 << (8 - minor_offset));
            }
            }
            break;
    }
    if (offset == link->count) {
        link->count++;
    }
    return SUCCESS;
}

static inline int narray_write_offset_to_zval(zval **result, ndata_array *link, long offset, int alloc_init)
{
    if (offset < 0 || offset >= link->count) {
        return FAILURE;
    }
    if (alloc_init) {
        MAKE_STD_ZVAL(*result);
    }
    switch (link->type) {
        case N_TYPE_LONG: {
            long *data = (long*)(link->data);
            ZVAL_LONG(*result, data[offset]);
            }
            break;
        case N_TYPE_DOUBLE: {
            double *data = (double*)(link->data);
            ZVAL_DOUBLE(*result, data[offset]);
            }
            break;
        case N_TYPE_BOOL: {
            char *data = (char*)(link->data);
            long start_offset = floor((double) offset / 8.0);
            long minor_offset = offset % 8;
            char bits = data[start_offset];
            ZVAL_BOOL(*result, (bits >> (8 - minor_offset)) & 1);
            }
            break;
    }
    return SUCCESS;
}
