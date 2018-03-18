#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>


#include "lib.h"

#define Y4M_HEAD_SIZE	0x28
#define Y4M_FRAME_SIZE	0x06
struct y4m_struct {
	int width;
	int higth;
	int frame;
	int inter;
	int colorsp;
};

int get_frame_header(char *filename, struct y4m_struct *y4m)
{
	int ret;
	char head[100];
	ret = read_file(filename, head, 0 , 46);
	if (ret < 0) {
		printf("read %s failed\n", filename);
		return -1;
	}

	return 0;
}

int split_yuv_frame(unsigned char *src, unsigned char *ybuf,
		unsigned char *ubuf, unsigned char *vbuf, int column, int row)
{
	memcpy(ybuf, src, column * row);
	memcpy(vbuf, src + column * row, column * row / 2);
	memcpy(ubuf, src + column * row / 2 * 3, column * row / 2);
	return 0;
}

int convert_yv16_uyvy(unsigned char *dst, unsigned char *ybuf,
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

#define BYTES_YUV422_PIXEL	0x2
#define BYTES_RGB888_PIXEL	0x4

void do_usage(void)
{
	printf("y4m2yuv: \n");
	printf("\t -b num \t bytes per pixel\n");
	printf("\t -s file\t source file, length < 100\n");
	printf("\t -o file\t destination file, length < 100\n");
	printf("\t -w num\t  width of src image, default 1280\n");
	printf("\t -h num\t  higth of src image, default 720\n");
	printf("\t -f num\t  convert frame num to yuv, default 0\n");
	printf("\t -y num\t  higth of src image, default 480\n");
}

int main(int argc, char *argv[])
{
	int opt;
	int ret = 0;
	int src_rows = 720;
	int src_column = 1280;
	int frame = 0;
	int bytes_per_pixel = BYTES_YUV422_PIXEL;
	char file_src[100];
	char file_dst[100];
	unsigned char *pic_src = NULL;
	unsigned char *pic_dst = NULL;
	unsigned char *pic_y = NULL;
	unsigned char *pic_u = NULL;
	unsigned char *pic_v = NULL;
	int file_lock = 0;
	int bytes_per_frame = 0;

	while ((opt = getopt(argc,argv,"b:s:o:w:h:f:")) != -1) {
		switch (opt) {

		case 'b':
			bytes_per_pixel = atoi(optarg);
			break;

		case 'f':
			frame = atoi(optarg);
			break;

		case 's':
			file_lock++;
			if (strlen(optarg) + 1 > 100) {
				printf("src file name length > 100\n");
				return -1;
			}

			memcpy(file_src, optarg, strlen(optarg) + 1);
			break;

		case 'o':
			file_lock++;
			if (strlen(optarg) + 1 > 100) {
				printf("dest file name length > 100\n");
				return -1;
			}
			memcpy(file_dst, optarg, strlen(optarg) + 1);
			break;

		case 'w':
			src_column = atoi(optarg);
			break;

		case 'h':
			src_rows = atoi(optarg);
			break;

		default:
			printf("option is not supported\n");
			return -1;
		}
	}

	if (file_lock < 2) {
		do_usage();
		return -1;
	}

	printf("convert frame %d from %s(%dx%d, %dB) to %s\n", frame, file_src,
			src_column, src_rows, get_file_size(file_src), file_dst);

	bytes_per_frame = src_column * src_rows * bytes_per_pixel;
	pic_src = malloc(bytes_per_frame);
	if (!pic_src) {
		printf("alloc memory failed\n");
		ret = -1;
		goto pic_out;
	}

	pic_dst = malloc(bytes_per_frame);
	if (!pic_dst) {
		printf("alloc memory failed\n");
		ret = -1;
		goto pic_out;
	}

	pic_y = malloc(bytes_per_frame);
	if (!pic_y) {
		printf("alloc memory failed\n");
		ret = -1;
		goto pic_out;
	}
	pic_u = malloc(bytes_per_frame);
	if (!pic_u) {
		printf("alloc memory failed\n");
		ret = -1;
		goto pic_out;
	}

	pic_v = malloc(bytes_per_frame);
	if (!pic_v) {
		printf("alloc memory failed\n");
		ret = -1;
		goto pic_out;
	}

	ret = read_file(file_src, pic_src,
		Y4M_HEAD_SIZE + frame * (bytes_per_frame + Y4M_FRAME_SIZE) + Y4M_FRAME_SIZE,
		bytes_per_frame);
	if (ret < 0) {
		printf("read %s failed\n", file_src);
		ret = -1;
		goto pic_out;
	}

	split_yuv_frame(pic_src, pic_y, pic_u, pic_v, src_column, src_rows);
	convert_yv16_uyvy(pic_dst, pic_y, pic_v, pic_u, src_column, src_rows);

	ret = save_file(file_dst, pic_dst, 0, bytes_per_frame);
	if (ret < 0) {
		ret = -1;
		printf("write %s failed\n", file_src);
	}

pic_out:
	if (pic_y)
		free(pic_y);
	if (pic_u)
		free(pic_u);
	if (pic_v)
		free(pic_v);
	if (pic_src)
		free(pic_src);
	if (pic_dst)
		free(pic_dst);

	return ret;
}
