#include <stdio.h>
#include <math.h>

#define MENUHIT_TAG 1

void js_begincurve()
{
	fputs("displaylist = [];\n", stdout);
	fflush(stdout);
}

void js_endcurve()
{
	puts("paintchart();\n");
	fflush(stdout);
}

int js_menuhit(int *index)
{
	int tag;
	char fmt[80];
	sprintf(fmt, "%d", MENUHIT_TAG);
	if (fscanf(stdin, fmt, &tag)) {
		fscanf(stdin, "%d", index);
	}
	return tag;
}

void js_lineto(float x, float y)
{
	fprintf(stdout,
			"displaylist.push(function (cx) { cx.lineTo(%f, %f); });\n", x,
			y);
	fflush(stdout);
}

int main(int argc, char *argv[])
{
	int index;
	float x, y;

	//setbuf(stdin, 0);
	//setbuf(stdout, 0);

	while (!feof(stdin)) {

		if (scanf("%d", &index) < 1)
			break;

		//fprintf(stderr, "graph: index = %d\n", index);

		js_begincurve();
		/* Start defining the display list. */

		for (x = 0; x <= 13; x += .1) {

			switch (index) {
			case 0:
				y = sin(x);
				break;
			case 1:
				y = cos(x);
				break;
			case 2:
				y = sin(x) * exp(-x / 3) * 3;
				break;
			case 3:
				y = sin(x) + .1 * sin(x * 5 + 1);
				break;
			default:
				y = 0;
			}

			js_lineto(x, y);
			/* Add point to the display list. */
		}

		js_endcurve();
		/* Finish off the display list. */
	}
}
