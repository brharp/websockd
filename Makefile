
websockd: websockd.c websocket.c websocket.h base64.c
	$(CC) -g -o $@ $^ -lssl -lcrypto

pretty:
	indent -kr -ts4 *.c *.h

clean:
	rm -f websockd

install: websockd
	install websockd /usr/sbin
	/etc/rc.d/rc.inetd restart

