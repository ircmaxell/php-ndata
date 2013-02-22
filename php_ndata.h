#ifndef PHP_NDATA_H

#define PHP_NDATA_H 1

#define PHP_NDATA_VERSION "1.0"
#define PHP_NDATA_EXTNAME "ndata"

extern zend_module_entry ndata_module_entry;
#define phpext_ndata_ptr &ndata_module_entry

PHP_MINIT_FUNCTION(ndata);
PHP_MSHUTDOWN_FUNCTION(ndata);
PHP_MINFO_FUNCTION(ndata);

typedef enum {
    N_TYPE_MIN = 0;
    N_TYPE_LONG,
    N_TYPE_DOUBLE,
    N_TYPE_BOOL,
    N_TYPE_MAX
} ndata_type_t;

#endif
