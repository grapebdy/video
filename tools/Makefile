all: yuv
yuv: clean
	gcc yuv.c lib.c -o yuv
	gcc grapic.c lib.c rgb-yuv.c -o grappic
	gcc rgb-yuv.c lib.c -o  color_convert
clean:
	@rm -fr yuv grappic rgb color_convert
