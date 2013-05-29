#include <stdlib.h>
#include <stdint.h>

/* typedef struct { uint8_t red, green, blue, alpha; } rgba_pixel; */

/* rgba_pixel * init(int n) */
/* { */
/* 	int i=0;  */
/* 	rgba_pixel * buf = (rgba_pixel*)malloc(sizeof(rgba_pixel)*n); */
/* 	if(buf != NULL) { */
/* 		for (i=0; i<n; i++) */
/* 			buf[i].red = 128; */
/* 	} */

/* 	return buf; */
/* } */

static struct {
	int buf_sz;
	int* buf;
} g;

int init(int n)
{
	g.buf = malloc(n);
	if (g.buf == NULL) 
		return -1;
	g.buf_sz = n;
	return 0;
}

int set_buf(int i, int v)
{
	if (i<0 || i>g.buf_sz) 
		return -1;
	g.buf[i] = v;
	return 0;
}

int get_buf(int i)
{
	if (i<0 || i>g.buf_sz) 
		return -1;
	return g.buf[i];
}

int fini()
{
	free(g.buf);
	return 0;
}
