#include <stdlib.h>
#include <stdint.h>

typedef struct { uint8_t red, green, blue, alpha; } rgba_pixel;

rgba_pixel * init(int n)
{
	int i=0; 
	rgba_pixel * buf = (rgba_pixel*)malloc(sizeof(rgba_pixel)*n);
	if(buf != NULL) {
		for (i=0; i<n; i++)
			buf[i].red = 128;
	}

	return buf;
}
