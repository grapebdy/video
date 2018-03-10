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
