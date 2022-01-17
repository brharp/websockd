#include <stdio.h>

void
newlist(const char *dl)
{
	printf("%s = new DisplayList();\n", dl);
}

void
call(const char *dl, const char *cx)
{
	printf("%s.call(%s);\n", dl, cx);
}

void
beginpath(const char *cx)
{
	printf("%s.beginPath();\n", cx);
}

void
clearrect(const char *cx, int x, int y, int width, int height)
{
	printf("%s.clearRect(%d, %d, %d, %d);\n", cx, x, y, width, height);
}

void
clip(const char *cx)
{
	printf("%s.clip();\n", cx);
}

void
filltext(const char *cx, const char *s, int x, int y)
{
	printf("%s.fillText('%s', %d, %d);\n", cx, s, x, y);
}

void
lineto(const char *cx, int x, int y)
{
	printf("%s.lineTo(%d, %d);\n", cx, x, y);
}

void
moveto(const char *cx, int x, int y)
{
	printf("%s.moveTo(%d, %d);\n", cx, x, y);
}

void
rect(const char *cx, int x, int y, int width, int height)
{
	printf("%s.rect(%d, %d, %d, %d);\n", cx, x, y, width, height);
}

void
restore(const char *cx)
{
	printf("%s.restore();\n", cx);
}

void
save(const char *cx)
{
	printf("%s.save();\n", cx);
}

void
setlinewidth(const char *cx, double x)
{
	printf("%s.lineWidth = %f;\n", cx, x);
}

void
setstrokestyle(const char *cx, const char *s)
{
	printf("%s.strokeStyle = '%s';\n", cx, s);
}

void
stroke(const char *cx)
{
	printf("%s.stroke();\n", cx);
}

void
strokerect(const char *cx, int x, int y, int width, int height)
{
	printf("%s.strokeRect(%d, %d, %d, %d);\n", cx, x, y, width, height);
}

void
translate(const char *cx, int x, int y)
{
	printf("%s.translate(%d, %d);\n", cx, x, y);
}

