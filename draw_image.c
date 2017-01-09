#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* determine and print visual class */
void visual_mode(Visual *visual){
  char *class;

  switch (visual->class){
    case PseudoColor: class = "PseudoColor"; break;
    case StaticColor: class = "StaticColor"; break;
    case GrayScale:   class = "GrayScale";   break;
    case StaticGray:  class = "StaticGray";  break;
    case DirectColor: class = "DirectColor"; break;
    case TrueColor:   class = "TrueColor";   break;
  }

  printf("Visual class: %s\n", class);
}

int main(int argc, char *argv[])
{
  Display *display;
  GC gc;
  Window window;
  Visual *visual;
  XImage *img;
  XEvent event;
  int screen;
  int depth;
  int fin;
  int width, heigth;

  if(argc < 4){
    fprintf(stderr, "Usage %s <width> <height> <image>\n",argv[0]);
    return EXIT_FAILURE;
  }

  width  = atoi(argv[1]);
  heigth = atoi(argv[2]);

  /* some safety */
  (width  == 0 || width  < 320) ? width  = 640 : width;
  (heigth == 0 || heigth < 240) ? heigth = 480 : heigth;

  if((display = XOpenDisplay(NULL)) == NULL){
    puts("Can't connect to X server!\n");
    exit(1);
  }

  screen = DefaultScreen(display);

  window = XCreateSimpleWindow(display, RootWindow(display, 0), 0, 0, width, heigth, 1, 0, 0);

  /* KeyPress for 'EXIT' */
  XSelectInput(display, window, ExposureMask | KeyPressMask);

  XMapWindow(display, window);

  depth = DefaultDepth(display, screen);

  visual = DefaultVisual(display, screen);

  visual_mode(visual);

  printf("Depth: %d\n",depth);

  fin = open(argv[3], O_RDONLY);
  if(fin == -1){
    perror("open fin");
    return EXIT_FAILURE;
  }

  printf("Width x Heigth: %d x %d\n", width, heigth);
  printf("Filename: %s\n", argv[3]);

    unsigned char *im[100];
  if(depth >= 24){
    int i = 0, j = 0;
    int nr;
    int frame = width * heigth * 4;
    // = malloc(width * heigth * 4);
    unsigned char rgb[3];

    im[0] = malloc(width * heigth * 4);
    while((nr = read(fin, rgb, sizeof(char) *3))!=0){
      im[j][i] = rgb[2]; /* b */
      ++i;
      im[j][i] = rgb[1]; /* g */
      ++i;
      im[j][i] = rgb[0]; /* r */
      ++i;
      im[j][i] = 0;      /* alpha channel */
      ++i;

      if(i == frame){
        i = 0;
        ++j;
        im[j] = malloc(width * heigth * 4);
      }
    }

    printf("Readed bytes: %d\n",nr);
  }

#include <unistd.h>

  while(1){
       XNextEvent(display, &event);

       if(event.type == Expose){
         int i = 0;
         unsigned long ms = 1000000;
         unsigned long us = ms / 20;
         while(i < 100){
           img = XCreateImage(display, visual, depth, ZPixmap, 0, (char *)(im[i]), width, heigth, 32, 0);
           XInitImage(img);
           XPutImage(display, window, DefaultGC(display, screen), img, 0, 0, 0, 0, width, heigth);
           usleep(us);
           ++i;
         }
       }

      if(event.type == KeyPress)
        break;
  }

  free(img);
  XCloseDisplay(display);
}
