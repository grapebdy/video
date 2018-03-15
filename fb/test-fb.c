#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>

#define GPT_FB_DEV	"/dev/fb0"

int gpt_fb_draw_copy(struct fb_var_screeninfo *vinfo, struct fb_fix_screeninfo *finfo, unsigned char *fb_base,
			unsigned char *buff, unsigned int xres, unsigned int yres)
{
	int row, column;
	int pixel = 0;
	unsigned int location;

	printf("xres: %d, yres: %d\n", xres, yres);

	for (row = 0; row  < yres; row++)
		for (column = 0; column < xres; column++, pixel++) {
			location = (vinfo->xoffset + column) * (vinfo->bits_per_pixel / 8) +
				(vinfo->yoffset + row) * finfo->line_length;
			if (vinfo->bits_per_pixel == 32) {
				*(fb_base + location + 0) = *(buff + pixel * 4 + 0);
				*(fb_base + location + 1) = *(buff + pixel * 4 + 1);
				*(fb_base + location + 2) = *(buff + pixel * 4 + 2);
				*(fb_base + location + 3) = *(buff + pixel * 4 + 3);
			} else {
				*(fb_base + location + 0) = *(buff + pixel * 2 + 0);
				*(fb_base + location + 1) = *(buff + pixel * 2 + 1);
			}
		}

}
unsigned char buff[0x100000];

int generate_data(int type, unsigned char *buf)
{
	int i = 0;

	int r;
	int g;
	int b;

	switch (type) {
		for (i = 0; i < 0x4000; i+=4) {
			buf[i + 0] = 10;
			buf[i + 1] = 100;
			buf[i + 2] = 100;
			buf[i + 3] = 0;
		}
		break;
	case 1:
		r = 0xff & 0x1f;
		g = 0x00  & 0x3f;
		b = 0x00 & 0x1f;
		for (i = 0; i < 0x80000; i++) {
			((unsigned short *)buf)[i] = r << 11 | g << 5 | b;
		}
		break;

	case 2:
		r = 0x00 & 0x1f;
		g = 0xff  & 0x3f;
		b = 0x00 & 0x1f;
		for (i = 0; i < 0x80000; i++) {
			((unsigned short *)buf)[i] = r << 11 | g << 5 | b;
		}
		break;

	case 3:
		r = 0x00 & 0x1f;
		g = 0x00  & 0x3f;
		b = 0xff & 0x1f;
		for (i = 0; i < 0x80000; i++) {
			((unsigned short *)buf)[i] = r << 11 | g << 5 | b;
		}
		break;

	case 4:
		r = 0x00 & 0x1f;
		g = 0xff  & 0x3f;
		b = 0xff & 0x1f;
		for (i = 0; i < 0x80000; i++) {
			((unsigned short *)buf)[i] = r << 11 | g << 5 | b;
		}
		break;

	}
	return 0;
}

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

	generate_data(1, buff);
	vinfo.xoffset = 0;
	vinfo.yoffset = 0;

	gpt_fb_draw_copy(&vinfo, &finfo, fb_base, buff, 420, 300);

	vinfo.xoffset = 500;
	vinfo.yoffset = 0;
	gpt_fb_draw_copy(&vinfo, &finfo, fb_base, buff, 420, 300);

	vinfo.xoffset = 1000;
	vinfo.yoffset = 0;
	gpt_fb_draw_copy(&vinfo, &finfo, fb_base, buff, 420, 300);

	generate_data(2, buff);
	vinfo.xoffset = 0;
	vinfo.yoffset = 310;

	gpt_fb_draw_copy(&vinfo, &finfo, fb_base, buff, 420, 300);

	vinfo.xoffset = 500;
	vinfo.yoffset = 310;
	gpt_fb_draw_copy(&vinfo, &finfo, fb_base, buff, 420, 300);

	vinfo.xoffset = 1000;
	vinfo.yoffset = 310;
	gpt_fb_draw_copy(&vinfo, &finfo, fb_base, buff, 420, 300);


	generate_data(3, buff);
	vinfo.xoffset = 0;
	vinfo.yoffset = 620;

	gpt_fb_draw_copy(&vinfo, &finfo, fb_base, buff, 420, 300);

	vinfo.xoffset = 500;
	vinfo.yoffset = 620;
	gpt_fb_draw_copy(&vinfo, &finfo, fb_base, buff, 420, 300);

	vinfo.xoffset = 1000;
	vinfo.yoffset = 620;
	gpt_fb_draw_copy(&vinfo, &finfo, fb_base, buff, 420, 300);


	munmap(fb_base, screen_size);
	close(fbfd);
	return 0;
}
