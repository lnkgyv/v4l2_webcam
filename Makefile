objs = main.c

jpeglib = jpeg

vcapt : $(objs)
	cc -o vcapt $(objs)

yuy2jpeg : $(jobjs)
	cc -o yuy2jpeg -l jpeg $(jobjs)

yuy2rgb : yuy2rgb.c
	cc -o yuy2rgb yuy2rgb.c

fbdraw: fbdraw.c
	cc -o fbdraw fbdraw.c

draw_image: draw_image.c
	cc -g -o draw_image draw_image.c -lX11
