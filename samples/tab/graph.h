
void beginpaint();
void endpaint();

void beginpath();
void clearrect(int x, int y, int width, int height);
void filltext(const char *s, int x, int y);
void lineto(int x, int y);
void moveto(int x, int y);
void restore();
void save();
void setlinewidth(double x);
void setstrokestyle(const char *s);
void stroke();
void strokerect(int x, int y, int width, int height);
void translate(int x, int y);

