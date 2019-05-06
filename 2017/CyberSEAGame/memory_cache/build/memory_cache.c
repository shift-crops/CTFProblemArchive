// gcc memory_cache.c -lcrypto -o memory_cache
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <openssl/sha.h>

#define BUF_SIZE 0x100
#define malloc_usable_size(p)	(*(size_t*)((void*)p-0x8) - 0x8) 	// vuln

enum COMMAND { COMM_EXIT, COMM_SET, COMM_GET, COMM_INVALID };

struct Entry {
	char hash[SHA_DIGEST_LENGTH];
	size_t size;
	char *data;
	struct Entry *l, *r;
} *top;

void sha1sum(char digest[], char *buf, size_t len);
enum COMMAND get_command(char **key);
void set(char *hash);
void get(char *hash);
struct Entry *find_entry(char *hash);

int main(void){
	enum COMMAND cmd;
	char *key;

	dprintf(STDOUT_FILENO, "Starting MemoryCache...\n");
	while(cmd = get_command(&key)){
		char hash[SHA_DIGEST_LENGTH];

		if(!key){
			dprintf(STDOUT_FILENO, "Syntax Error\n");
			continue;
		}

		sha1sum(hash, key, strlen(key));
		free(key);

		switch(cmd){
			case COMM_SET:
				set(hash);
				break;
			case COMM_GET:
				get(hash);
				break;
			default:
				dprintf(STDOUT_FILENO, "Invalid Command\n");
		}
	}
}

void sha1sum(char digest[], char *buf, size_t len){
	SHA_CTX c;

	SHA1_Init(&c);
	SHA1_Update(&c, buf, len);
	SHA1_Final(digest, &c);
}

enum COMMAND get_command(char **key){
	char buf[BUF_SIZE]={0}, *p;
	enum COMMAND cmd = COMM_INVALID;

	*key = NULL;
	if(read(STDIN_FILENO, buf, sizeof(buf)-1) <= 0)
		return COMM_EXIT;

	if(!strncasecmp("EXIT", buf, 4))
		return COMM_EXIT;
	if(!(p = strchr(buf, ' ')))
		return COMM_INVALID;

	if(!strncasecmp("SET", buf, p-buf))
		cmd = COMM_SET;
	else if(!strncasecmp("GET", buf, p-buf))
		cmd = COMM_GET;

	*key = strdup(p+1);
	return cmd;
}

void set(char *hash){
	struct Entry *entry;
	char buf[BUF_SIZE]={0};
	size_t size;

	size = read(STDIN_FILENO, buf, sizeof(buf)-1)>0 ? atoi(buf) : 0;
	if((int)size < 0 || size > 0x400){
		dprintf(STDOUT_FILENO, "NG %zd\n", size);
		return;
	}

	if(entry = find_entry(hash)){
		size_t usable;

		assert((usable = malloc_usable_size(entry->data)) < 0x400);

		if(usable < size){
			void *p;

			if(!(p = realloc(entry->data, size))){
				dprintf(STDOUT_FILENO, "NG %zd\n", size);
				return;
			}
			entry->data = p;
		}
	}
	else{
		entry = realloc(NULL, sizeof(struct Entry));
		if(!entry)
			return;

		memcpy(entry->hash, hash, sizeof(entry->hash));
		entry->data = realloc(NULL, size);
		if(!entry->data){
			free(entry);
			dprintf(STDOUT_FILENO, "NG %zd\n", size);
			return;
		}

		entry->l = entry->r = NULL;

		if(!top)
			top = entry;
		else{
			struct Entry *e = top;

			for(;;){
				int cmp;

				cmp = memcmp(hash, e->hash, sizeof(e->hash));
				assert(cmp != 0);
				if(cmp < 0){
					if(!e->l){
						e->l = entry;
						break;
					}
					e = e->l;
				}
				else if(cmp > 0){
					if(!e->r){
						e->r = entry;
						break;
					}
					e = e->r;
				}
			}
		}
	}

	size = read(STDIN_FILENO, entry->data, size);
	entry->size = size;

	write(STDOUT_FILENO, "OK\n", 3);
}

void get(char *hash){
	struct Entry *entry;

	if(!(entry = find_entry(hash))){
		write(STDOUT_FILENO, "0, \n", 4);
		return;
	}

	dprintf(STDOUT_FILENO, "%zd, ", entry->size);
	write(STDOUT_FILENO, entry->data, entry->size);
	write(STDOUT_FILENO, "\n", 1);
}

struct Entry *find_entry(char *hash){
	struct Entry *e;

	for(e = top; e; ){
		int cmp;

		cmp = memcmp(hash, e->hash, sizeof(e->hash));
		if(cmp < 0)
			e = e->l;
		else if(cmp > 0)
			e = e->r;
		else
			return e;
	}

	return NULL;
}
