#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include "lib.h"
struct pixel {
	int rc;
	int gc;
	int bc;
	int y;
	int cb;
	int cr;
};

void yuv2rgb_601(struct pixel *p)
{
	p->rc = 1.164 * (p->y - 16) + 1.596 * (p->cr - 128);
	p->gc = 1.164 * (p->y - 16) - 0.392 * (p->cb - 128) - 0.813 * (p->cr - 128);
	p->bc = 1.164 * (p->y - 16) + 2.017 * (p->cb - 128);
}

void rgb2yuv_601(struct pixel *p)
{
	int rc = p->rc;
	int gc = p->gc;
	int bc = p->bc;

	p->y  = 0.257  * rc + 0.564 * gc + 0.098 * bc + 16;
	p->cb = -0.148 * rc - 0.291 * gc + 0.439 * bc + 128;
	p->cr = 0.439  * rc - 0.368 * gc - 0.071 * bc + 128;
	
	if (p->y < 0)
		p->y = 0;
	if (p->cb < 0)
		p->cb = 0;
	if (p->cr < 0)
		p->cr = 0;
	p->y &= 0xff;
	p->cb &= 0xff;
	p->cr &= 0xff;
}

void yuv2rgb_709(struct pixel *p)
{
}

void rgb2yuv_709(struct pixel *p)
{
	int rc = p->rc;
	int gc = p->gc;
	int bc = p->bc;

	p->y  = 0.2126  * rc + 0.7154 * gc + 0.072 * bc;
	p->cb = -0.1145 * rc - 0.3855 * gc + 0.5 * bc + 128;
	p->cr = 0.5     * rc - 0.4543 * gc - 0.045 * bc + 128;
	
	if (p->y < 0)
		p->y = 0;
	if (p->cb < 0)
		p->cb = 0;
	if (p->cr < 0)
		p->cr = 0;
	p->y &= 0xff;
	p->cb &= 0xff;
	p->cr &= 0xff;
}


void yuv2rgb_2020(struct pixel *p)
{
	p->rc = p->y + 1.4746 * (p->cr - 128);
	p->gc = p->y - 0.1646 * (p->cb - 128) - 0.5714 * (p->cr - 128);
	p->bc = p->y + 1.8814 * (p->cb - 128);
}

void rgb2yuv_2020(struct pixel *p)
{
	int rc = p->rc;
	int gc = p->gc;
	int bc = p->bc;

	p->y  = 0.2627 * rc + 0.678 * gc + 0.0593 * bc;
	p->cb = 0.5315 * (bc - p->y) + 128;
	p->cr = 0.6781 * (rc - p->y) + 128;
	
	if (p->y < 0)
		p->y = 0;
	if (p->cb < 0)
		p->cb = 0;
	if (p->cr < 0)
		p->cr = 0;
	p->y &= 0xff;
	p->cb &= 0xff;
	p->cr &= 0xff;
}

