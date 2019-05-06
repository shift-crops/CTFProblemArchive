#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 64

void enquete(unsigned long enquete_str, unsigned long buf);
int getaline(char *buf);

int main(void){
	char buf[BUF_SIZE];
	int enquete_id;
	char *enquete_list[]={
		"How old are you?",
		"Where are you from?",
		"Do you like pwning?",
		"How do you like this CTF?"
	};

	srand(time(NULL));
	enquete_id = rand() % (sizeof(enquete_list)/sizeof(char*));

	enquete((unsigned long)enquete_list[enquete_id], (unsigned long)buf);
	dprintf(STDOUT_FILENO, "Thank you for your cooperation in the enquete!\n");
}

void enquete(unsigned long enquete_str, unsigned long buf){
	char name[BUF_SIZE];

	dprintf(STDOUT_FILENO,	"What is your name?"
				"\n>> ");
	getaline(name);

	dprintf(STDOUT_FILENO, "Hi, %s\n%s\n>> ", name, (char*)enquete_str);
	getaline((char*)buf);
}

int getaline(char *buf){
	char c = -1;
	int i;

	for(i=0; c && read(STDIN_FILENO, &c, 1); i++){
		if(!(c^'\n'))
			c = '\0';
		buf[i] = c;
	}

	return i;
}

