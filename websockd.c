#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <openssl/sha.h>
#include <sys/select.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>
#include <libgen.h>
#include "base64.h"
#include "websocket.h"

#define IOBUFSZ 4096
#define MAXPATH 512

#define MAX(x, y) (x > y ? x : y)


void log_err(int status, int errnum, const char *msg)
{
	syslog(LOG_ERR, msg);
	if (errnum > 0)
		syslog(LOG_ERR, strerror(errnum));
	if (status > 0)
		exit(status);
}


void log_debug(const char *msg)
{
	syslog(LOG_DEBUG, msg);
}


void sig_pipe(int signo)
{
	log_err(EXIT_FAILURE, 0, "received SIGPIPE, exiting");
}


struct buf {
	char *base;
	char *ptr;
	int cnt;
};


void service(const char *cmd)
{
	int req[2], res[2];			/* request and response pipes */
	int pid, nfds, nr;
	fd_set readers;
	char buf[IOBUFSZ];
	struct buf line = { 0 };
	int i, n;
	char path[MAXPATH];

	websocket_open();

	if (pipe(req) < 0 || pipe(res) < 0)
		log_err(EXIT_FAILURE, errno, "pipe failed");

	if ((pid = fork()) == 0) {	/* child */
		close(req[1]);
		close(res[0]);

		if (dup2(req[0], STDIN_FILENO) != STDIN_FILENO)
			log_err(EXIT_FAILURE, errno, "dup2 failed");
		close(req[0]);

		if (dup2(res[1], STDOUT_FILENO) != STDOUT_FILENO)
			log_err(EXIT_FAILURE, errno, "dup2 failed");
		close(res[1]);

		snprintf(path, MAXPATH, "%s%s", "/usr/sbin/", cmd);
		if (execlp(path, cmd, (char *) 0) < 0)
			log_err(EXIT_FAILURE, errno, "execlp failed");
	} else if (pid > 0) {		/* parent */
		close(req[0]);
		close(res[1]);
		for (;;) {
			FD_ZERO(&readers);
			FD_SET(STDIN_FILENO, &readers);
			FD_SET(res[0], &readers);
			nfds = MAX(res[0], STDIN_FILENO) + 1;
			nfds = select(nfds, &readers, NULL, NULL, NULL);
			if (nfds < 0)
				log_err(EXIT_FAILURE, errno, "select error");
			/* Read from pipe. */
			if (FD_ISSET(res[0], &readers)) {
				if ((nr = read(res[0], buf, IOBUFSZ)) < 0)
					log_err(EXIT_FAILURE, errno, "read error");
				for (i = 0; i < nr; i++) {
					if (--line.cnt < 0) {
						n = line.ptr - line.base;
						line.base = realloc(line.base, n + IOBUFSZ);
						line.ptr = line.base + n;
						line.cnt = IOBUFSZ;
					}
					if ((*line.ptr++ = buf[i]) == '\n') {
						n = line.ptr - line.base;
						line.base[n] = 0;
						if (websocket_write(line.base, n) < 0)
							log_err(EXIT_FAILURE, 0, "websocket_write error");
						line.ptr = line.base;
						line.cnt += n;
					}
				}
			}
			/* Read from socket. */
			if (FD_ISSET(STDIN_FILENO, &readers)) {
				if ((nr = websocket_read(buf, IOBUFSZ - 1)) < 0)
					log_err(EXIT_FAILURE, 0, "websocket_read error");
				if (nr == 0)
					break;
				buf[nr++] = '\n';
				buf[nr] = 0;
				if (write(req[1], buf, nr) < 0)
					log_err(EXIT_FAILURE, errno, "write error");
			}
		}
		kill(pid, SIGKILL);
	} else {
		log_err(EXIT_FAILURE, errno, "fork error");
	}

	close(res[0]);
	close(req[1]);

	websocket_close(STDIN_FILENO);
}


int main(int argc, char *argv[])
{
	if (argc != 1) {
		fprintf(stderr, "usage: %s\n", argv[0]);
		exit(1);
	}
	service(argv[0]);
	exit(0);
}
