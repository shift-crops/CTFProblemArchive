// gcc diary.c -o diary
#include <stdio.h>
#include <unistd.h>
#include "heap.h"
#include "config.h"
#include "seccomp-bpf.h"
#define BUF_SIZE 32
#define TRUE 1
#define FALSE 0

#define __NR_execveat 322

__attribute__((constructor))
init(){
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	init_heap();
	init_seccomp();
}

init_seccomp(){
	struct sock_filter filter[] = {
		EXAMINE_SYSCALL,
		DENY_SYSCALL(open),	// fork
		DENY_SYSCALL(openat),	// remap_file_pages
		//DENY_SYSCALL(stat),	// write
		DENY_SYSCALL(execve),	// olduname
		DENY_SYSCALL(clone),
		DENY_SYSCALL(fork),	// setpgid
		DENY_SYSCALL(vfork),
		DENY_SYSCALL(creat),	// readlink
		DENY_SYSCALL(execveat),
		DEFAULT_ALLOW,
	};
	struct sock_fprog prog = {
		.len = (unsigned short)(sizeof(filter)/sizeof(filter[0])),
		.filter = filter,
	};

	if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) goto fail;
	if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog)) goto fail;

	return;

fail:	fprintf(stderr, "SECCOMP_FILTER is not available...\n");
	_exit(-1);
}

__attribute__((destructor))
fini(){
	fini_heap();
}

typedef struct DATE{
	int year;
	char month;
	char day;
} Date;

typedef struct DIARY{
	Date date;
	char *content;
	struct DIARY *next, *back;
} Diary;

Diary d_top = {
	.date = {
		.year = 0,
		.month = 0,
		.day = 0,
	},
	.content = NULL,
	.next = &d_top,
	.back = &d_top,
};

Diary *register_entry(void);
void delete_entry(void);
void show_entry(void);

int Insert(Diary*);
int Remove(Diary*);
Diary *Find(Date);
int List(void);

Date input_date(void);
int cmp_date(Date, Date);

int getnline(char*, int);
int getint(void);

int main(void){
	printf(	"Welcome to diary management service\n\n"
		"Menu : ");

	while(TRUE){
		printf("\n1.Register\t2.Show\t\t3.Delete\t0:Exit\n>> ");
		switch(getint()){
			case 1:
				register_entry();
				break;
			case 2:
				show_entry();
				break;
			case 3:
				delete_entry();
				break;
			case 0:
				printf("Bye!\n");
				exit(0);
			default:
				printf("Wrong input ;(\n");
		}
	}
}

Diary *register_entry(void){
	int size;
	Diary *entry;
	Date date;

	printf("\nAdd entry to diary\n");

	date = input_date();
	if(!date.year || !date.month || !date.day){
		printf("Wrong date ;(\n");
		return NULL;
	}

	entry = malloc(sizeof(Diary));
	entry->date = date;

	if(Find(entry->date)){
		printf("Already exsists ;(\n");
		free(entry);
		return NULL;
	}

	printf("Input content size... ");
	if((size=getint())<=0){
		free(entry);
		return NULL;
	}
		
	entry->content = malloc(size);
	printf("\nPlease write what happened on %d/%02d/%02d\n>> ", entry->date.year, entry->date.month, entry->date.day);
	getnline(entry->content, size);

	if(Insert(entry))
		printf("Registration Complete! :)\n");
	else
		printf("Registration Failed ;(\n");
	return entry;
}

void delete_entry(void){
	Diary *entry;
	Date date;

	printf("\nRemove entry from diary\n");
	if(!List()){
		printf("No entry exsists.\n");
		return;
	}

	date = input_date();
	if(!(entry=Find(date))){
		printf(	"Entry not found\n"
			"Deletion Failed ;(\n");
		return;
	}

	if(Remove(entry))
		printf("Deletion Complete! :)\n");
	else
		printf("Deletion Failed ;(\n");
}

void show_entry(void){
	Diary *entry;
	Date date;

	printf("\nShow entry\n");
	if(!List()){
		printf("No entry exsists.\n");
		return;
	}

	date = input_date();
	if(!(entry=Find(date))){
		printf("Entry not found\n");
		return;
	}

	printf("%d/%02d/%02d\n%s\n", date.year, date.month, date.day, entry->content);
}

int Insert(Diary* entry){
	int i;
	Diary *ptr=d_top.next;

	for(i=1; ptr!=&d_top && cmp_date(entry->date, ptr->date)>0; i++)
		ptr = ptr->next;

	entry->next = ptr;
	entry->back = ptr->back;
	ptr->back->next = entry;
	ptr->back = entry;

	return i;
}

int Remove(Diary* entry){
	if(!entry || entry==&d_top)
		return 0;

	entry->back->next = entry->next;
	entry->next->back = entry->back;

	free(entry->content);
	free(entry);
	return 1;
}

Diary *Find(Date date){
	Diary *ptr=d_top.next;

	while(ptr!=&d_top && cmp_date(date, ptr->date)>0)
		ptr = ptr->next;

	if(ptr==&d_top || cmp_date(date, ptr->date))
		return NULL;
	return ptr;
}

int List(void){
	int i;
	Diary *ptr=d_top.next;

	for(i=1; ptr!=&d_top; i++){
		printf("%02d : %d/%02d/%02d\n", i, ptr->date.year, ptr->date.month, ptr->date.day);
		ptr = ptr->next;
	}

	return i-1;
}

Date input_date(void){
	static char first=TRUE;
	int year, month, day;
	Date date;

	if(first){
		first=FALSE;
		printf(	"Input date\n"
			"(range : 1970/01/01 ~ 2016/12/31) ... ");
	}
	else
		printf("Input date ... ");
	scanf("%d/%d/%d", &year, &month, &day);
	getchar();

	if((year<1970 || year>2016) || (month<1 || month>12) || (day<1 || day>31))
		year=month=day=0;

	date.year 	= year;
	date.month 	= (char)month;
	date.day 	= (char)day;
	/*
	for(year=0; year<1970 || year>2016; year=getint())
		printf("Input year (1970-2016)... ");
	date.year = year;
	for(month=0; month<1 || month>12; month=getint())
		printf("Input month (1-12)... ");
	date.month = (char)month;
	for(day=0; day<1 || day>31; day=getint())
		printf("Input day (1-31)... ");
	date.day = (char)day;
	*/

	return date;
}

int cmp_date(Date a, Date b){
	if(a.year<b.year) return -3;
	else if(a.year>b.year) return 3;

	if(a.month<b.month) return -2;
	else if(a.month>b.month) return 2;

	if(a.day<b.day) return -1;
	else if(a.day>b.day) return 1;

	return 0;
}

int getnline(char *buf, int len){
	int i,n;

	n = read(STDIN_FILENO, buf, len+1);
	for(i=0; i<n; i++)
		if(buf[i]=='\n'){
			buf[i]='\0';
			break;
		}
	return i;
}

int getint(void){
	char buf[BUF_SIZE];

	fgets(buf, sizeof(buf), stdin);
	return atoi(buf);
}
