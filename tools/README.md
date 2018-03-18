1) grappic
	grap image from another raw image
	for example, grap a dstimage(640x480)  from srcimage(720x480), bytes per pixel is 2byte
	grappic -s srcimage -o dtsimage -b 2 -w720 -h480 -x640 -y480

2) color-convert
	convert color space from rgb to yuv, or from yuv to rgb

3) y4m2yuv
	convert y4m file to uyvy.yuv, y4m header is follow
	y4m2yuv -s file.y4m -o dst.yuv -w 1280 -h 720 -b 2
