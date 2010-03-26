typedef struct {
    int x0, y0;			/* xmin and ymin */
    int x1, y1;			/* xmax and ymax (inclusive) */
} Window;

void fill(int x, int y, unsigned char threshold, Window *win, unsigned char *data, unsigned char *mask, int bytesPerRow, int bytesPerPixel, int *totalOut);
