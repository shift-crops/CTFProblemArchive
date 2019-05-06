// gcc -Wall streaming.c -lmysqlclient -L/usr/lib64/mysql/ -o streaming
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <signal.h>
#include <poll.h>
#include <grp.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include <mysql/mysql.h>

#define TIMEOUT		300
#define PORT 		8000

typedef struct settings {
	struct {
		char *addr;
		char *user;
		char *pass;
		char *database;
	} db;

	struct {
		char addr[0x20];
		int port;
	} rank;

	struct {
		gid_t gid;
		int port;
	} stream;
} SETTINGS;

static void help(char *name);
static int server(SETTINGS *set);
static int service_main(SETTINGS *set, int fd);
static void set_sighandler(int signum,void *func);

__attribute__((constructor))
void init(void){
	//set_sighandler(SIGCHLD, wait_child);
	set_sighandler(SIGCHLD, SIG_IGN);
}

int main(int argc, char *argv[]){
	char opt;
	int long_opt;
	struct option long_options[] = {
		{"db_addr", 	required_argument, &long_opt, 'a'},
		{"db_user", 	required_argument, &long_opt, 'u'},
		{"db_pass", 	required_argument, &long_opt, 'p'},
		{"db_name", 	required_argument, &long_opt, 'n'},
		{"rank", 	required_argument, &long_opt, 'r'},
		{"gid", 	required_argument, &long_opt, 'g'},
		{"listen", 	required_argument, &long_opt, 'l'},
		{"help", 	no_argument,       &long_opt, 'h'},
		{0, 0, 0, 0}};
	SETTINGS set = {
		.db = {
			.addr = "localhost",
			.user = "root",
			.pass = "pass",
			.database = "db_test",
		},

		.rank = {
			.addr = "localhost",
			.port = 4296,
		},

		.stream = {
			.gid = 10000,
			.port = PORT,
		},
	};

	gid_t gid;
	int i;

	while((opt=getopt_long(argc, argv, "-h", long_options, NULL))!=-1 && opt!=1)
		switch(opt){
			case 0:
				switch(long_opt){
					case 'a':
					case 'u':
					case 'p':
					case 'n':
						{
						char *s;
						s = alloca(strlen(optarg)+1);
						strcpy(s, optarg);
						for(i=0; optarg[i]; i++)
							optarg[i] = '\0';

						switch(long_opt){
							case 'a':
								set.db.addr = s;
								break;
							case 'u':
								set.db.user = s;
								break;
							case 'p':
								set.db.pass = s;
								break;
							case 'n':
								set.db.database = s;
								break;
						}
						}
						break;
					case 'r':
						if(sscanf(optarg, "%31[^:]:%d", set.rank.addr, &set.rank.port) < 2)
							goto help;
#ifdef DEBUG
						dprintf(STDERR_FILENO, "Ranking Server %s:%d\n", set.rank.addr, set.rank.port);
#endif
						break;
					case 'g':
						set.stream.gid = atoi(optarg);
						break;
					case 'l':
						set.stream.port = atoi(optarg);
						break;
					case 'h':
						goto help;
				}
				break;
			case 'h':
			case ':':
			case '?':
				goto help;
		}

	gid = set.stream.gid;
	if (setgroups(1, &gid) || setgid(gid)){
		perror("setgid");
		return -1;
	}

	return server(&set);

help:
	help(argv[0]);
}

static void help(char *name){
	dprintf(STDERR_FILENO,
			"StreamingServer -Fuchu-\n\n"

			"Usage :\n"
			"\t%s [options]\n\n", name);

	dprintf(STDERR_FILENO,
			"Options : \n"
			"\t--db_addr=DB_ADDR\t: Database Address\t(Default : 127.0.0.1)\n"
			"\t--db_user=DB_USER\t: Database Username\t(Default : root)\n"
			"\t--db_pass=DB_PASS\t: Database Password\t(Default : pass)\n"
			"\t--db_name=DB_NAME\t: Database Name\t\t(Default : db_test)\n"
			"\n"
			"\t--rank=ip:port\t\t: Ranking Server\t(Default : 127.0.0.1:4296)\n"
			"\n"
			"\t--gid=GID\t\t: Group ID\t\t(Default : 10000)\n"
			"\t--listen=LISTEN_PORT\t: Listen Port\t\t(Default : 8000)\n"
			"\n"
			"\t--help, -h\t\t: show this help\n");

	_exit(0);
}

static int server(SETTINGS *set){
	struct sockaddr_in	saddr;
	int			sfd;
	const int 		on = 1;

	if ((sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return -1;
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family      = AF_INET;
	saddr.sin_port        = htons(set->stream.port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr))) {
		perror("bind");
		return -1;
	}

	if(listen(sfd, 5)) {
		perror("listen");
		return -1;
	}

	while(1) {
		struct sockaddr_in	caddr;
		int 				cfd;
		socklen_t 			len;

		if((cfd = accept(sfd, (struct sockaddr*)&caddr, &len)) < 0) {
			perror("accept");
			exit(1);
		}

		switch(fork()){
			case 0:
				{
				struct rlimit rl = {
					.rlim_cur = 1,
					.rlim_max = 2,
				};
				uid_t uid;
				pid_t pid;

				close(sfd);

				if(setrlimit(RLIMIT_CPU, &rl) < 0){
					perror("setrlimit");
					exit(-1);
				}

				uid = ntohl(caddr.sin_addr.s_addr);
#ifdef DEBUG
				dprintf(STDERR_FILENO, "uid : %u\n", uid);
#endif
				if(!uid || setuid(uid)){
					perror("setuid");
					exit(-2);
				}

				pid = getpid();
				if(setpgid(pid, pid)){
					perror("setpgid");
					exit(-3);
				}


				alarm(TIMEOUT);
				service_main(set, cfd);
				write(cfd, "\xff", 1);

				shutdown(cfd, SHUT_RDWR);
				close(cfd);

				exit(0);
				}
			case -1:
				perror("child process");
				break;
			default:
				close(cfd);	
		}
	}
	
	shutdown(sfd, SHUT_RDWR);
	close(sfd);	
	return 0;
}

static void set_sighandler(int signum,void *func){
	struct sigaction act;

	memset(&act, 0, sizeof(act));

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NOCLDSTOP | SA_RESTART;

	sigaction(signum, &act, NULL);
}

#define MAX_BUF_SIZE 	0x1000
#define BUF_SIZE 	128
#define ID_SIZE 	36
#define KEY_SIZE 	64
#define min(x,y)	((x)<(y) ? (x) : (y))

struct ContentInfo {
	char *filepath;
	size_t size;
};

struct WavHeader {
	char		sRiff[4];
	uint32_t	nFileSize;
	char		sWave[4];
	char		sFmt[4];
	uint32_t	nFmtSize;
	uint16_t	wFormatTag;
	uint16_t	nChannels;
	uint32_t	nSamplesPerSec;
	uint32_t	nAvgBytesPerSec;
	uint16_t	nBlockAlign;
	uint16_t	wBitsPerSample;
	char		sData[4];
	uint32_t	nDataSize;
};

static void send_resp(int fd, uint8_t code, uint32_t meta, void *data, size_t size);
static int get_contentinfo(SETTINGS *set, struct ContentInfo *info, char *id);
static int incr_playcount(char *addr, int port, char *id, char *key);
static int send_message(char *addr, int port, char *buf, size_t len);

static int service_main(SETTINGS *set, int fd){
	char buf[0x10];
	char id[ID_SIZE+1]={}, apikey[KEY_SIZE+1]={};

	struct ContentInfo info = {};
	struct stat sb;
	FILE *fp = NULL;

	while(read(fd, buf, sizeof(uint8_t)) > 0){
		switch(*(uint8_t*)buf){
			case 0x80:
				if(fp){
					send_resp(fd, 0x40, -1, NULL, 0);
					continue;
				}
				break;
			case 0x81:
			case 0x82:
			case 0x83:
			case 0x84:
				if(!fp){
					send_resp(fd, 0x41, -1, NULL, 0);
					continue;
				}
				break;
			case 0x90:
				goto end;
			default:
				send_resp(fd, 0x50, -1, NULL, 0);
				continue;
		}

		switch(*(uint8_t*)buf){
			case 0x80:
#ifdef DEBUG
				dprintf(STDERR_FILENO, "Open Music\n");
#endif

				send_resp(fd, 0x10, ID_SIZE, NULL, 0);
				if(read(fd, id, ID_SIZE) < ID_SIZE){
#ifdef DEBUG
					dprintf(STDERR_FILENO, "Wrong ID Size\n");
#endif
					goto end;
				}

				if(!get_contentinfo(set, &info, id)){
#ifdef DEBUG
					dprintf(STDERR_FILENO, "get_contentinfo failed.\n");
#endif
					send_resp(fd, 0x42, -1, NULL, 0);
					break;
				}

				send_resp(fd, 0x11, KEY_SIZE, NULL, 0);
				if(read(fd, apikey, KEY_SIZE) < KEY_SIZE){
#ifdef DEBUG
					dprintf(STDERR_FILENO, "Wrong KEY Size\n");
#endif
					goto end;
				}

#ifdef DEBUG
				dprintf(STDERR_FILENO, "file : %s\n", info.filepath);
#endif
				stat(info.filepath, &sb);
				if(S_ISREG(sb.st_mode) && (fp = fopen(info.filepath, "rb")))
					send_resp(fd, 0x20, 0, NULL, 0);
				else{
#ifdef DEBUG
					dprintf(STDERR_FILENO, "open file failed.\n");
#endif
					send_resp(fd, 0x43, strlen(info.filepath), info.filepath, strlen(info.filepath));
					break;
				}

				break;
			case 0x81:
#ifdef DEBUG
				dprintf(STDERR_FILENO, "Close Music\n");
#endif

				fclose(fp);
				fp = NULL;

				incr_playcount(set->rank.addr, set->rank.port, id, apikey);
				memset(id, 0,sizeof(id));
				memset(apikey, 0, sizeof(apikey));

				free(info.filepath);
				memset(&info, 0,sizeof(info));
				memset(&sb, 0, sizeof(sb));

				send_resp(fd, 0x21, 0, NULL, 0);

				break;
			case 0x82:
				{
				struct WavHeader header = {"RIFF", info.size+0x28, "WAVE", "fmt ", 16, 1, 2, 0xac44, 0x02b110, 4, 16, "data", info.size};

#ifdef DEBUG
				dprintf(STDERR_FILENO, "Music Header\n");
#endif
				send_resp(fd, 0x22, sizeof(struct WavHeader), &header, sizeof(struct WavHeader));
				break;
				}
			case 0x83:
#ifdef DEBUG
				dprintf(STDERR_FILENO, "Music Seek\n");
#endif

				send_resp(fd, 0x13, sizeof(long), NULL, 0);
				if(read(fd, buf, sizeof(long)) < sizeof(long)){
#ifdef DEBUG
					dprintf(STDERR_FILENO, "Wrong Seek pos Size\n");
#endif
					goto end;
				}

				if(*(long*)buf > sb.st_size)
					send_resp(fd, 0x44, -1, NULL, 0);
				else{
					fseek(fp, *(long*)buf, SEEK_SET);
					send_resp(fd, 0x23, sb.st_size-ftell(fp), NULL, 0);
				}
				break;
			case 0x84:
				{
				char *content;
				size_t buf_size, remain, read_size;

#ifdef DEBUG
				dprintf(STDERR_FILENO, "Music Send\n");
#endif
				send_resp(fd, 0x14, sizeof(size_t), NULL, 0);
				if(read(fd, buf, sizeof(size_t)) < sizeof(size_t)){
#ifdef DEBUG
					dprintf(STDERR_FILENO, "Wrong Send Size\n");
#endif
					goto end;
				}

				buf_size = min(MAX_BUF_SIZE, info.size);
				content = alloca(buf_size);

				remain = min(sb.st_size-ftell(fp), *(size_t*)buf);
				if(remain < MAX_BUF_SIZE){
					read_size = fread(content, 1, remain, fp);
					send_resp(fd, 0x24, (uint32_t)read_size, content, read_size);
				}
				else
					for(; remain > 0; remain -= read_size){
						read_size = fread(content, 1, buf_size, fp);
						send_resp(fd, remain > read_size ? 0x34:0x24, (uint32_t)read_size, content, read_size);
					}
				break;
				}
		}
	}
end:
	return 0;
}

static void send_resp(int fd, uint8_t code, uint32_t meta, void *data, size_t size){
	struct __attribute__((__packed__)) {
		uint8_t code;
		uint32_t meta;
	} buf = {code, meta};

	write(fd, &buf, sizeof(buf));
	if(data && size > 0)
		write(fd, data, size);
}

static int get_contentinfo(SETTINGS *set, struct ContentInfo *info, char *id){
	MYSQL *conn     = NULL;
	MYSQL_RES *resp = NULL;
	MYSQL_ROW row;
	char sql_str[BUF_SIZE] = {0};

	conn = mysql_init(NULL);
	if(!mysql_real_connect(conn, set->db.addr, set->db.user, set->db.pass, set->db.database, 3306, NULL, 0))
		exit(-1);

	snprintf(sql_str ,sizeof(sql_str)-1 , "select file_path, data_byte_size from file where id='%s'", id);
#ifdef DEBUG
	dprintf(STDERR_FILENO, "SQL : %s\n", sql_str);
#endif
	if(mysql_query(conn, sql_str)){
		mysql_close(conn);
		exit(-1);
	}

	resp = mysql_use_result(conn);
	if((row = mysql_fetch_row(resp)) != NULL){
		info->filepath = strdup(row[0]);
		info->size = atoi(row[1]);
	}

	mysql_free_result(resp);
	mysql_close(conn);

	return row ? 1 : 0;
}

static int incr_playcount(char *addr, int port, char *id, char *key){
	char buf[BUF_SIZE*2] = {};

	snprintf(buf, sizeof(buf), 
			"GET /play/%s HTTP/1.1\r\n"
			"Host: %s:%d\r\n"
			"Connection: close\r\n"
			"Accept: */*\r\n"
			"X-FUCHU-KEY: %s\r\n"
			"\r\n"
			, id, addr, port, key);
	return send_message(addr, port, buf, strlen(buf));
}

static int send_message(char *addr, int port, char *msg, size_t len){
	int fd;
	struct sockaddr_in saddr = {};
	char buf[BUF_SIZE];

	if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("socket");
		return -1;
	}

	saddr.sin_family      = AF_INET;
	saddr.sin_port        = htons(port);

	saddr.sin_addr.s_addr = inet_addr(addr);
	if(saddr.sin_addr.s_addr == 0xffffffff){
		struct hostent *host;

		host = gethostbyname(addr);
		if(!host)
			return -1;
		saddr.sin_addr.s_addr = *(unsigned int *)host->h_addr_list[0];
	}

	if(connect(fd, (const struct sockaddr*)&saddr, sizeof(saddr)) < 0){
		perror("connect");
		return -1;
	}

	write(fd, msg, len);
#ifdef DEBUG
	dprintf(STDERR_FILENO, "%s", msg);
#endif

	shutdown(fd, SHUT_WR);

	while(read(fd, buf, sizeof(buf)) > 0){
#ifdef DEBUG
		dprintf(STDERR_FILENO, "%s", buf);
#endif
	}
	close(fd);

	poll(NULL, 0, 100);

	return 1;
}
