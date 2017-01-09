#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <sys/stat.h>


/*
   YUY422 or YUV4:2:2 or YUYV is the same

*/

static void yuv422_to_jpeg(FILE *fout, unsigned char *image, int width, int height, int quality){
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  int i;

  /* some safety */

  ((width < 320)   || (width < 0))                   ? width   = 320 : width;
  ((height < 240)  || (height < 0))                  ? height  = 240 : height;
  ((quality > 100) || (!quality) || (quality < 0))   ? quality = 20  : quality;

  unsigned char *jrow[width];
  unsigned char *buf = calloc(sizeof(char), width * 3);

  jpeg_create_compress(&cinfo);

  cinfo.err = jpeg_std_error(&jerr);
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = 3;
  cinfo.in_color_space = JCS_YCbCr;

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 92, TRUE);
  jpeg_stdio_dest(&cinfo, fout);
  jpeg_start_compress(&cinfo, TRUE);

  while(cinfo.next_scanline < height){
    for(i = 0; i < cinfo.image_width; i += 2){
      buf[i*3] = image[i*2];
      buf[i*3+1] = image[i*2+1];
      buf[i*3+2] = image[i*2+3];
      buf[i*3+3] = image[i*2+2];
      buf[i*3+4] = image[i*2+1];
      buf[i*3+5] = image[i*2+3];
    }
    jrow[0] = buf;
    image += width * 2;
    jpeg_write_scanlines(&cinfo, jrow, 1);
  }

  jpeg_finish_compress(&cinfo);
  free(buf);
}

int main(int argc, char *argv[]){
  FILE *fin, *fout;
  int i=0;
  char *image;

  struct stat fin_stat; /* determine size of input file */

  if(argc < 5 ){
    fprintf(stderr, "Usage %s <fin-file> <fout-file> <width> <height> <quality>\n",argv[0]);
    printf("<fin-file>  - input file.\n");
    printf("<fout-file> - output file.\n");
    printf("<width>     - width of input file.\n");
    printf("<height>    - height of input file.\n");
    printf("<quality>   - quality of output image [1..100].\n");
    printf("Example: %s image image.jpeg 640 480 90\n",argv[0]);
    return EXIT_FAILURE;
  }

  if(stat(argv[1], &fin_stat) == -1){
    perror("stat");
    return EXIT_FAILURE;
  }

  image = calloc(sizeof(char), fin_stat.st_size);

  fin  = fopen(argv[1], "r");
  fout = fopen(argv[2], "w");

  while((image[i++] = fgetc(fin)) != EOF);


  yuv422_to_jpeg(fout, image, atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));

  fclose(fin);
  fclose(fout);

  return EXIT_SUCCESS;
}
