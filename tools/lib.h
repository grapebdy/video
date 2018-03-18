#ifndef __LIB_H__
#define __LIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int generate_random(unsigned char *src, int length);
void show_msg(unsigned char *src, int length);
int save_file(char* filename, void *buff, int offset, int size);
int read_file(char* filename, void *buff, int offset, int size);
int get_file_size(char* filename);
#endif
