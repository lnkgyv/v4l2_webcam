#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[]){
  int fbfd = 0;
  struct fb_var_screeninfo vinfo;
  struct fb_fix_screeninfo finfo;
  long int screensize = 0;
  char *fbp = 0;
  int x = 0, y = 0;
  long int location = 0;

  if(argc < 2){
    fprintf(stderr, "Using %s input file\n", argv[0]);
    return EXIT_FAILURE;
  }

  fbfd = open("/dev/fb0", O_RDWR);
  if(fbfd == -1){
    perror("Open fbfd");
    return EXIT_FAILURE;
  }

  printf("FB dev opened successfully!");

  if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1){
    perror("Error reading fixed information");
    return EXIT_FAILURE;
  }

  if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1){
    perror("Error reading variable inforrmation");
    return EXIT_FAILURE;
  }

  printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

  screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

  fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

  if((int)fbp == -1){
    perror("Failed to map framebuffer device to memory");
    return EXIT_FAILURE;
  }

  printf("The framebuffer device was mapped to memory successfully.\n");

  int fin;
  int nr;
  int i=0, j=0, t=0;
  unsigned char rgb[3];
  struct rrggbb{
    unsigned char r;
    unsigned char g;
    unsigned char b;
  } im[307200];

  fin = open(argv[1], O_RDONLY);
  while(t!=307200){
    nr = read(fin, rgb, sizeof(unsigned char) * 3);
    im[t].r = rgb[0];
    im[t].g = rgb[1];
    im[t].b = rgb[2];
    ++t;
  }

  /* put pixel from this position */
  x = 0;
  y = 0;
  t = 0;

  for(y = 0; y < 480; y++)
     for(x = 0; x < 640; x++, t++){
        location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                   (y + vinfo.yoffset) * finfo.line_length;

      if(vinfo.bits_per_pixel == 32){
       *(fbp + location) = im[t].b; /* blue color */
       *(fbp + location + 1) = im[t].g; /* green color */
       *(fbp + location + 2) = im[t].r; /* red color */
       *(fbp + location + 3) = -100; /* transparency */

     } else { //16bpp
       int b = 10;
       int g = (x - 100) / 6; //green
       int r = 31 - (y - 100) / 16;  //red
       unsigned short int t = r < 11 | g << 5 | b;
       *((unsigned short int *)(fbp + location)) = t;
     }
    }

  munmap(fbp, screensize);
  close(fbfd);
  return EXIT_SUCCESS;
}
