/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/bmem.c,v 1.3 1999/11/26 14:30:27 bnv Exp $
 * $Log: bmem.c,v $
 * Revision 1.3  1999/11/26 14:30:27  bnv
 * Added: A dummy int, at Memory structure for 8-byte alignment on 32-bit machines
 *
 * Revision 1.2  1999/11/26 13:13:47  bnv
 * Changed: Added MAGIC number also to the end.
 * Chanted: Modified to work with 64-bit computers.
 *
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
	dword	magic;
	char	*desc;
	size_t	size;
	struct	tmemory_st *next;
	struct	tmemory_st *prev;
#if defined(ALIGN) && !defined(__ALPHA)
	/* Some machines have problems ithe address is not at 8-bytes aligned */
	int	dummy;
#endif
	byte	data[sizeof(void*)];
} Memory;

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
	mem = (Memory *)farmalloc(sizeof(Memory)+size);
#else
	mem = (Memory *)malloc(sizeof(Memory)+size);
#endif
	if (mem) {
		/* Create the memory header */
		mem->magic = MAGIC;
		mem->desc = desc;
		mem->size = size;
		mem->next = NULL;
		mem->prev = mem_head;
		if (mem_head)
			mem_head->next = mem;

		mem_head = mem;
		total_mem += size;

		/* Mark also the END of data */
		*(dword *)((byte*)mem->data+mem->size) = MAGIC;

		return (void *)(mem->data);
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
	mem = (Memory *)((char *)ptr - (sizeof(Memory)-sizeof(void*)));

	/* check if the memory is valid */
	if (mem->magic != MAGIC) {
		fprintf(STDERR,"mem_realloc: object is not allocated size=%d!\n",size);
		mem_list();
		exit(99);
	}

	if (*(dword *)(mem->data+mem->size) != MAGIC) {
		fprintf(STDERR,"mem_realloc: End magic number doesn't match!\n");
		mem_list();
		exit(99);
	}

	total_mem -= mem->size;
	head = (mem==mem_head);

#if defined(__BORLANDC__)&&(defined(__HUGE__)||defined(__LARGE__))
	mem = (Memory *)farrealloc(mem,size+sizeof(Memory));
#else
	mem = (Memory *)realloc(mem,size+sizeof(Memory));
#endif

	if (head) mem_head = mem;
	mem->size = size;
	total_mem += size;

	other = mem->prev;
	if (other)	other->next = mem;
	other = mem->next;
	if (other)	other->prev = mem;

	/* Mark also the new END of data */
	*(dword *)(mem->data+mem->size) = MAGIC;

	return (void *)(mem->data);
} /* mem_realloc */

/* -------------- mem_free ---------------- */
void
mem_free(void *ptr)
{
	Memory	*mem, *mem_prev, *mem_next;
	int	head;

	/* find our header */
	mem = (Memory *)((char *)ptr - (sizeof(Memory)-sizeof(void*)));

	if (mem->magic != MAGIC) {
		fprintf(STDERR,"mem_free: object is not allocated!\n");
		mem_list();
		exit(99);
	}
	if (*(dword *)(mem->data+mem->size) != MAGIC) {
		fprintf(STDERR,"mem_realloc: End magic number doesn't match!\n");
		mem_list();
		exit(99);
	}

	/* Remove the MAGIC number, just to catch invalid entries */
	mem->magic = 0L;

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
	fprintf(STDERR,"\nMemory list:\n");
	y = 0;
	while (mem) {
		fputs((mem->magic==MAGIC)?"  ":"??",STDERR);

		fprintf(STDERR,"%3d %6d  %s\t\"",
			++count, mem->size, mem->desc);
		for (i=0; i<10; i++)
			fprintf(STDERR,"%c",mem->data[i]);
		fprintf(STDERR,"\"  ");
		for (i=0; i<10; i++)
			fprintf(STDERR,"%02X ",mem->data[i]);
		fprintf(STDERR,"\n");
		mem = mem->prev;
		if (++y==15) {
			if (getchar()=='q') exit(0);
			y = 0;
		}
	}
	fprintf(STDERR,"\n");
} /* mem_list */

/* --------------- memchk_list ------------------- */
int
memchk_list( void )
{
	Memory	*mem;
	int	i=0;

	for (mem=mem_head; mem; mem = mem->prev,i++) {
		if (mem->magic != MAGIC) {
			fprintf(STDERR,"Magic number destroyed! ID=%d\n",i);
			mem_list();
		}
		if (*(dword *)(mem->data+mem->size) != MAGIC) {
			fprintf(STDERR,"mem_realloc: End magic number doesn't match! ID=%d\n",i);
			mem_list();
		}
	}
	return 0;
} /* memchk_list */

/* -------------- mem_allocated ----------------- */
long
mem_allocated( void )
{
	return total_mem;
} /* mem_allocated */
