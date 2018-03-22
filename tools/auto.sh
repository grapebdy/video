#/bin/sh

if [ $# -ne 2 ]
then
	echo "run.sh src.y4m dst.yuv"
	exit
fi
filename=$(basename $1 .y4m)

./y4m2yuv -s $1 -o $2 -w 1280 -h 720  -f 100

./grappic.exe -s $2 -w 1280 -h 720 -o $filename-640x480.yuv -x 640 -y 480 -m 0 -n 0
