#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "graph.h"

#define MAXLEN 256
#define MAXFRET 24
#define MAXSTR 5
#define MAXPATH 512

struct rect { int x; int y; int width; int height; };

const char *cv = "cv";
const char *cx = "cx";
const char *dp = "dp";
const char *px = "px";

const int cw = 600;
const int ch = 800;
const int em = 16;
const int ex = 8;

struct note {
	int string;
	int fret[MAXSTR];
	int duration;
	int stem;
	int beam;
	int hon;	/* Hammer on */
	int flags;
	int x;
	int y;
} tune[MAXLEN];

int string[] = {
 3, 5, 1, 2, 1, 2,
 2, 1, 1, 2, 2, 5, 3, 5,
 3, 1, 1, 2, 2, 3,
 3, 3, 3, 3, 2, 3, 3,
 3, 3, 5, 3, 3, 3,
 0
};

int fret[] = {
 0, 0, 4, 5, 0, 1,
 0, 0, 0, 0, 0, 0, 2, 0,
 0, 2, 0, 1, 0, 2,
 0, 0, 1, 2, 1, 2, 0,
 0, 0, 0, 2, 0, 2,
};

int duration[] = {
 4, 4, 8, 8, 8, 8,
 8, 8, 8, 8, 8, 8, 8, 8,
 4, 4, 8, 8, 8, 8,
 4, 8, 8, 8, 8, 8, 8, 
 4, 8, 8, 4, 8, 8,
};

int stem[] = {
 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1,
};

int beam[] = {
 0, 0, 1, 0, 1, 0,
 1, 0, 1, 0, 1, 0, 1, 0,
 0, 0, 1, 0, 1, 0,
 0, 1, 0, 1, 0, 1, 0,
 0, 1, 0, 0, 1, 0,
};

int hon[] = {
 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 1, 0,
};

int time[] = { 4, 4 };

int length;

void initune()
{
	int i, j;

	bzero(tune, sizeof(tune));

	for (i = 0; i < MAXLEN; i++) {
		if ((j = string[i]) == 0)
			break;
		tune[i].string = 1 << (j - 1);
		tune[i].fret[j-1] = fret[i];
		tune[i].duration = duration[i];
		tune[i].stem = stem[i];
		tune[i].beam = beam[i];
		tune[i].hon = hon[i];
	}

	length = i;
}

struct { int pos; int depth; int duration; } caret;

int depth(int j)
{
	return (j + 1) * em + em / 3;
}

int width(int i)
{
	return 160 / tune[i].duration;
}

int delta(int i)
{
	return 16 / tune[i].duration;
}

void mtov(int index, struct rect *r)
{
	int x, y;

	x = tune[index].x;
	y = tune[index].y;

	/* Center rectangle around midpoint of position. */
	r->x = x - em / 2;
	r->y = depth(caret.depth) + y - em + 2;
	r->width = em;
	r->height = em;
}

void staffline(const char *cx)
{
	translate(cx, 0, 16);
	beginpath(cx);
	moveto(cx, 0, 0);
	lineto(cx, 510, 0);
	stroke(cx);
}

void staff(const char *cx, int x, int y)
{
	int i;

	save(cx);
	translate(cx, 0, y);
	setstrokestyle(cx, "lightgray");
	for (i = 0; i < 5; i++) {
		staffline(cx);
	}
	restore(cx);
}

void bar(const char *cx, int x, int y)
{
	beginpath(cx);
	moveto(cx, x, y + em);
	lineto(cx, x, y + 5 * em);
	stroke(cx);
}

void note(const char *cx, int i, int x, int y)
{
	char s[10];
	int  j;
	int  maxj = 0;

	save(cx);
	translate(cx, 0, y);
	for (j = 0; j < MAXSTR; j++) {
		if (tune[i].string & (1 << j)) {
			sprintf(s, "%d", tune[i].fret[j]);
			filltext(cx, s, x - strlen(s) * ex / 2, depth(j));
			maxj = j > maxj ? j : maxj;
		}
	}
	if (tune[i].stem) {
		beginpath(cx);
		moveto(cx, x, 6 * em);
		lineto(cx, x, depth(maxj) + 2);
		stroke(cx);
	}
	if (tune[i].beam) {
		save(cx);
		setlinewidth(cx, 2);
		beginpath(cx);
		moveto(cx, x, 6 * em);
		lineto(cx, x + width(i), 6 * em);
		stroke(cx);
		restore(cx);
	}
	if (tune[i].flags) {
		beginpath(cx);
		moveto(cx, x, 6 * em);
		lineto(cx, x + ex, 5 * em);
		stroke(cx);
	}
	if (tune[i].hon) {
		filltext(cx, "H", x + width(i)/2, depth(0) - 5);
	}
	restore(cx);

	tune[i].x = x;
	tune[i].y = y;
}

void paintcaret(const char *cx)
{
	struct rect r;

	mtov(caret.pos, &r);

	save(cx);
	setstrokestyle(cx, "red");
	strokerect(cx, r.x, r.y, r.width, r.height);
	restore(cx);
}

void setup()
{
	printf("%s = window.devicePixelRatio\n", dp);
	printf("%s = document.getElementById('%s')\n", cv, cv);
	printf("%s.width = %d * %s\n", cv, cw, dp);
	printf("%s.height = %d * %s\n", cv, ch, dp);
	printf("%s.style.width = %d + '%s'\n", cv, cw, px);
	printf("%s.style.height = %d + '%s'\n", cv, ch, px);
	printf("%s = %s.getContext('2d')\n", cx, cv);
	printf("%s.fillStyle = '%s'\n", cx, "black");
	printf("%s.font = '%dpx sans'\n", cx, em);
	printf("%s.scale(%s, %s)\n", cx, dp, dp);
	printf("%s.translate(%d, %d)\n", cx, 30, 20);
}

void paint(const char *cx)
{
	int i, x = 0, dx, y = 0, t, dt, m, w;
	const char *dl = "dl";

	m = 16;
	w = 512;

	newlist(dl);

	save(dl);
	clearrect(dl, 0, 0, cw, ch);

	staff(dl, x, y);

	for (i = 0, x = 10, t = 0; i < length; i++) {
		dt = delta(i);
		dx = width(i);
		if (t + dt > m) {
			bar(dl, x, y);
			x += 10;
			t = 0;
		}
		if (x + dx > w) {
			x = 0;
			y += 100;
			staff(dl, x, y);
			bar(dl, x, y);
			x += 10;
		}
		note(dl, i, x, y);
		t += dt;
		x += dx;
	}

	bar(dl, x, y);

	paintcaret(dl);
	restore(dl);

	call(dl, cx);

	fflush(stdout);
}

void calcbeams()
{
	int i, t = 0, d1, d2, b;
	
	for (i = 0; i < length; i++) {
		tune[i].flags = (tune[i].duration == 8);
	}

	for (i = 0; i < length - 1; i++) {
		d1 = tune[i].duration;
		d2 = tune[i+1].duration;
		b = ((d1 == d2) && (d1 == 8) && (t % 4 == 0));
		tune[i].beam = b;
		t += delta(i);
		if (b) {
			tune[i].flags = 0;
			tune[i+1].flags = 0;
		}
	}
}

void repaint()
{
	calcbeams();
	paint(cx);
}

void setdur(int dur)
{
	caret.duration = dur;

	if (caret.pos < length) {
		tune[caret.pos].duration = dur;
		repaint();
	}
}

void setpos(int newpos, int newdepth)
{
	struct rect r;

	if (newpos < 0 || newpos > length || newdepth < 0 || newdepth > 4)
		return;

	save(cx);

	mtov(caret.pos, &r);
	rect(cx, r.x - 1, r.y - 1, r.width + 2, r.height + 2);

	caret.depth = newdepth;

	mtov(newpos, &r);
	rect(cx, r.x - 1, r.y - 1, r.width + 2, r.height + 2);

	clip(cx);

	caret.pos = newpos;
	paint(cx);

	restore(cx);
}


void clear()
{
	int i, j;

	if ((i = caret.pos) < length) {
		j = caret.depth;
		tune[i].string &= ~(1 << j);
		tune[i].stem = (tune[i].string != 0);
		repaint();
	}
}


void input(char c)
{
	int i, n, f, g;

	i = caret.pos;
	n = c - '0';
	g = caret.depth;
	f = tune[i].fret[g] * 10;

	if (f + n > MAXFRET)
		f = n;
	else
		f += n;

	tune[i].string |= 1 << g;
	tune[i].fret[g] = f;

	if (i == length) {
		tune[i].duration = caret.duration;
		++length;
	}

	tune[i].stem = (tune[i].duration > 2);

	repaint();
}

void doctrlinput(const char *s)
{
	switch (*s) {
	case '4':
		setdur(4);
		break;
	case '8':
		setdur(8);
		break;
	}
}

void del()
{
	int i = caret.pos;
	int n = (--length) - i;

	memmove(&tune[i], &tune[i+1], n * sizeof(struct note));

	repaint();
}

void insert()
{
	if (length < MAXLEN) {
		int i = caret.pos;
		int n = (length++) - i;
		memmove(&tune[i+1], &tune[i], n * sizeof(struct note));
		bzero(&tune[i], sizeof(tune[i]));
		tune[i].duration = caret.duration;
		repaint();
	}
}

void alert(const char *message)
{
	printf("alert('%s');\n", message);
	fflush(stdout);
}

void prompt(char *buffer, int bufsiz, const char *message)
{
	const char *s = "s";
	const char *ws = "ws";
	int n;

	printf("%s = prompt('%s', '%s');\n", s, message, buffer);
	printf("console.log('User entered ' + %s);\n", s);
	printf("%s.send(%s);\n", ws, s);
	fflush(stdout);
	scanf("%s", buffer);
	n = strlen(buffer);
	if (n > 0 && buffer[n-1] == '\n') {
		buffer[n-1] = 0;
	}
}

void savefile()
{
	char filename[80];
	char pathname[MAXPATH];
	int nw;
	FILE *fp;

	strcpy(filename, "");
	prompt(filename, sizeof(filename), "Save as");
	if (strlen(filename) > 0) {
		strcat(strcpy(pathname, "/var/tmp/"), filename);
		fp = fopen(pathname, "w");
		if (fp == NULL) {
			alert("File not found");
			return;
		}
		nw = fwrite(tune, sizeof tune[0], length, fp);
		if (nw < length) {
			fclose(fp);
			alert("Save failed.");
			return;
		}
		fclose(fp);
		alert("Saved");
	}
}

void loadfile()
{
	char filename[80];
	char pathname[MAXPATH];
	int nr, nb;
	FILE *fp;

	strcpy(filename, "");
	prompt(filename, sizeof(filename), "Load from:");
	if (strlen(filename) > 0) {
		strcat(strcpy(pathname, "/var/tmp/"), filename);
		fp = fopen(pathname, "r");
		if (fp == NULL) {
			alert("File not found");
			return;
		}
		fseek(fp, 0, SEEK_END);
		nb = ftell(fp);
		rewind(fp);
		length = nb / sizeof tune[0];
		nr = fread(tune, sizeof tune[0], length, fp);
		if (nr < length) {
			fclose(fp);
			alert("Load failed.");
			return;
		}
		fclose(fp);
	}
	repaint();
}

int main(int argc, char *argv[])
{
	char cmd[80];

	initune();

	setup();
	paint(cx);

	caret.pos = 0;
	caret.depth = 0;
	caret.duration = 4;

	while ((scanf("%s", cmd)) == 1) {
		switch (cmd[0]) {
		case 'l':
			setpos(caret.pos + 1, caret.depth);
			break;
		case 'h':
			setpos(caret.pos - 1, caret.depth);
			break;
		case 'j':
			setpos(caret.pos, caret.depth + 1);
			break;
		case 'k':
			setpos(caret.pos, caret.depth - 1);
			break;
		case 'x':
			clear();
			break;
		case 'C':
			if (strncmp(cmd, "Ctrl", 4) == 0) {
				doctrlinput(cmd+4);
			}
			break;
		case 'D':
			del();
			break;
		case 'I':
			insert();
			break;
		case 'L':
			loadfile();
			break;
		case 'S':
			savefile();
			break;
		default:
			if (isdigit(*cmd)) {
				input(*cmd);
			}
			break;
		}
	}
}


