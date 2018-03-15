#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>

#define GPT_FB_DEV	"/dev/fb0"

int main()
{
	int fbfd = -1;
	int screen_size;

	unsigned char *fb_base;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	
	fbfd = open(GPT_FB_DEV, O_RDWR);
	if (fbfd < 0) {
		printf("/dev/fb0 is not exist\n");
		return -1;
	}

	ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);
	printf("fb id: %d\n", finfo.id);
	printf("fix--> smem_start: 0x%lx, smem_len: %d, fb_type: 0x%x, line_length: %d\n",
			finfo.smem_start, finfo.smem_len, finfo.type, finfo.line_length);

	ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
	printf("var--> xres: %d, yres: %d, xoff: %d, yoff: %d, xvirt: %d, yvirt: %d, bit_per_pixel: %d\n",
			vinfo.xres, vinfo.yres, vinfo.xoffset, vinfo.yoffset, vinfo.xres_virtual,
			vinfo.yres_virtual, vinfo.bits_per_pixel);
	screen_size = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
	printf("screen size %d\n", screen_size);

	fb_base = (void *)mmap(0, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if ((int )fb_base == -1) {
		printf("map failed\n");
		close(fbfd);	
		return -1;
	}
	memset(fb_base, 0xfe51fe5a, screen_size);

	munmap(fb_base, screen_size);
	close(fbfd);
	return 0;
}
