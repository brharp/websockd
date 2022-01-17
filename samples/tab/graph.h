
void newlist(const char *dl);
void call(const char *dl, const char *cx);

void beginpath(const char *cx);
void clearrect(const char *cx, int x, int y, int width, int height);
void clip(const char *cx);
void filltext(const char *cx, const char *s, int x, int y);
void lineto(const char *cx, int x, int y);
void moveto(const char *cx, int x, int y);
void rect(const char *cx, int x, int y, int width, int height);
void restore(const char *cx);
void save(const char *cx);
void setlinewidth(const char *cx, double x);
void setstrokestyle(const char *cx, const char *s);
void stroke(const char *cx);
void strokerect(const char *cx, int x, int y, int width, int height);
void translate(const char *cx, int x, int y);

