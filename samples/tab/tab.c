#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "graph.h"

#define MAXLEN 256
#define MAXFRET 24

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
	int fret;
	int duration;
	int stem;
	int beam;
	int hon;
	int flags;
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
	int i;

	bzero(tune, sizeof(tune));

	for (i = 0; i < MAXLEN; i++) {
		if ((tune[i].string = string[i]) == 0)
			break;
		tune[i].fret = fret[i];
		tune[i].duration = duration[i];
		tune[i].stem = stem[i];
		tune[i].beam = beam[i];
		tune[i].hon = hon[i];
	}

	length = i;
}

struct { int pos; int depth; int duration; } caret;

int depth(int i)
{
	int s;
	return ((s = tune[i].string) == 0 ? caret.depth : s) * em + em / 3;
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
	int i, x, y = 0, t;

	for (i = 0, x = 10, t = 0; i < index; x += width(i), t += delta(i), i++) {
		if (t > 0 && t % 48 == 0) {
			x = 0;
			y += 100;
		}
		if (t > 0 && t % 16 == 0) {
			x += 10;
		}
	}

	if (t > 0 && t % 48 == 0) {
		x = 0;
		y += 100;
	}
	if (t > 0 && t % 16 == 0) {
		x += 10;
	}

	r->x = x - em / 2;
	r->y = depth(i) + y - em + 2;
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

	save(cx);
	translate(cx, 0, y);
	sprintf(s, "%d", tune[i].fret);
	filltext(cx, s, x - strlen(s) * ex / 2, depth(i));
	if (tune[i].stem) {
		beginpath(cx);
		moveto(cx, x, 6 * em);
		lineto(cx, x, depth(i) + 2);
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
		filltext(cx, "H", x + width(i)/2, depth(i) - 5);
	}
	restore(cx);
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
	int i, x = 0, y = 0, t;
	const char *dl = "dl";

	newlist(dl);

	save(dl);
	clearrect(dl, 0, 0, cw, ch);

	staff(dl, x, y);

	for (i = 0, x = 10, t = 0; i < length; x += width(i), t += delta(i), i++) {
		if (t > 0 && t % 48 == 0) {
			bar(dl, x, y);
			x = 0;
			y += 100;
			staff(dl, x, y);
		}
		if (t > 0 && t % 16 == 0) {
			bar(dl, x, y);
			x += 10;
		}
		note(dl, i, x, y);
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

	if (newpos < 0 || newpos > length || newdepth < 1 || newdepth > 5)
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


void input(char c)
{
	int i, n, f;

	i = caret.pos;
	n = c - '0';
	f = tune[i].fret * 10;

	if (f + n > MAXFRET)
		f = n;
	else
		f += n;

	tune[i].fret = f;

	if (i == length) {
		tune[i].string = caret.depth;
		tune[i].duration = caret.duration;
		++length;
	}

	//tune[i].stem = (tune[i].duration > 2);

	repaint();
}

int main(int argc, char *argv[])
{
	int c;

	initune();

	setup();
	paint(cx);

	caret.pos = 0;
	caret.depth = 3;
	caret.duration = 4;

	while ((c = getchar()) != EOF) {
		switch (c) {
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
		case 'e':
			setdur(8);
			break;
		case 'q':
			setdur(4);
			break;
		default:
			if (isdigit(c)) {
				input(c);
			}
			break;
		}
	}
}


