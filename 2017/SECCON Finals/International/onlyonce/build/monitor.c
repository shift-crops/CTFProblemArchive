// gcc monitor.c -o monitor && strip monitor

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/syscall.h>

#define RD		0
#define WR		1
#define NR_syscalls	546

typedef struct{
	pid_t pid;
	int status;
	int pipe[3];
	struct user_regs_struct regs;
	int syscall;
	unsigned long long ret;
} PROC_INFO;

int mvee(int n, char *argv[]);
int chk_syscall(PROC_INFO *master, PROC_INFO slave[]);
int emu_read(PROC_INFO *master, PROC_INFO slave[]);
int emu_write(PROC_INFO *master, PROC_INFO slave[]);

pid_t p_run(char* argv[], PROC_INFO *p);
int p_get_syscall(PROC_INFO *p);
int p_do_syscall(PROC_INFO *p);
int p_kill(PROC_INFO *p);

int main(int argc, char *argv[]){
	if(argc>1)
		mvee(2, &argv[1]);
}

int mvee(int n, char *argv[]){
	int i, c;
	PROC_INFO master, *slave;

	slave = (PROC_INFO*)malloc(sizeof(PROC_INFO)*(n+1));
	if(!slave)
		return -1;

	if(!p_run(argv, &master))
		return -1;

	for(i=0; i<n; i++)
		if(!p_run(argv, &slave[i]))
			break;
	slave[n].pid = 0;

	if(i<n)
		goto err;

	while((c=chk_syscall(&master, slave)) == n);

	if(!WIFEXITED(master.status)){
		char *errmsg;

		switch(c){
			case -0x10:
				errmsg = "Error Occurred!!";
				break;
			case -1:
				errmsg = "Systemcall Responces Different!!";
				break;
			default:
				errmsg = "Exploitation Detected!!";
		}
		dprintf(STDERR_FILENO, 	"[MVEEs] %s\n"
					"Sending SIGKILL to all processes...\n", errmsg);

		goto err;
	}

	return 0;

err:
	p_kill(&master);
	for(i=0; i<n; i++)
		p_kill(&slave[i]);
	return 1;
}

int chk_syscall(PROC_INFO *master, PROC_INFO slave[]){
	int i, ret = 0;

	if(!p_get_syscall(master))
		return -0x10;
	for(i=0; slave[i].pid; i++)
		if(!p_get_syscall(&slave[i]) || master->syscall^slave[i].syscall)
			goto end;

	switch(master->syscall){
		case SYS_read:
			ret = emu_read(master, slave);
			break;
		case SYS_write:
			ret = emu_write(master, slave);
			break;
		default:
			if(!p_do_syscall(master))
				return -0x10;
			for(i=0; slave[i].pid; i++)
				if(!p_do_syscall(&slave[i]))
					goto end;
	}

end:
	return ret<0 ? ret : i;
}

int emu_read(PROC_INFO *master, PROC_INFO slave[]){
	int i, fd;
	signed long long count;
	size_t len;
	char *buf;

	fd = master->regs.rdi;
	if(fd==STDIN_FILENO){
		count = master->regs.rdx;

		// 64bit overflow vuln
		buf = malloc(count+1);
		if(!buf)
			goto err;

		len = read(STDIN_FILENO, buf, (unsigned)count);

		if(count > 0)
			write(master->pipe[STDIN_FILENO], buf, len);
		if(!p_do_syscall(master))
			goto err;

		for(i=0; slave[i].pid; i++){
			if(count > 0)
				write(slave[i].pipe[STDIN_FILENO], buf, len);
			if(!p_do_syscall(&slave[i]))
				goto err;

			if(master->ret ^ slave[i].ret)
				goto err;
		}

		// memory leak
		if(count > 0)
			free(buf);
	}

	return 0;

err:
	free(buf);
	return -1;
}

int emu_write(PROC_INFO *master, PROC_INFO slave[]){
	int i, fd;
	signed long long count;
	size_t len;
	char *mbuf, *sbuf;

	fd = master->regs.rdi;
	if(fd==STDOUT_FILENO || fd==STDERR_FILENO){
		count = master->regs.rdx;

		// 64bit overflow vuln
		mbuf = malloc(count+1);
		sbuf = malloc(count+1);

		if(!mbuf || !sbuf)
			goto err;

		if(!p_do_syscall(master))
			goto err;
		len = read(master->pipe[fd], mbuf, count);

		for(i=0; slave[i].pid; i++){
			if(!p_do_syscall(&slave[i]))
				goto err;
			read(slave[i].pipe[fd], sbuf, len);

			if((master->ret ^ slave[i].ret) || (count > 0 && memcmp(mbuf, sbuf, len)))
				goto err;
		}

		write(fd, mbuf, len > 0x10000 ? 0x10000 : len);
		free(mbuf);
		free(sbuf);
	}

	return 0;

err:
	free(mbuf);
	free(sbuf);
	return -1;
}

pid_t p_run(char* argv[], PROC_INFO *p){
	pid_t pid;
	int i, status;
	int pipe_in[2], pipe_out[2], pipe_err[2];

	pipe(pipe_in);
	pipe(pipe_out);
	pipe(pipe_err);

	switch(pid=fork()){
		case 0:
			close(pipe_in[WR]);
			close(pipe_out[RD]);
			close(pipe_err[RD]);

			dup2(pipe_in[RD], STDIN_FILENO);
			dup2(pipe_out[WR], STDOUT_FILENO);
			dup2(pipe_err[WR], STDERR_FILENO);

			for(i=STDERR_FILENO+1; i<0xff; i++)
				close(i);

			if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1){
				perror("ptrace");
				exit(-1);
			}

			execvp(argv[0], argv);
			perror("execvp");
			break;
		case -1:
			perror("fork");
			break;
		default:
			p->pid = pid;

			close(pipe_in[RD]);
			close(pipe_out[WR]);
			close(pipe_err[WR]);

			p->pipe[STDIN_FILENO] = pipe_in[WR];
			p->pipe[STDOUT_FILENO] = pipe_out[RD];
			p->pipe[STDERR_FILENO] = pipe_err[RD];

			waitpid(pid, &status, 0);
			if(WIFEXITED(status) || WIFSIGNALED(status))
				return 0;
	}

	return pid;
}

int p_get_syscall(PROC_INFO *p){
	if(!p || !p->pid)
		return 0;

	ptrace(PTRACE_SYSCALL, p->pid, NULL, NULL);
	waitpid(p->pid, &p->status, 0);
	if(WIFEXITED(p->status) || WIFSIGNALED(p->status))
		return 0;

	ptrace(PTRACE_GETREGS, p->pid, NULL, &p->regs);
	p->syscall=p->regs.orig_rax;
	if(p->syscall < 0)
		return 0;

	return 1;
}

int p_do_syscall(PROC_INFO *p){
	if(!p || !p->pid)
		return 0;

	ptrace(PTRACE_SINGLESTEP, p->pid, NULL, NULL);
	waitpid(p->pid, &p->status, 0);
	if(WIFEXITED(p->status) || WIFSIGNALED(p->status))
		return 0;

	ptrace(PTRACE_GETREGS, p->pid, NULL, &p->regs);
	p->ret = p->regs.rax;

	return 1;
}

int p_kill(PROC_INFO *p){
	int status;

	if(!p || !p->pid)
		return 0;

	ptrace(PTRACE_KILL, p->pid, NULL, NULL);
	waitpid(p->pid, &status, 0);

	return 1;
}
