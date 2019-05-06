#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LIST_SIZE 16
#define BUF_SIZE 96
#define true     1

#define while_input(x, cond) do{ printf(">> "); x = getint(); }while(cond);

int menu(void);
void reserve(void);
void confirm(void);
void cancel(void);
int getnline(char *buf, int size);
int getint(void);

typedef enum {TOKYO, SHINAGAWA, SHIN_YOKOHAMA, ODAWARA, ATAMI
		, MISHIMA, SHIN_FUJI, SHIZUOKA, KAKEGAWA, HAMAMATSU
		, TOYOHASHI, MIKAWA_ANJO, NAGOYA, GIFU_HASHIMA, MAIBARA
		, KYOTO, SHIN_OSAKA, SHIN_KOBE, NISHI_AKASHI, HIMEJI
		, AIOI, OKAYAMA, SHIN_KURASHIKI, FUKUYAMA, SHIN_ONOMICHI
		, MIHARA, HIGASHI_HIROSHIMA, HIROSHIMA, SHIN_IWAKUNI, TOKUYAMA
		, SHIN_YAMAGUCHI, ASA, SHIN_SHIMONOSEKI, KOKURA, HAKATA
		, MAX_STATION} station_t;
char *station[] = {"Tokyo", "Shinagawa", "Shin-Yokohama", "Odawara", "Atami"
		, "Mishima", "Shin-Fuji", "Shizuoka", "Kakegawa", "Hamamatsu"
		, "Toyohashi", "Mikawa-anjo", "Nagoya", "Gifu-hashima", "Maibara"
		, "Kyoto", "Shin-Osaka", "Shin-Kobe", "Nishi-Akashi", "Himeji"
		, "Aioi", "Okayama", "Shin-Kurashiki", "Fukuyama", "Shin-Onomichi"
		, "Mihara", "Higashi-Hiroshima", "Hiroshima", "Shin-Iwakuni", "Tokuyama"
		, "Shin-Yamaguchi", "Asa", "Shin-Shimonoseki", "Kokura", "Hakata"};

typedef struct TICKET {
	short car_n;
	short seat_n;
	char seat_c;
	station_t from;
	station_t to;
	char *comment;
} ticket_t;

ticket_t *list[LIST_SIZE];
char name[BUF_SIZE];

__attribute__((constructor))
void init(){
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	srand(time(NULL));
}

int main(void){
	int i;

	puts("Shinkansen Ticket vending machine");

	while(true){
		printf("[LOGIN] Input your name : ");
		if(!getnline(name, BUF_SIZE))
			break;

		while(true){
			switch(menu()){
				case 1:
					reserve();
					break;
				case 2:
					confirm();
					break;
				case 3:
					cancel();
					break;
				case 0:
					goto logout;
				default:
					puts("Wrong input.");
			}
		}

logout:
		for(i=0; i<LIST_SIZE; i++)
			list[i] = NULL;
		printf("[LOGOUT] Bye, %s!\n", name);
	}
	puts("Thank you!");
}

int menu(void){
	printf(	"\n"
		"1. Reservation\n"
		"2. Confirmation\n"
		"3. Cancel\n"
		"0. Logout\n"
		">> ");
	return getint();
}

void reserve(void){
	int i, id, n;

	for(i=0; i<LIST_SIZE && list[i]; i++);
	if(i>=LIST_SIZE){
		printf("Exceed maximum number of reservations...\n");
		return;
	}

	id = i;
	list[id] = malloc(sizeof(ticket_t));

	printf("Stations:\n");
	for(i=0; i<MAX_STATION; i++)
		printf("[%02d] %s\n", i, station[i]);

	printf("Station to get on ");
	while_input(n, n<0 || n>=MAX_STATION);
	list[id]->from = n;

	printf("Station to get off ");
	while_input(n, n<0 || n>=MAX_STATION);
	list[id]->to = n;

	printf("Car number(1-16) ");
	while_input(n, n<1 || n>16);
	list[id]->car_n = n;

	printf("Seat number(1-20) ");
	while_input(n, n<1 || n>20);
	list[id]->seat_n = n;
	list[id]->seat_c = 'A' + rand()%5;

	printf("Comment length >> ");
	n = getint();
	if(n<0x100){
		char *buf;
		buf = malloc(n);
		if(!buf)
			return;

		list[id]->comment = buf;
		printf("Comment >> ");
		getnline(buf, n);
	}
}

void confirm(void){
	int i;

	printf("Reserved Tickets\n");
	for(i=0; i<LIST_SIZE; i++){
		ticket_t *l = list[i];
		if(!l)	continue;

		printf(	"#========================================#\n"
			"ID : %d (%s - %s) %d-%d%c\n"
			"comment : %s\n"
			, i+1, station[l->from], station[l->to]
			, l->car_n, l->seat_n, l->seat_c, l->comment ? : "None");
	}
	printf("#========================================#\n");
}

void cancel(void){
	int id;

	printf("Input the ID to cancel >> ");
	id = getint();

	if(id < 0 || id > LIST_SIZE){
		printf("Out of range...\n");
		return;
	}

	id-=1;
	if(!list[id]){
		printf("Nothing to do...\n");
		return;
	}

	free(list[id]->comment);
	free(list[id]);
	list[id] = NULL;
}

int getnline(char *buf, int size){
	char *lf;

	if(size < 0)
		return 0;

	fgets(buf, size-1, stdin);
	if((lf=strchr(buf,'\n')))
		*lf='\0';

	return strlen(buf);
}

int getint(void){
	char buf[BUF_SIZE];

	getnline(buf, sizeof(buf));
	return atoi(buf);
}

