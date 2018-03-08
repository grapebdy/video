#include "lib.h"

void show_msg(unsigned char *src, int length)
{
	int i;
	for (i = 0; i < length; i++)
                printf("%0.2x%c", src[i], i%16 == 15?'\n':' ');
	printf("\n");
}

int get_file_size(char* filename)
{
	struct stat file_st;
	stat(filename,&file_st);
	return file_st.st_size;
}

int read_file(char* filename, void *buff, int offset, int size)
{
	int fd = -1;
	int rd_num = -1;
	fd = open(filename, O_RDONLY);
	if (fd == -1) {
		printf("open file error\n");
		return -1;
	}
	lseek(fd, offset, SEEK_SET);
	rd_num = read(fd,buff,size);
	close(fd);

	return rd_num;
}

int save_file(char* filename, void *buff, int offset, int size)
{
	int fd = -1;
	int wt_num = -1;
	fd = open(filename, O_RDWR|O_CREAT|O_TRUNC|O_SYNC, S_IRWXU);
	if (fd == -1) {
		printf("open file error\n");
		return -1;
	}
	lseek(fd, offset, SEEK_SET);
	wt_num = write(fd,buff,size);
	close(fd);
	return wt_num;
}


#if 0
void dump_data(unsigned char *data, int length)
{
	int i;
	for (i = 0; i < length; i++)
		printf("%x%c", data[i], i % 16 == 15?'\n':' ');

	printf("\n");
}
#define VIDEO_SIZE	(640 * 480 * 2)
int main()
{
	unsigned char data[VIDEO_SIZE];
	
	
	generate_data(data, 640, 480, 244, 127, 127);
	dump_data(data, 96);
	save_file("/tmp/xxx.yuv", data, 0, VIDEO_SIZE);
	return 0;
}
#endif
