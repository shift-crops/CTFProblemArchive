// gcc -Wl,-z,relro,-z,now -fPIE -pie BBQ.c -o BBQ && strip BBQ

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE	0x40
#define LIMIT_GRILL 8

#define MAGIC_COOKING	0xdeadbeef11
#define MAGIC_CHARCOAL	0x0badf00d22
#define MAGIC_EATEN		0xcafebabe33

struct stock {
	struct stock *next;
	unsigned amount;
	char *name;
} *stock_top = NULL;

struct cook {
	struct stock *food;
	unsigned progress;
	long magic;
} *cooking[6];

void initialize(void);
int menu(void);

void buy(void);
void grill(void);
void eat(void);

void add_stock(char *name, int n);
void list_stock(void);
void list_cooking(void);
void progress_grill(void);

void getnline(char *buf, int size);
int getint(void);

int main(void){
	unsigned long select;

	initialize();

	puts("Today is BBQ Party!");

	while(1){
		progress_grill();
		select = menu();
		switch(select % 0x10){
			case 1:
				buy();
				break;
			case 2:
				grill();
				break;
			case 3:
				eat();
				break;
			case 0:
				goto end;
			default:
				puts("Wrong input.");
		}
	}

end:
	puts("Bye!");
	return 0;
}

void initialize(void){
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	add_stock("Beef", 1);
}

int menu(void){
	printf(	"\n"
		"1. Buy\n"
		"2. Grill\n"
		"3. Eat\n"
		"0. Break up\n"
		"Choice: ");

	return getint();
}

void buy(void){
	int n;
	char buf[BUF_SIZE];

	list_stock();
	printf("food name >> ");
	getnline(buf, sizeof(buf));
	printf("amount >> ");
	n = getint();

	add_stock(buf, n);
}

void grill(void){
	struct stock *s;
	struct cook *c;
	int idx;
	char buf[BUF_SIZE];

	list_stock();
	list_cooking();

	printf("which food >> ");
	getnline(buf, sizeof(buf));

	for(s = stock_top; s; s = s->next) {
		if(s->name && !strcmp(buf, s->name))
			break;
	}

	if(!s || s->amount<1){
		puts("Not found...");
		return;
	}

	printf("griddle index >> ");
	idx = getint();
	if(idx < 0 || idx >= sizeof(cooking)/sizeof(struct cook*)){
		puts("There is no griddle!");
		return;
	}
	if(cooking[idx]){
		puts("now cooking...");
		return;
	}

	c = (struct cook*)malloc(sizeof(struct cook));
	c->food = s;
	s->amount--;
	c->magic = MAGIC_COOKING;
	c->progress = 0;
	cooking[idx] = c;
}

void eat(void){
	int idx;
	struct cook *c;
	char _[0x10];

	list_cooking();
	printf("griddle index >> ");
	idx = getint();
	if(idx < 0 || idx >= sizeof(cooking)/sizeof(struct cook*)){
		puts("There is no griddle!");
		return;
	}

	if(cooking[idx]){
		c = cooking[idx];
		puts("found food.");
	}
	else 
		puts("empty...");

	if(!c)	return;

	switch(c->magic){
		case MAGIC_COOKING:
			c->magic = MAGIC_EATEN;
			free(c);
			puts("Yummy!");
			cooking[idx] = NULL;
			break;
		case MAGIC_CHARCOAL:
			puts("I don't want to eat charcoal...");
			break;
	}
}

void add_stock(char *name, int n){
	struct stock *p;

	if(n < 1){
		puts("b.u..y...???");
		return;
	}

	for(p = stock_top; p; p = p->next) {
		if(p->name && !strcmp(name, p->name))
			break;
	}

	if(!p){
		p = (struct stock*)malloc(sizeof(struct stock));
		if(!p)
			exit(1);

		p->name = strdup(name);
		p->amount = 0;
		p->next = stock_top;
		stock_top = p;
	}

	p->amount += n;
}

void list_stock(void){
	struct stock *p;

	puts("\nFood Stock List");
	for(p = stock_top; p; p = p->next)
		printf("* %s (%u)\n", p->name, p->amount);
}

void list_cooking(void){
	int i;

	puts("\nCooking List");
	for(i = 0; i < sizeof(cooking)/sizeof(struct cook*); i++)
		printf("[%02d] %s\n", i, cooking[i] ? cooking[i]->magic == MAGIC_COOKING  ?  cooking[i]->food->name : "<CHARCOAL>" : "<FREE>");
}

void progress_grill(void){
	int i;

	for(i = 0; i < sizeof(cooking)/sizeof(struct cook*); i++){
		struct cook *c = cooking[i];
		if(c && ++c->progress == LIMIT_GRILL)
			c->magic = MAGIC_CHARCOAL;
	}
}

void getnline(char *buf, int size){
	char *lf;

	if(size < 0)
		return;

	fgets(buf, size, stdin);
	if((lf=strchr(buf,'\n')))
		*lf='\0';
}

int getint(void){
	char buf[BUF_SIZE];

	buf[0] = 0x00;
	getnline(buf, sizeof(buf));
	return atoi(buf);
}
