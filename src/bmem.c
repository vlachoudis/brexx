/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/bmem.c,v 1.1 1998/07/02 17:34:50 bnv Exp $
 * $Log: bmem.c,v $
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#define __BMEM_C__

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#include <bmem.h>
#include <lstring.h>

#define MAGIC	0x12346789L

/*
 * This file provides some debugging functions for memory allocation
 */

typedef struct tmemory_st {
	long	magic;
	char	*desc;
	int	size;
	struct	tmemory_st *next;
	struct	tmemory_st *prev;
#if defined(ALIGN)
	/* add 4 bytes to make them to 8 byte alignment */
	int	dummy;
#endif
	byte	data[4];
} Memory;

#define MEMSIZE	(sizeof(Memory)-4)

static Memory	*mem_head = NULL;
static long	total_mem = 0L;

void mem_list(void);

/* -------------- mem_malloc ---------------- */
void *
mem_malloc(size_t size, char *desc)
{
	Memory	*mem;

	/* add space for the header */
#if defined(__BORLANDC__)&&(defined(__HUGE__)||defined(__LARGE__))
	mem = (Memory *)farmalloc(MEMSIZE+size);
#else
	mem = (Memory *)malloc(MEMSIZE+size);
#endif
	if (mem) {
		mem->magic = MAGIC;
		mem->desc = desc;
		mem->size = size;
		mem->next = NULL;
		mem->prev = mem_head;
		if (mem_head)
			mem_head->next = mem;

		mem_head = mem;
		total_mem += size;

		return &(mem->data);
	} else
		return NULL;
} /* mem_malloc */

/* -------------- mem_realloc ---------------- */
void *
mem_realloc(void *ptr, size_t size)
{
	Memory	*mem, *other;
	int	head;

	/* find our header */
	mem = (Memory *)((char *)ptr - MEMSIZE);

	if (mem->magic != MAGIC) {
		fprintf(stderr,"mem_realloc: object is not allocated size=%d!\n",size);
		mem_list();
		exit(99);
	}

	total_mem -= mem->size;
	head = (mem==mem_head);

#if defined(__BORLANDC__)&&(defined(__HUGE__)||defined(__LARGE__))
	mem = (Memory *)farrealloc(mem,size+MEMSIZE);
#else
	mem = (Memory *)realloc(mem,size+MEMSIZE);
#endif

	if (head) mem_head = mem;
	mem->size = size;
	total_mem += size;

	other = mem->prev;
	if (other)	other->next = mem;
	other = mem->next;
	if (other)	other->prev = mem;

	
	return &(mem->data);
} /* mem_realloc */

/* -------------- mem_free ---------------- */
void
mem_free(void *ptr)
{
	Memory	*mem, *mem_prev, *mem_next;
	int	head;

	/* find our header */
	mem = (Memory *)((char *)ptr - MEMSIZE);

	if (mem->magic != MAGIC) {
		fprintf(stderr,"mem_free: object is not allocated!\n");
		mem_list();
		exit(99);
	}

	mem_prev = mem->prev;
	mem_next = mem->next;
	total_mem -= mem->size;
	head = (mem==mem_head);

#if defined(__BORLANDC__)&&(defined(__HUGE__)||defined(__LARGE__))
	farfree(mem);
#else
	free(mem);
#endif

	if (mem_next) mem_next->prev = mem_prev;
	if (mem_prev) mem_prev->next = mem_next;
	if (head) mem_head = mem_prev;

} /* mem_free */

/* -------------- mem_list ---------------- */
void
mem_list(void)
{
	Memory	*mem;
	int	count,i,y;

	mem = mem_head;
	count = 0;
	fprintf(stderr,"\nMemory list:\n");
	y = 0;
	while (mem) {
		fputs((mem->magic==MAGIC)?"  ":"??",stderr);

		fprintf(stderr,"%3d %6d  %s\t\"",
			++count, mem->size, mem->desc);
		for (i=0; i<10; i++)
			fprintf(stderr,"%c",mem->data[i]);
		fprintf(stderr,"\"  ");
		for (i=0; i<10; i++)
			fprintf(stderr,"%02X ",mem->data[i]);
		fprintf(stderr,"\n");
		mem = mem->prev;
		if (++y==15) {
			if (getchar()=='q') exit(0);
			y = 0;
		}
	}
	fprintf(stderr,"\n");
} /* mem_list */

/* --------------- chk_list ------------------- */
int
chk_list( void )
{
	Memory	*mem;

	for (mem=mem_head; mem; mem = mem->prev)
		if (mem->magic != MAGIC) {
			fprintf(stderr,"Magic number destroyed...\n");
			mem_list();
		}
	return 0;
} /* chk_list */

/* -------------- mem_allocated ----------------- */
long
mem_allocated( void )
{
	return total_mem;
} /* mem_allocated */
