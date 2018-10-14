/*
 * Config section
 */

/*
 * prints every malloc, calloc or free
 */
//#define _SMC_CONF_DEBUG

/*
 * Prevent invalid free()
 */
#define _SMC_CONF_OMIT_INVALID_FREE

/*
 * Prevent double free() corruption
 */
#define _SMC_CONF_OMIT_DOUBLE_FREE

/*
 * Prints a memory leak report
 */
#define _SMC_CONF_SUMMARY
