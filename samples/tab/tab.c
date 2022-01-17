#include <stdio.h>
#include <string.h>
#include "graph.h"

struct rect { int x; int y; int width; int height; };

const char *cv = "cv";
const char *cx = "cx";
const char *dp = "dp";
const char *px = "px";

const int cw = 600;
const int ch = 800;
const int em = 16;
const int ex = 8;

/*
struct {
	int string;
	int fret;
	int duration;
	int stem;
	int beam;
	int hon;
} tune[] = {
	3, 0, 4, 1, 0, 0,
	5, 0, 4, 1, 0, 0,

};
*/

int string[] = {
 3, 5, 1, 2, 1, 2,
 2, 1, 1, 2, 2, 5, 3, 5,
 3, 1, 1, 2, 2, 3,
 3, 3, 3, 3, 2, 3, 3,
 3, 3, 5, 3, 3, 3,
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

int length = sizeof(string)/sizeof(*string);

struct { int pos; int depth; } caret;

int depth(int i)
{
	int s;
	return ((s = string[i]) == 0 ? caret.depth : s) * em + em / 3;
}

int width(int i)
{
	return 160 / duration[i];
}

int delta(int i)
{
	return 16 / duration[i];
}

void call(const char *fn)
{
	printf("dl.push((cx) => %s(cx));\n", fn);
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

void bar(int x, int y)
{
	beginpath();
	moveto(x, y + em);
	lineto(x, y + 5 * em);
	stroke();
}

void note(int i, int x, int y)
{
	char s[10];

	save();
	translate(0, y);
	sprintf(s, "%d", fret[i]);
	filltext(s, x - strlen(s) * ex / 2, depth(i));
	if (stem[i]) {
		beginpath();
		moveto(x, 6 * em);
		lineto(x, depth(i) + 2);
		stroke();
	}
	if (beam[i]) {
		save();
		setlinewidth(2);
		beginpath();
		moveto(x, 6 * em);
		lineto(x + width(i), 6 * em);
		stroke();
		restore();
	}
	if (hon[i]) {
		filltext("H", x + width(i)/2, depth(i) - 5);
	}
	restore();
}

void paintcaret()
{
	struct rect r;

	mtov(caret.pos, &r);

	save();
	setstrokestyle("red");
	strokerect(r.x, r.y, r.width, r.height);
	restore();
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

void paint()
{
	int i, x = 0, y = 0, t;

	beginpaint();
	save();
	clearrect(0, 0, cw, ch);

	printf("dl.push((cx) => staff(cx, %d, %d));\n", x, y);

	for (i = 0, x = 10, t = 0; i < length;
	     x += width(i), t += delta(i), i++) {
		if (t > 0 && t % 48 == 0) {
			bar(x, y);
			x = 0;
			y += 100;
			printf("dl.push((cx) => staff(cx, %d, %d));\n", x, y);
		}
		if (t > 0 && t % 16 == 0) {
			bar(x, y);
			x += 10;
		}
		note(i, x, y);
	}

	bar(x, y);

	paintcaret();
	restore();
	endpaint();

	fflush(stdout);
}

void setpos(int newpos, int newdepth)
{
	struct rect r;

	if (newpos < 0 || newpos > length || newdepth < 1 || newdepth > 5)
		return;

	printf("%s.save();", cx);

	mtov(caret.pos, &r);
	printf("%s.rect(%d, %d, %d, %d);", cx, r.x - 5, r.y - 5, r.width + 10, r.height + 10);

	caret.depth = newdepth;

	mtov(newpos, &r);
	printf("%s.rect(%d, %d, %d, %d);", cx, r.x - 5, r.y - 5, r.width + 10, r.height + 10);
	printf("%s.clip();", cx);

	//printf("%s.fillRect(0, 0, %d, %d)\n", cx, cw, ch);

	caret.pos = newpos;
	paint();

	printf("%s.restore();", cx);
}

int main(int argc, char *argv[])
{
	int c;

	setup();
	paint();

	caret.pos = 0;
	caret.depth = 3;

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
		}
	}
}


