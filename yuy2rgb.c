#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

char stage(int st){
  switch(st){
    case 0 ... 100: return '|';
    case 101 ... 200: return '/';
    case 201 ... 300: return '-';
    case 301 ... 400: return '\\';
    case 401 ... 500: return '|';
    case 501 ... 600: return '/';
    case 601 ... 700: return '-';
    case 701 ... 800: return '|';
  }
}

static unsigned char clamp(double x){
  int r = x;

  if(r < 0) return 0;
  else if(r > 255) return 255;
  else return r;
}


static unsigned char *torgb(unsigned char *ycbcr){
  unsigned char *rgb = calloc(6, sizeof(char));
  /* temporaries */
  double r, g, b;
  double y1, pb, pr;

  y1 = (ycbcr[0] - 16) / 219.0; /* y */
  pb = (ycbcr[1] - 128) / 224.0; /* cb */
  pr = (ycbcr[2] - 128) / 224.0; /* cr */

  r = 1.0 * y1 + 0 * pb + 1.402 * pr;
  g = 1.0 * y1 - 0.344 * pb - 0.714 * pr;
  b = 1.0 * y1 + 1.772 * pb + 0 * pr;

  rgb[0] = clamp(r * 255);
  rgb[1] = clamp(g * 255);
  rgb[2] = clamp(b * 255);

  y1 = (ycbcr[3] - 16) / 219.0; /* y */
  pb = (ycbcr[4] - 128) / 224.0; /* cb */
  pr = (ycbcr[5] - 128) / 224.0; /* cr */

  r = 1.0 * y1 + 0 * pb + 1.402 * pr;
  g = 1.0 * y1 - 0.344 * pb - 0.714 * pr;
  b = 1.0 * y1 + 1.772 * pb + 0 * pr;

  rgb[3] = clamp(r * 255);
  rgb[4] = clamp(g * 255);
  rgb[5] = clamp(b * 255);


  return rgb;
}

int main(int argc, char *argv[]){
  int fid, fod;
  int nr; /* readed bytes */
  int wc; /* writed bytes */
  unsigned char yuv422[4]; /* packed raw from file */
  unsigned char ycbcr[6]; /* unpucked raw from file*/
  unsigned char *rgb; /* output rgb */

  if(argc < 3 ){
    fprintf(stderr, "Usage %s <fin-file> <fout-file> \n",argv[0]);
    return EXIT_FAILURE;
  }

  fid = open(argv[1], O_RDONLY);
  if(fid == -1){
    perror("input file");
    return EXIT_FAILURE;
  }

  fod = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0666);
  if(fod == -1){
    perror("output file");
    return EXIT_FAILURE;
  }

  printf("Decoding in progress ");
  int st = 0; /* for view progress */
  while((nr = read(fid, yuv422, sizeof(char) * 4)) != 0){
    if(nr == -1 || nr < 4){
      perror((nr == -1)?"Read input file":"Something wrong while reading input file");
      return EXIT_FAILURE;
    }

    ycbcr[0] = yuv422[0];
    ycbcr[1] = yuv422[1];
    ycbcr[2] = yuv422[3];
    ycbcr[3] = yuv422[2];
    ycbcr[4] = yuv422[1];
    ycbcr[5] = yuv422[3];

    rgb = torgb(ycbcr);
    wc = write(fod, rgb, sizeof(char) * 6);
    if(!wc){
      perror("Maybe something wrong write fod");
      return EXIT_FAILURE;
    }

    printf("\b%c",stage(st));
   (st >= 800) ? st = 0 : ++st;
  }

  printf("\b!\nDecoding complete!\n");

  close(fid);
  close(fod);

  return EXIT_SUCCESS;
}


