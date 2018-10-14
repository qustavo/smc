#ifndef _SMC_H
#define _SMC_H

#define _SMC_VERSION_	0.1

#include "config.h"

#include <string.h>
#include <stdlib.h>

#ifndef __builtin_expect
#define __builtin_expect(x, expected_value) (x)
#endif
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define TXT_LEN	64

#define FREE_STATE_OK	0
#define FREE_STATE_INVALID	1
#define FREE_STATE_DOUBLE	2


struct mem_unit {
		void *mem;
		size_t size;
		char file[TXT_LEN];
		char func[TXT_LEN];
		int line;
		int inuse;
		struct mem_unit *prev, *next;
};

struct mem_unit *mem_head = NULL;
struct mem_unit *mem_tail = NULL;

static inline void *
mem_alloc(size_t size, const char *file, const char *func, int line) {
		struct mem_unit *mu = (struct mem_unit *)malloc(sizeof(struct mem_unit));

		if (mem_head == NULL) {
				mem_head = mem_tail = mu;
				mem_head->next = mem_tail;
				mem_tail->prev = mem_head;
				mem_head->prev = NULL;
		}
		else {
				mem_tail->next = mu;
				mu->prev = mem_tail;
				mem_tail = mu;
		}

		mu->mem = malloc(size);
		mu->size = size;
		strncpy(mu->file, file, strlen(file));
		strncpy(mu->func, func, strlen(func));
		mu->line = line;
		mu->inuse = 1;
		mu->next = NULL;

#if defined (_SMC_CONF_DEBUG)
		printf("[@ 0x%x] malloc(): %10d Byte(s) > %s:%s():%d\n",
						(int)mu->mem, mu->size, mu->file, mu->func, mu->line);
#endif

		return mu->mem;
}


static inline void *
mem_calloc(size_t size, const char *file, const char *func, int line) {
		struct mem_unit *mu = (struct mem_unit *)malloc(sizeof(struct mem_unit));

		if (mem_head == NULL) {
				mem_head = mem_tail = mu;
				mem_head->next = mem_tail;
				mem_tail->prev = mem_head;
				mem_head->prev = NULL;
		}
		else {
				mem_tail->next = mu;
				mu->prev = mem_tail;
				mem_tail = mu;
		}

		mu->mem = malloc(size);
		memset(mu->mem, 0, sizeof(size));
		mu->size = size;
		strncpy(mu->file, file, strlen(file));
		strncpy(mu->func, func, strlen(func));
		mu->line = line;
		mu->inuse = 1;
		mu->next = NULL;

#if defined (_SMC_CONF_DEBUG)
		printf("[@ 0x%x] calloc(): %10d Byte(s) > %s:%s():%d\n",
						(int)mu->mem, mu->size, mu->file, mu->func, mu->line);
#endif

		return mu->mem;
}


static inline void
mem_free(void *ptr, const char *file, const char *func, int line) {
		struct mem_unit *mu = mem_head;
		int state = FREE_STATE_OK;

		if (unlikely(mu == NULL)) {
				state = FREE_STATE_INVALID;
				goto error;
		}

		while ( (mu->mem != ptr) && (mu->next != NULL) ) {
				mu = mu->next;
		}

		/* if not found */
		if(unlikely(mu->mem != ptr)) {
				state = FREE_STATE_INVALID;
				goto error;
		}
		else {
				if (mu->inuse == 0) {
						state = FREE_STATE_DOUBLE;
						goto error;
				}

				free(ptr);
				mu->inuse = 0;
		}
#if defined (_SMC_CONF_DEBUG)
		printf("[@ 0x%x] free()  : %10d Byte(s) > %s:%s():%d\n",
						(int)mu->mem, mu->size, file, func, line);
#endif
		return;

error:
		printf("[@ 0x%p] free()  : %10ld Byte(s) > %s:%s():%d - ",
						mu->mem, mu->size, file, func, line);
		switch(state) {
				case FREE_STATE_INVALID:
						printf("Invalid free() detected");
#if !defined (_SMC_CONF_OMIT_INVALID_FREE)
						free(ptr);
#else
						printf(" [omited] ");
#endif
						printf("\n");
						break;
				case FREE_STATE_DOUBLE:
						printf("Double  free() detected");
#if !defined (_SMC_CONF_OMIT_DOUBLE_FREE)
						free(ptr);
#else
						printf(" [omited] ");
#endif
						printf("\n");
		}
}

#if defined (_SMC_CONF_SUMMARY)
__attribute__ ((destructor)) static void
mem_result(void) {
		struct mem_unit *mu;
		int c = 0;
		int mem = 0;

		mu = mem_head;

		if (likely(mu != NULL)) {
				printf("\nleak summary:");
				printf("\n~~~~ ~~~~~~~\n");
				while(mu != NULL) {
						if (mu->inuse != 0) {
								c++;
								mem = mem + mu->size;
								printf("%10ld Byte%s @ 0x%p > %s:%s():%d\n",
												mu->size, mu->size > 1 ? "s" : " ",
												mu->mem,
												mu->file, mu->func, mu->line);
						}
						mu = mu->next;
				}
				free(mu);
				printf("%d bytes allocated in %d calls\n", mem, c);
		}
}
#endif

#define malloc(x) mem_alloc(x, __FILE__, __func__, __LINE__)
#define calloc(x, y) mem_calloc(x * y, __FILE__, __func__, __LINE__)
#define free(x) mem_free(x, __FILE__, __func__, __LINE__)

#endif	/*	_SMC_H	*/

