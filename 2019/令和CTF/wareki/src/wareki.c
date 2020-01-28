#include <stdio.h>
#include <unistd.h>

void readflag(void);
int christ2emperor(void);

int main(void){
	setbuf(stdout, NULL);
	alarm(30);

	printf("西暦 -> 和暦 変換サービス");
	for(;;) {
		int x = 0;

		printf(
		"\n"
		"1: 和暦変換\n"
		"2: フラグ読み込み\n"
		"0: 終了\n"
		">> "
		);
		scanf("%d", &x);
		switch(x){
			case 1:
				christ2emperor();
				break;
			case 2:
				readflag();
				break;
			default:
				goto end;
		}
	}

end:
	puts("Bye!");
}

void readflag(void){
	FILE *fp;
	unsigned offset;
	char buf[90] = {};

	if(!(fp = fopen("flag.txt", "r"))){
		perror("fopen");
		return;
	}

	printf("\nオフセット >> ");
	scanf("%d", &offset);

	fseek(fp, offset, SEEK_SET);
	fread(buf, sizeof(buf), 1, fp);
	fclose(fp);

	puts("Done!");
}

int christ2emperor(void){
	unsigned long year;
	char *era[] = {"明治", "大正", "昭和", "平成", "令和"};
	unsigned long first_year[] = {1868, 1912, 1926, 1989, 2019};
	int sel = 0, i;
	char c;

	printf("\n西暦 >> ");
	scanf("%lu", &year);

	printf("変換しますか？(Yes:1, No:Others) >> ");
	scanf("%d", &sel);
	if(sel != 1)
		return 0;

	for(i = sizeof(era)/sizeof(char *)-1; i >= 0;  i--){
		if(year >= first_year[i]){
			printf("変換結果：%s", era[i]);
			printf(year-first_year[i] ? " %lu 年\n" : " 元年\n", year-first_year[i]+1);
			goto fin;
		}
	}
	printf("明治以前は未実装\n");
	return -1;

fin:
	return 1;
}
