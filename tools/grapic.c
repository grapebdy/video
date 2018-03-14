#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include "lib.h"


#define BYTES_YUV422_PIXEL	0x2
#define BYTES_RGB888_PIXEL	0x4

int grap_raw_picture(unsigned char *pic_src, unsigned char *pic_dst,
		int bytes_per_pixel, int src_rows, int src_column,
		int dst_rows, int dst_column)
{
	int i, j;
	int stride = (src_column - dst_column) * bytes_per_pixel;
	int src_index = 0;
	int dst_index = 0;
	if (src_column * src_rows < dst_column * dst_rows)
		return -1;


	for (i = 0; i < dst_rows; i++) {
		for (j = 0; j < bytes_per_pixel * dst_column; j++)
			pic_dst[dst_index++] = pic_src[src_index++];
		src_index += stride;
	}
	return dst_index;

}

void do_usage(void)
{
	printf("grapic: \n");
	printf("\t -b num \t bytes per pixel\n");
	printf("\t -s file\t source file, length < 100\n");
	printf("\t -o file\t destination file, length < 100\n");
	printf("\t -w num\t  width of src image, default 1280\n");
	printf("\t -h num\t  higth of src image, default 720\n");
	printf("\t -x num\t  width of src image, default 720\n");
	printf("\t -y num\t  higth of src image, default 480\n");
}

int main(int argc, char *argv[])
{
	int opt;
	int ret = 0;
	int src_rows = 720;
	int src_column = 1280;
	int dst_rows = 480;
	int dst_column = 720;
	int bytes_per_pixel = BYTES_YUV422_PIXEL;
	char file_src[100];
	char file_dst[100];
	unsigned char *pic_src = NULL;
	unsigned char *pic_dst = NULL;
	int file_lock = 0;

	while ((opt = getopt(argc,argv,"b:s:o:w:h:x:y:")) != -1) {
		switch (opt) {

		case 'b':
			bytes_per_pixel = atoi(optarg);
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
		case 'x':
			dst_column = atoi(optarg);
			break;
		case 'y':
			dst_rows = atoi(optarg);
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

	printf("from %s(%dx%d) to %s(%dx%d) %d bytes per pixel\n", file_src,
			src_column, src_rows, file_dst, dst_column,
			dst_rows, bytes_per_pixel);

	pic_src = malloc(src_column * src_rows * bytes_per_pixel);
	if (!pic_src) {
		printf("alloc memory failed\n");
		ret = -1;
		goto pic_out;
	}

	pic_dst = malloc(dst_column * dst_rows * bytes_per_pixel);
	if (!pic_dst) {
		printf("alloc memory failed\n");
		ret = -1;
		goto pic_out;
	}

	ret = read_file(file_src, pic_src, 0, src_column * src_rows * bytes_per_pixel);
	if (ret < 0) {
		printf("read %s failed\n", file_src);
		goto pic_out;
	}
#if 0
	ret = grap_raw_picture(pic_src, pic_dst, bytes_per_pixel,
			src_rows, src_column, dst_rows, dst_column);
	if (ret < 0) {
		printf("grap picture failed, check src len >= dst len\n");
		goto pic_out;
	}
#endif
	ret = save_file(file_dst, pic_dst, 0, ret);
	if (ret < 0) {
		printf("write %s failed\n", file_src);
	}

pic_out:
	if (pic_src)
		free(pic_src);
	if (pic_dst)
		free(pic_dst);

	return ret;
}
