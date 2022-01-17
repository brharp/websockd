#include <stdio.h>

void
beginpaint()
{
	printf("dl = new Array();\n");
}

void
endpaint()
{
	printf("dl.forEach((op) => op(cx));\n");
}

void
beginpath()
{
	printf("dl.push((cx) => cx.beginPath());\n");
}

void
clearrect(int x, int y, int width, int height)
{
	printf("dl.push((cx) => cx.clearRect(%d, %d, %d, %d));\n", x, y, width, height);
}

void
filltext(const char *s, int x, int y)
{
	printf("dl.push((cx) => cx.fillText('%s', %d, %d));\n", s, x, y);
}

void
lineto(int x, int y)
{
	printf("dl.push((cx) => cx.lineTo(%d, %d));\n", x, y);
}

void
moveto(int x, int y)
{
	printf("dl.push((cx) => cx.moveTo(%d, %d));\n", x, y);
}

void
restore()
{
	printf("dl.push((cx) => cx.restore());\n");
}

void
save()
{
	printf("dl.push((cx) => cx.save());\n");
}

void
setlinewidth(double x)
{
	printf("dl.push((cx) => cx.lineWidth = %f);\n", x);
}

void
setstrokestyle(const char *s)
{
	printf("dl.push((cx) => cx.strokeStyle = '%s');\n", s);
}

void
stroke()
{
	printf("dl.push((cx) => cx.stroke());\n");
}

void
strokerect(int x, int y, int width, int height)
{
	printf("dl.push((cx) => cx.strokeRect(%d, %d, %d, %d));\n", x, y, width, height);
}

void
translate(int x, int y)
{
	printf("dl.push((cx) => cx.translate(%d, %d));\n", x, y);
}



/*
void show(int i, int x, int y)
{
	printf("%s.fillText('%d', %d - %s.measureText('%d').width / 2, %d)\n",
		cx, fret[i], x, cx, fret[i], y);
}
*/


