#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include "lib.h"

#define DST_FILE_NAME "/mnt/hgfs/workspace/ducks_take_off_yv16_720p50_1frame.yuv"
#define SRC_FILE_NAME "/mnt/hgfs/workspace/ducks_take_off_422_720p50.y4m"
#define TST_FILE_NAME "/mnt/hgfs/workspace/test.yuv"
#define FILE_SIZE (1280 * 720 * 2)

#define UYVY_BLACK/*Y Cr Y Cb*/	0x10801080
#define UYVY_BLUE/*Y Cr Y Cb*/	0x286e28ef
#define UYVY_RED/*Y Cr Y Cb*/	0x51ef515b
#define UYVY_GREEN/*Y Cr Y Cb*/	0x90239036

int grap_yuv_picture(unsigned char *pic_src, unsigned char *pic_dst,
		int src_rows, int src_column, int dst_rows, int dst_column)
{
	int i, j;
	int stride = (src_column - dst_column) * 2;
	int src_index = 0;
	int dst_index = 0;

	for (i = 0; i < dst_rows; i++) {
		for (j = 0; j < 2 * dst_column; j++)
			pic_dst[dst_index++] = pic_src[src_index++];
		src_index += stride;
	}
	return dst_index;

}

void generate_data1(unsigned char *data, int column, int row)
{
	int i;
	unsigned int *buff = (unsigned int *)data;
	for (i = 0; i < (column * row * 2) / 4; i++)
		buff[i] = UYVY_GREEN;
}

void generate_data(unsigned char *data, int column, int row, int y, int cb, int cr)
{
	int i, j;
	for (i = 0; i < column * row * 2; i += 4) {
		data[i + 0] = cb;
		data[i + 1] = y;
		data[i + 2] = cr;
		data[i + 3] = y;
	}
}

void generate_plannar_data(unsigned char *data, int column, int row, int y, int cb, int cr)
{
	int i, j;
	for (i = 0; i < column * row; i++) {
		data[i] = y;
	}

	for (; i < column * row * 2; i += 2) {
		data[i] = cb;
		data[i + 1] = cr;
	}
}

int data_dump(unsigned char *data, int size)
{
        int i;
        for (i = 0; i < size; i++) {
                printf("%.2x%c", data[i], i % 16 == 15?'\n':' ');
        }
        printf("\n");
}

int get_yv16_data(unsigned char *src, unsigned char *ybuf,
		unsigned char *ubuf, unsigned char *vbuf, int column, int row)
{
	memcpy(ybuf, src, column * row);
	memcpy(vbuf, src + column * row, column * row / 2);
	memcpy(ubuf, src + column * row / 2 * 3, column * row / 2);
	return 0;
}

int yv16touyvy(unsigned char *dst, unsigned char *ybuf,
		unsigned char *ubuf, unsigned char *vbuf, int column, int row)
{
	int i, j;

	for (i = 0, j = 0; i < column * row / 2; i++, j+=4)
		dst[j] = ubuf[i];

	for (i = 0, j = 1; i < column * row; i++, j+=2)
		dst[j] = ybuf[i];

	for (i = 0, j = 2; i < column * row / 2; i++, j+=4)
		dst[j] = vbuf[i];

	return 0;
}

void rgb2ycbcr(unsigned char r,unsigned char g,unsigned char b)
{
	unsigned char ycbcr_res[3];
	ycbcr_res[0]= (unsigned char)(r * 0.256789 + g * 0.504129 + b * 0.097906)+ 16;
	ycbcr_res[1]= (unsigned char)(r *-0.148223 + g * -0.290992 + b * 0.439215)+ 128;
	ycbcr_res[2]= (unsigned char)(r * 0.439215 + g * -0.367789 + b *-0.071426)+ 128;
	printf("y:%x, cb:%x, cr:%x \n", ycbcr_res[0], ycbcr_res[1], ycbcr_res[2]);
}

unsigned char ybuff[FILE_SIZE];
unsigned char ubuff[FILE_SIZE];
unsigned char vbuff[FILE_SIZE];
unsigned char pic_src[FILE_SIZE];
unsigned char pic_dst[FILE_SIZE];

int main(int argc, char *argv[])
{
	int byte;
	int opt, y, u, v, r, g, b;
        unsigned char data[FILE_SIZE];
	while ((opt = getopt(argc,argv,"fscktey:u:v:r:g:b:")) != -1) {
		switch (opt) {
		case 'e':
			read_file(DST_FILE_NAME, pic_src, 0, FILE_SIZE);
			byte = grap_yuv_picture(pic_src, pic_dst, 720, 1280, 480, 720);
			save_file(TST_FILE_NAME, pic_dst, 0, byte);
			break;
		case 't':
			read_file(DST_FILE_NAME, data, 0, FILE_SIZE);
			get_yv16_data(data, ybuff, ubuff, vbuff, 1280, 720);
			yv16touyvy(data, ybuff, ubuff, vbuff, 1280, 720);
			save_file(TST_FILE_NAME, data, 0, FILE_SIZE);
			break;
		case 'f':
        		read_file(SRC_FILE_NAME, data, 46, FILE_SIZE);
        		save_file(DST_FILE_NAME, data, 0, FILE_SIZE);
        		data_dump(data, 1024);
			break;
		case 'k':
			generate_data1(data, 1280, 720);
        		save_file(TST_FILE_NAME, data, 0, FILE_SIZE);
			break;
		case 'd':
			generate_data(data, 1280, 720, y, u, v);
        		save_file(TST_FILE_NAME, data, 0, FILE_SIZE);
			return 0;
		case 'y':
			y = atoi(optarg);
			break;
		case 'u':
			u = atoi(optarg);
			break;
		case 'v':
			v = atoi(optarg);
			break;
		case 'r':
			r = atoi(optarg);
			break;
		case 'g':
			g = atoi(optarg);
			break;
		case 'b':
			b = atoi(optarg);
			break;
		case 's':
			generate_plannar_data(data, 1280, 720, y, u, v);
        		save_file(TST_FILE_NAME, data, 0, FILE_SIZE);
			break;
		case 'c':
			rgb2ycbcr(r, g, b);
			break;
		default:
			printf("unsupport data format \n");
		}
	}
        return 0;
}
