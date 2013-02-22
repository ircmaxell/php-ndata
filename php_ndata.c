#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ndata.h"
#include <ext/standard/info.h>


zend_module_entry ndata_module_entry = {
    STANDARD_MODULE_HEADER,
    "ndata",
    NULL,
    PHP_MINIT(ndata),
    PHP_MSHUTDOWN(ndata),
    NULL,
    NULL,
    PHP_MINFO(ndata),
    PHP_NDATA_VERSION,
    NO_MODULE_GLOBALS,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};

#if COMPILE_DL_NDATA
ZEND_GET_MODULE(ndata)
#endif

PHP_MINIT_FUNCTION(ndata)
{
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(ndata)
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(ndata)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "NData Support", "enabled");
    php_info_print_table_row(2, "Version", PHP_NDATA_VERSION);
    php_info_print_table_end();
}


