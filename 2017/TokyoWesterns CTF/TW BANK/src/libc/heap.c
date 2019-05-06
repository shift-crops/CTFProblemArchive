#include "standard.h"

#define INIT_HEAP_SIZE 	0x1000
#define PREV_INUSE(p)	(p->size&1)
#define PREV_SIZE(p)	(*(long*)((void*)p-sizeof(long)))


void *heap_base = NULL;
unsigned int heap_size = INIT_HEAP_SIZE;
typedef struct HEADER{
	long size;
	struct HEADER *fd, *bk;
} Heap;

Heap h_top = {
	.size = 0,
	.fd = &h_top,
	.bk = &h_top,
};

void link_freelist(Heap*);
void unlink_freelist(Heap*);

void *malloc(unsigned int size){
	Heap *ptr, *post;
	unsigned int next_size;

	ptr = h_top.fd;
	size += sizeof(long);
	if(size%4)
		size = (size/4+1)*4;
	if(size<0x20)
		size = 0x20;

	while(ptr!=&h_top && ptr->size<size)
		ptr = ptr->fd;
	if(ptr==&h_top){
		Heap *tail;
		void *mmap_addr;
		unsigned int mmap_size;

		if(!heap_base){
			int fd;
		       
			fd = open("/dev/urandom", 0, 0);
			read(fd, &heap_base, 4);
			close(fd);

			*((int*)&heap_base) &= ~(0x1000-1);
		}

		if(heap_size < size){
			mmap_size = size;
			if(mmap_size%0x1000)
				mmap_size = (mmap_size/0x1000+1)*0x1000;
		}
		else
			mmap_size = heap_size;

		mmap_addr = mmap(heap_base, mmap_size
			, 0x03 /* PROT_READ|PROT_WRITE */
			, 0x22 /*MAP_PRIVATE|MAP_ANONYMOUS */, -1, 0);
		if((unsigned)mmap_addr % 0x1000)
			return NULL;

		heap_base = mmap_addr;
		heap_size = mmap_size;

		ptr  = (Heap*)heap_base;
		ptr->size = (heap_size-0x10)|1;
		tail = (Heap*)((void*)ptr+(heap_size-0x10));
		tail->size = 0x01;

		link_freelist(ptr);

		heap_base += heap_size;
		heap_size *= 2;

		return malloc(size);
	}

	unlink_freelist(ptr);

	if((int)(next_size = (ptr->size&~1)-size) > 0x20){
		Heap *next;

		next = (Heap*)((void*)ptr+size);
		next->size = next_size;
		PREV_SIZE((Heap*)((void*)next+next_size)) = next_size;

		link_freelist(next);
		ptr->size -= next_size;
	}

	post = (Heap*)((void*)ptr+(ptr->size&~1));
	post->size |= 1;

	return (void*)ptr+sizeof(long);
}

void free(void *p){
	Heap *ptr = (Heap*)(p-sizeof(long));
	Heap *post = (Heap*)((void*)ptr+(ptr->size&~1)), *post2;

	post->size &= ~1;

	if(!PREV_INUSE(ptr)){
		long prev_size = PREV_SIZE(ptr);
		Heap *prev = (Heap*)((void*)ptr-prev_size);

		unlink_freelist(prev);
		prev->size += ptr->size;

		ptr = prev;
	}

	post2 = (Heap*)((void*)post+post->size);
	if(!PREV_INUSE(post2)){
		unlink_freelist(post);
		ptr->size += post->size;

		post = post2;
	}
	PREV_SIZE(post) = ptr->size&~1;
	link_freelist(ptr);
}

void link_freelist(Heap *chunk){
	Heap *ptr=h_top.fd;

	while(ptr!=&h_top && ptr->size<chunk->size)
		ptr = ptr->fd;

	chunk->fd = ptr;
	chunk->bk = ptr->bk;
	ptr->bk->fd = chunk;
	ptr->bk = chunk;
}

void unlink_freelist(Heap *chunk){
	if(!chunk || chunk==&h_top)
		return;

	if(chunk->bk->fd != chunk || chunk->fd->bk != chunk)
		exit(-1);

	chunk->bk->fd = chunk->fd;
	chunk->fd->bk = chunk->bk;
}
