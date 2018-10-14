#include <stdio.h>
#include <stdlib.h>

#if defined(SMC)
#include "smc.h"
#endif

int
main(void) {
		char *a = malloc(10);
		char *b = malloc(1024);
		char *c = malloc(34);
		char *d = calloc(1,100);
		char *e = malloc(1024);	// Missing free()
		malloc(1);

		free((void *)123);	// Invalid Address
		free(b);
		free(b);	// Double free()
		free(a);
		free(c);
		free(d);
		return 0;
}
