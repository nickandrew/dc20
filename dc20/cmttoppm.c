/*
 *
 * Converts CMT file of Chinon ES-1000 or IMG file of LXDC to PPM file.
 *
 *	written by YOSHIDA Hideki <hideki@yk.rim.or.jp>
 *	In public domain; you can do whatever you want to this program
 *	as long as you admit that the original code is written by me.
 *
 * $Id: cmttoppm.c,v 1.1 1998/12/12 20:32:33 malcolm Exp $
 *
 */

#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define COLUMNS 512
#define LINES   243
#define LEFT_MARGIN 2
#define RIGHT_MARGIN 10
#define TOP_MARGIN 1
#define BOTTOM_MARGIN 1
#define NET_COLUMNS (COLUMNS - LEFT_MARGIN - RIGHT_MARGIN)
#define NET_LINES   (LINES - TOP_MARGIN - BOTTOM_MARGIN)
#define NET_PIXELS  (NET_COLUMNS * NET_LINES)

#define MAGIC "COMET"
#define CMT_HEADER_SIZE 128
#define IMG_THUMBNAIL_SIZE 5120
#define CAMERA_HEADER_SIZE 512

#define SCALE 64
#define SMAX (256 * SCALE - 1)
#define HORIZONTAL_INTERPOLATIONS 3
#define HISTOGRAM_STEPS 4096

#define RFACTOR 0.64
#define GFACTOR 0.58
#define BFACTOR 1.00
#define RINTENSITY 0.476
#define GINTENSITY 0.299
#define BINTENSITY 0.175

#define SATURATION 1.5
#define NORM_PERCENTAGE 3
#define GAMMA 0.5

const char *progname;
int verbose_flag = 0;
int low_i = -1, high_i = -1;
float gamma_value = GAMMA;
float saturation = SATURATION;
float rfactor = RFACTOR, gfactor = GFACTOR, bfactor = BFACTOR;
int norm_percentage = NORM_PERCENTAGE;

void read_cmt_file(int infd, unsigned char ccd[LINES][COLUMNS])
{
  int cmt_format_p;
  char magic[sizeof(MAGIC)];
  read(infd, magic, sizeof(magic));
  cmt_format_p = !strncmp(magic, MAGIC, sizeof(MAGIC));
  if (!cmt_format_p) {
    fprintf(stderr, "%s: not in CMT format; IMG format assumed\n", progname);
  }
  lseek(infd,
	cmt_format_p ? CMT_HEADER_SIZE +    CAMERA_HEADER_SIZE
	             : IMG_THUMBNAIL_SIZE + CAMERA_HEADER_SIZE,
	SEEK_SET);
  read(infd, ccd, COLUMNS * LINES);
}

void set_initial_interpolation(const unsigned char ccd[LINES][COLUMNS],
			       short horizontal_interpolation[LINES][COLUMNS])
{
  int column, line;
  for (line = 0; line < LINES; line++) {
    horizontal_interpolation[line][LEFT_MARGIN] =
      ccd[line][LEFT_MARGIN + 1] * SCALE;
    horizontal_interpolation[line][COLUMNS - RIGHT_MARGIN - 1] =
      ccd[line][COLUMNS - RIGHT_MARGIN - 2] * SCALE;
    for (column = LEFT_MARGIN + 1; column < COLUMNS - RIGHT_MARGIN - 1;
	 column++) {
      horizontal_interpolation[line][column] =
	(ccd[line][column - 1] + ccd[line][column + 1]) * (SCALE / 2);
    }
  }
}

void interpolate_horizontally(const unsigned char ccd[LINES][COLUMNS],
			      short horizontal_interpolation[LINES][COLUMNS])
{
  int column, line, i, initial_column;
  for (line = TOP_MARGIN - 1; line < LINES - BOTTOM_MARGIN + 1; line++) {
    for (i = 0; i < HORIZONTAL_INTERPOLATIONS; i++) {
      for (initial_column = LEFT_MARGIN + 1; initial_column <= LEFT_MARGIN + 2;
	   initial_column++) {
	for (column = initial_column; column < COLUMNS - RIGHT_MARGIN - 1;
	     column += 2) {
	  horizontal_interpolation[line][column] =
	    ((float)ccd[line][column - 1] /
	     horizontal_interpolation[line][column - 1] +
	     (float)ccd[line][column + 1] /
	     horizontal_interpolation[line][column + 1]) *
	       ccd[line][column] * (SCALE * SCALE / 2) + 0.5;
	}
      }
    }
  }
}

void
interpolate_vertically(const unsigned char ccd[LINES][COLUMNS],
		       const short horizontal_interpolation[LINES][COLUMNS],
		       short red[LINES][COLUMNS], short green[LINES][COLUMNS],
		       short blue[LINES][COLUMNS])
{
  int column, line;
  for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++) {
    for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++) {
      int r2gb, g2b, rg2, rgb2, r, g, b;
      int this_ccd = ccd[line][column] * SCALE;
      int up_ccd   = ccd[line - 1][column] * SCALE;
      int down_ccd = ccd[line + 1][column] * SCALE;
      int this_horizontal_interpolation =
	horizontal_interpolation[line][column];
      int this_intensity = this_ccd + this_horizontal_interpolation;
      int up_intensity = horizontal_interpolation[line - 1][column] + up_ccd;
      int down_intensity = horizontal_interpolation[line+1][column] + down_ccd;
      int this_vertical_interpolation;
      if (line == TOP_MARGIN) {
	this_vertical_interpolation =
	  (float)down_ccd / down_intensity * this_intensity + 0.5;
      } else if (line == LINES - BOTTOM_MARGIN - 1) {
	this_vertical_interpolation =
	  (float)up_ccd / up_intensity * this_intensity + 0.5;
      } else {
	this_vertical_interpolation =
	  ((float)up_ccd / up_intensity + (float)down_ccd / down_intensity) *
	    this_intensity / 2.0 + 0.5;
      }
      if (line & 1) {
	if (column & 1) {
	  r2gb = this_ccd;
	  g2b = this_horizontal_interpolation;
	  rg2 = this_vertical_interpolation;
	  r = (2 * (r2gb - g2b) + rg2) / 5;
	  g = (rg2 - r) / 2;
	  b = g2b - 2 * g;
	} else {
	  g2b = this_ccd;
	  r2gb = this_horizontal_interpolation;
	  rgb2 = this_vertical_interpolation;
	  r = (3 * r2gb - g2b - rgb2) / 5;
	  g = 2 * r - r2gb + g2b;
	  b = g2b - 2 * g;
	}
      } else {
	if (column & 1) {
	  rg2 = this_ccd;
	  rgb2 = this_horizontal_interpolation;
	  r2gb = this_vertical_interpolation;
	  b = (3 * rgb2 - r2gb - rg2) / 5;
	  g = (rgb2 - r2gb + rg2 - b) / 2;
	  r = rg2 - 2 * g;
	} else {
	  rgb2 = this_ccd;
	  rg2 = this_horizontal_interpolation;
	  g2b = this_vertical_interpolation;
	  b = (g2b - 2 * (rg2 - rgb2)) / 5;
	  g = (g2b - b) / 2;
	  r = rg2 - 2 * g;
	}
      }
      if (r < 0) r = 0;
      if (g < 0) g = 0;
      if (b < 0) b = 0;
      red  [line][column] = r;
      green[line][column] = g;
      blue [line][column] = b;
    }
  }
}

void adjust_color_and_saturation(short red[LINES][COLUMNS],
				 short green[LINES][COLUMNS],
				 short blue[LINES][COLUMNS])
{
  int line, column;
  int r_min = SMAX, g_min = SMAX, b_min = SMAX;
  int r_max =    0, g_max =    0, b_max =    0;
  int r_sum =    0, g_sum =    0, b_sum =    0;
  float sqr_saturation = sqrt(saturation);
  for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++) {
    for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++) {
      float r = red  [line][column] * rfactor;
      float g = green[line][column] * gfactor;
      float b = blue [line][column] * bfactor;
      if (saturation != 1.0) {
	float *min, *mid, *max, new_intensity;
	float intensity = r * RINTENSITY + g * GINTENSITY + b * BINTENSITY;
	if (r > g) {
	  if (r > b) {
	    max = &r;
	    if (g > b) {
	      min = &b;
	      mid = &g;
	    } else {
	      min = &g;
	      mid = &b;
	    }
	  } else {
	    min = &g;
	    mid = &r;
	    max = &b;
	  }
	} else {
	  if (g > b) {
	    max = &g;
	    if (r > b) {
	      min = &b;
	      mid = &r;
	    } else {
	      min = &r;
	      mid = &b;
	    }
	  } else {
	    min = &r;
	    mid = &g;
	    max = &b;
	  }
	}
	*mid = *min + sqr_saturation * (*mid - *min);
	*max = *min + saturation * (*max - *min);
	new_intensity = r * RINTENSITY + g * GINTENSITY + b * BINTENSITY;
	r *= intensity / new_intensity;
	g *= intensity / new_intensity;
	b *= intensity / new_intensity;
      }
      r += 0.5;
      g += 0.5;
      b += 0.5;
      if (r_min > r) r_min = r;
      if (g_min > g) g_min = g;
      if (b_min > b) b_min = b;
      if (r_max < r) r_max = r;
      if (g_max < g) g_max = g;
      if (b_max < b) b_max = b;
      r_sum += r;
      g_sum += g;
      b_sum += b;
      red  [line][column] = r;
      green[line][column] = g;
      blue [line][column] = b;
    }
  }
}

int min3(int x, int y, int z)
{
  return (x < y ? (x < z ? x : z) : (y < z ? y : z));
}

int max3(int x, int y, int z)
{
  return (x > y ? (x > z ? x : z) : (y > z ? y : z));
}

void determine_limits(const short red[LINES][COLUMNS],
		      const short green[LINES][COLUMNS],
		      const short blue[LINES][COLUMNS],
		      int *low_i_ptr, int *high_i_ptr)
{
  unsigned int histogram[HISTOGRAM_STEPS + 1];
  int column, line, i, s;
  int low_i = *low_i_ptr, high_i = *high_i_ptr;
  int max_i = 0;
  for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++) {
    for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++) {
      i = max3(red[line][column], green[line][column], blue[line][column]);
      if (i > max_i) max_i = i;
    }
  }
  if (low_i == -1) {
    for (i = 0; i <= HISTOGRAM_STEPS; i++) histogram[i] = 0;
    for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++) {
      for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++) {
	i = min3(red[line][column], green[line][column], blue[line][column]);
	histogram[i * HISTOGRAM_STEPS / max_i]++;
      }
    }
    for (low_i = 0, s = 0;
	 low_i <= HISTOGRAM_STEPS && s < NET_PIXELS * norm_percentage / 100;
	 low_i++) {
      s += histogram[low_i];
    }
    low_i = (low_i * max_i + HISTOGRAM_STEPS / 2) / HISTOGRAM_STEPS;
    *low_i_ptr = low_i;
  }
  if (high_i == -1) {
    for (i = 0; i <= HISTOGRAM_STEPS; i++) histogram[i] = 0;
    for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++) {
      for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++) {
	i = max3(red[line][column], green[line][column], blue [line][column]);
	histogram[i * HISTOGRAM_STEPS / max_i]++;
      }
    }
    for (high_i = HISTOGRAM_STEPS, s = 0;
	 high_i >= 0 && s < NET_PIXELS * norm_percentage / 100; high_i--) {
      s += histogram[high_i];
    }
    high_i = (high_i * max_i + HISTOGRAM_STEPS / 2) / HISTOGRAM_STEPS;
    *high_i_ptr = high_i;
  }
  fprintf(stderr, "low_i = %d, high_i = %d\n", low_i, high_i);
}

unsigned char *make_gamma_table(int range)
{
  int i;
  double factor = pow(256.0, 1.0 / gamma_value) / range;
  unsigned char *gamma_table;
  if ((gamma_table = malloc(range * sizeof(unsigned char))) == NULL) {
    fprintf(stderr, "%s: can't allocate memory for gamma table\n", progname);
    exit(1);
  }
  for (i = 0; i < range; i++) {
    int g = pow((double)i * factor, gamma_value) + 0.5;
#ifdef DEBUG
    fprintf(stderr, "gamma[%4d] = %3d\n", i, g);
#endif
    if (g > 255) g = 255;
    gamma_table[i] = g;
  }
  return gamma_table;
}

int lookup_gamma_table(int i, int low_i, int high_i,
		       const unsigned char gamma_table[])
{
  if (i <= low_i)  return   0;
  if (i >= high_i) return 255;
  return gamma_table[i - low_i];
}

void output_rgb(const short red[LINES][COLUMNS],
		const short green[LINES][COLUMNS],
		const short blue[LINES][COLUMNS],
		int low_i, int high_i, FILE *outfp)
{
  int r_min = 255, g_min = 255, b_min = 255;
  int r_max =   0, g_max =   0, b_max =   0;
  int r_sum =   0, g_sum =   0, b_sum =   0;
  int column, line;
  unsigned char *gamma_table = make_gamma_table(high_i - low_i);

  for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++) {
    for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++) {
      int r =
	lookup_gamma_table(red  [line][column], low_i, high_i, gamma_table);
      int g =
	lookup_gamma_table(green[line][column], low_i, high_i, gamma_table);
      int b =
	lookup_gamma_table(blue [line][column], low_i, high_i, gamma_table);
      if (r > 255) r = 255; else if (r < 0) r = 0;
      if (g > 255) g = 255; else if (g < 0) g = 0;
      if (b > 255) b = 255; else if (b < 0) b = 0;
      putc(r, outfp);
      putc(g, outfp);
      putc(b, outfp);
      if (r_min > r) r_min = r;
      if (g_min > g) g_min = g;
      if (b_min > b) b_min = b;
      if (r_max < r) r_max = r;
      if (g_max < g) g_max = g;
      if (b_max < b) b_max = b;
      r_sum += r;
      g_sum += g;
      b_sum += b;
    }
  }
  free(gamma_table);
  fprintf(stderr, "r: min = %d, max = %d, ave = %d\n",
	  r_min, r_max, r_sum / NET_PIXELS);
  fprintf(stderr, "g: min = %d, max = %d, ave = %d\n",
	  g_min, g_max, g_sum / NET_PIXELS);
  fprintf(stderr, "b: min = %d, max = %d, ave = %d\n",
	  b_min, b_max, b_sum / NET_PIXELS);
}

void cmttoppm(int infd, FILE *outfp)
{
  int pgm_p = 0;
  unsigned char ccd[LINES][COLUMNS];
  short horizontal_interpolation[LINES][COLUMNS];
  short red[LINES][COLUMNS], green[LINES][COLUMNS], blue[LINES][COLUMNS];

  read_cmt_file(infd, ccd);

  /* Decode raw CCD data to RGB */
  set_initial_interpolation(ccd, horizontal_interpolation);
  interpolate_horizontally(ccd, horizontal_interpolation);
  interpolate_vertically(ccd, horizontal_interpolation, red, green, blue);

  adjust_color_and_saturation(red, green, blue);

  /* Determine lower and upper limit using histogram */
  if (low_i == -1 || high_i == -1) {
    determine_limits(red, green, blue, &low_i, &high_i);
  }

  /* Output PPM file */
  fprintf(outfp, "P%d\n%d %d\n%d\n", pgm_p ? 5 : 6,
	  NET_COLUMNS, NET_LINES, 255);
  output_rgb(red, green, blue, low_i, high_i, outfp);
}

void usage(void)
{
  fprintf(stderr, "usage: %s [options] [cmt_file [ppm_file]]\n", progname);
  exit(1);
}

void main(int argc, char *const *argv)
{
  int infd, c;
  FILE *outfp;
  progname = argv[0];
  while ((c = getopt(argc, argv, "vr:g:b:s:G:n:l:h:")) != -1) {
    switch (c) {
    case 'v':
      verbose_flag = 1;
      break;
    case 'r':
      rfactor = atof(optarg);
      break;
    case 'g':
      gfactor = atof(optarg);
      break;
    case 'b':
      bfactor = atof(optarg);
      break;
    case 's':
      saturation = atof(optarg);
      break;
    case 'G':
      gamma_value = atof(optarg);
      break;
    case 'n':
      norm_percentage = atoi(optarg);
      break;
    case 'l':
      low_i = atoi(optarg);
      break;
    case 'h':
      high_i = atoi(optarg);
      break;
    default:
      usage();
    }
  }
  if (optind + 2 < argc) usage();
  if (optind >= argc) {
    infd = 0;
  } else {
    if ((infd = open(argv[optind], O_RDONLY)) < 0) {
      perror(argv[optind]);
      exit(1);
    }
  }
  optind++;
  if (optind >= argc) {
    outfp = stdout;
  } else {
    if ((outfp = fopen(argv[optind], "wb")) == NULL) {
      perror(argv[optind]);
      exit(1);
    }
  }
  cmttoppm(infd, outfp);
  fclose(outfp);
}
