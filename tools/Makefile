all: yuv
yuv: clean
	gcc yuv.c lib.c -o yuv
	gcc grapic.c lib.c  -o grappic
	gcc rgb-yuv.c lib.c -o  color-convert
	gcc y4m-yuv.c lib.c -o  y4m2yuv
clean:
	@rm -fr yuv grappic rgb color-convert y4m2yuv
