#include <sys/mman.h>

#define HEAP_SIZE 0x1000
#define PREV_INUSE(p)	(p->size&1)
#define PREV_SIZE(p)	(*(long*)((void*)p-sizeof(long)))

void *mmaped_buf;

typedef struct HEADER{
	long size;
	struct HEADER *fd, *bk;
} Heap;

Heap h_top = {
	.size = 0,
	.fd = NULL,
	.bk = NULL,
};

init_heap(){
	Heap *ptr;

	mmaped_buf = mmap(NULL, HEAP_SIZE, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	ptr  = (Heap*)mmaped_buf;

	ptr->size = HEAP_SIZE|1;
	ptr->fd = ptr->bk = &h_top;
	h_top.fd = h_top.bk = ptr;
}

fini_heap(){
	munmap(mmaped_buf, HEAP_SIZE);
}

void link_freelist(Heap*);
void unlink_freelist(Heap*);

void *malloc(size_t size){
	Heap *ptr = h_top.fd, *post;
	size_t next_size;

	size += sizeof(long);
	if(size%8)
		size = (size/8+1)*8;
	if(size<0x20)
		size = 0x20;

	while(ptr!=&h_top && ptr->size<size)
		ptr = ptr->fd;
	if(ptr==&h_top)
		return NULL;

	unlink_freelist(ptr);

	if((int)(next_size = (ptr->size&~1)-size) > sizeof(long)){
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

	if(!PREV_INUSE(post))
		return;

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

	chunk->bk->fd = chunk->fd;
	chunk->fd->bk = chunk->bk;
}
