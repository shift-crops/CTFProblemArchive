//// gcc -m32 shopping.c -o shopping
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 32

typedef struct PRODUCT{
	char name[BUF_SIZE];
	unsigned int price;
} product;

typedef struct CART{
	product *p;
	unsigned int n;
} cart;

product *l_product=NULL;
int c_product=0;

cart *l_cart=NULL;
int c_cart=0;

__attribute__((constructor))
init(){
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
}

void add_product(void);
void list_product(void);
void add_cart(void);
void list_cart(void);
int getint(void);

void shop(void){
	int menu;

	printf("\n#### SHOP MODE ####");
	do{
		printf(	"\n"
			"1. Add product to list\n"
			"2. List product\n"
			"3. Reset product list\n"
			"0. Return\n"
			": ");
		menu = getint();
		switch(menu){
			case 0:
				break;
			case 1:
				add_product();
				break;
			case 2:
				list_product();
				break;
			case 3:
				free(l_product);
				l_product=NULL;
				c_product=0;
				free(l_cart);
				l_cart=NULL;
				c_cart=0;
				break;
			default:
				printf("Wrong Input...\n");
		}
	}while(menu);
}

void customer(void){
	int menu;

	printf("\n#### CUSTOMER MODE ####");
	do{
		printf(	"\n"
			"1. Add to cart\n"
			"2. List cart\n"
			"3. Reset cart\n"
			"0. Return\n"
			": ");
		menu = getint();
		switch(menu){
			case 0:
				break;
			case 1:
				add_cart();
				break;
			case 2:
				list_cart();
				break;
			case 3:
				free(l_cart);
				l_cart=NULL;
				c_cart=0;
				break;
			default:
				printf("Wrong Input...\n");
		}
	}while(menu);
}

int main(void){
	int menu;

	printf("== SHOPPING ==");
	do{
		printf(	"\n"
			"1. Shop mode\n"
			"2. Customer mode\n"
			"0. Exit\n"
			": ");
		menu = getint();
		switch(menu){
			case 0:
				break;
			case 1:
				shop();
				break;
			case 2:
				customer();
				break;
			default:
				printf("Wrong Input...\n");
		}
	}while(menu);
}

void add_product(void){
	product *l_p;
	int price;

	l_p = (product *)realloc(l_product, sizeof(product)*(c_product+1));
	if(!l_p) exit(0);

	if(l_product!=l_p){
		int i;
		for(i=0; i<c_cart; i++)
			l_cart[i].p = (product *)((int)l_cart[i].p+(int)l_p-(int)l_product);
		l_product=l_p;
	}

	memset(l_product[c_product].name, 0, BUF_SIZE);

	printf("Product id : %d\n", c_product);
	printf("Price >>");
	price = getint();

	if(price>0){
		char *lf;
		printf("Name >>");
		fgets(l_product[(char)c_product].name, BUF_SIZE, stdin);
		if(lf=strchr(l_product[(char)c_product].name,'\n'))
			*lf='\0';
		l_product[(char)c_product].price = price;
	}
	c_product++;
	printf("ADD PRODUCT SUCCESS\n");
}

void list_product(void){
	int i;

	printf("&&&&&& PRODUCT &&&&&&\n");
	for(i=0; i<c_product; i++)
		printf("%03d : %s(%u$)\n", i, l_product[(char)i].name, l_product[(char)i].price);
	printf("LIST DONE\n");
}

void add_cart(void){
	int id, amount, i;

	printf("Product id >>");
	id = getint();
	if(id<0 || id>=c_product)
		return;

	printf("Amount >>");
	amount = getint();
	if(amount<=0)
		return;

	for(i=0; i<c_cart; i++)
		if(l_cart[i].p==&l_product[id])
			break;

	if(i==c_cart){
		l_cart = (cart *)realloc(l_cart, sizeof(cart)*++c_cart);
		if(!l_cart) exit(0);

		l_cart[i].p=&l_product[id];
		l_cart[i].n=0;
	}

	l_cart[i].n += amount;
	printf("ADD TO CART SUCCESS\n");
}

void list_cart(void){
	int i;
	unsigned int sum;

	printf("$$$$$$ CART $$$$$$\n");
	for(i=0, sum=0; i<c_cart; i++){
		printf("%s(%u$) x %d\n", l_cart[i].p->name, l_cart[i].p->price, l_cart[i].n);
		sum += l_cart[i].p->price*l_cart[i].n;
	}
	printf("Total: %u$\n", sum);
}

int getint(void){
	char buf[BUF_SIZE];

	fgets(buf, sizeof(buf), stdin);
	return atoi(buf);
}
