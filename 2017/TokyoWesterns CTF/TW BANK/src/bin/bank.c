#include <standard.h>
#include <utils.h>
#include <heap.h>

#define BUF_SIZE 64

int select(char *name);
void deposit_withdraw(bool add);
void transfer(void);
void show(char *name);
bool register_record(int type, int amount, char *s);
int getnline(char *buf, int len);
int getint(void);

typedef struct {
	int type;
	int amount;
	char *comment;
} record;

int property;
record latest;

int main(void){
	char name[BUF_SIZE];

	printf( "Welcome to TWBANK\n"
		"Input your name : ");

	getnline(name, sizeof(name));
	while(true){
		switch(select(name)){
			case 1:
				deposit_withdraw(true);
				break;
			case 2:
				deposit_withdraw(false);
				break;
			case 3:
				transfer();
				break;
			case 4:
				show(name);
				break;
			case 0:
				goto end;
			default:
				puts("Wrong input.");
		}
	}

end:
	puts("Thank you for using TWBANK!");
	exit(0);
}

int select(char *name){
	printf(	"\n%s's account\n"
		"1. Deposit\n"
		"2. Withdraw\n"
		"3. Transfer\n"
		"4. Show the latest record\n"
		"0. Exit\n"
		"> "
		, name);
	
	return getint();
}

void deposit_withdraw(bool add){
	int amount, size;
	char *s = NULL;

	printf("Input amount : ");
	amount = getint();
	if(!add && amount > property){
		puts("Amount exceeded your deposit balance.");
		return;
	}

	printf("Input comment size : ");

	size = getint() + 1;
	if(!(s = malloc(size))){
		puts("failed...");
		return;
	}

	printf("Input comment : ");
	getnline(s, size);
	if(strchr(s,'%')){
		puts("You cannot include '%' in comments.");
		free(s);
		s = NULL;
	}

	if(register_record(add ? 1 : 2, amount, s)){
		property += add ? amount : -amount;
		puts("success!");
	}
	else
		puts("failed...");
}

void transfer(void){
	int amount;
	char dest[BUF_SIZE], *s;

	printf("Input amount : ");
	amount = getint();
	if(amount > property){
		puts("Amount exceeded your deposit balance.");
		return;
	}

	printf("Destination : ");
	getnline(dest, sizeof(dest));

	s = malloc(BUF_SIZE+0x10);
	if(!s){
		puts("failed...");
		return;
	}
	sprintf(s, "transfer to [%s]", dest);

	if(register_record(3, amount, s)){
		property -= amount;
		puts("success!");
	}
	else
		puts("failed...");
}

void show(char *name){
	switch(latest.type){
		case 1:
			puts("[DEPOSIT]");
			break;
		case 2:
			puts("[WITHDRAW]");
			break;
		case 3:
			puts("[TRANSFER]");
			break;
		default:
			puts("[NONE]");
			return;
	}

	printf(latest.comment, name, property, latest.amount);
	printf("\n");
}

bool register_record(int type, int amount, char *s){
	char *buf;

	latest.type = type;
	latest.amount = amount;

	if(latest.comment)
		free(latest.comment);

	buf = malloc(BUF_SIZE + (s ? strlen(s) : 0));
	buf[0] = '\0';
	strcat(buf, 	"<<%s : %d yen>>\n"
			"|| amount  : %d yen\n");
	if(s){
		strcat(buf, "|| comment : ");
		strcat(buf, s);
		free(s);
	}
	latest.comment = buf;

	if((type < 1 || type > 3) || amount < 0){
		latest.type = 0;
		return false;
	}

	return true;
}

int getnline(char *buf, int size){
	int len;
        char *lf;

	if(size < 0)
		return 0;
	
        len = read(0, buf, size-1);
	buf[len] = '\0';

        if((lf=strchr(buf,'\n')))
                *lf='\0';

	return len;
}

int getint(void){
        char buf[BUF_SIZE];

        getnline(buf, sizeof(buf));
        return atoi(buf);
}

