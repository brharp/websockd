src = $(wildcard *.c)
obj = $(src:.c=.o)
prognam = tab

LDFLAGS = 
CFLAGS = -DDEBUG -Wall -g 

a.out: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

install: a.out
	install a.out /usr/sbin/$(prognam)
	mkdir -p /var/www/htdocs/$(prognam)
	cp index.html $(prognam).js /var/www/htdocs/$(prognam)

.PHONY: clean
clean:
	rm -f $(obj) a.out
