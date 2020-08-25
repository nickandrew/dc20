/*
 *
 *  Converts raw data files of Chinon ES-1000 or Kodak DC20 to TGA files.
 *
 *  based on cmttoppm.c written by YOSHIDA Hideki <hideki@yk.rim.or.jp>
 *  enhanced to dc2totga.c by Oliver.Hartmann@t-online.de
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <ctype.h>

#ifdef BC
/* Borland C */
#include <conio.h>
#endif

#define VERSION_STRING  "Version 1.61 / Oct 98"

#define FULL_FEATURED  1

#define UINT8 unsigned char
#define UINT16 unsigned short
#define UINT32 unsigned long

#define COLUMNS     512
#define LINES       243L
#define RES_LINES   375

#define LEFT_MARGIN   2
#define RIGHT_MARGIN 10
#define TOP_MARGIN    1
#define BOTTOM_MARGIN 1

#define NET_COLUMNS (COLUMNS - LEFT_MARGIN - RIGHT_MARGIN)
#define NET_LINES   (LINES - TOP_MARGIN - BOTTOM_MARGIN)
#define NET_PIXELS  (NET_COLUMNS * NET_LINES)

#define CAMERA_HEADER_SIZE 512

#define SCALE 64
#define SMAX (256 * SCALE - 1)
#define HORIZ_IPOL 3
#define HISTOGRAM_STEPS 4096

#define RFACTOR 0.64f
#define GFACTOR 0.58f
#define BFACTOR 1.00f
#define RINTENSITY 0.476f
#define GINTENSITY 0.299f
#define BINTENSITY 0.175f

#define SATURATION 1.2f
#define NORM_PERCENTAGE 0.5f
#define GAMMA 0.5f

long low_i = -1, high_i = -1;
float gamma_value = GAMMA;
float saturation = SATURATION;
float rfactor = RFACTOR, gfactor = GFACTOR, bfactor = BFACTOR;
float norm_percentage = NORM_PERCENTAGE;
int all_files= 0, fcnt= 1, opt_lev= 2;
UINT8 max_ccd_val= 255;
char filename[127]= { 0 };

UINT8 *ccd[RES_LINES];
short *horiz_ipol[RES_LINES];
short *red[RES_LINES], *green[RES_LINES], *blue[RES_LINES];

static struct
{
  char part1[12];
  UINT16 wid;
  UINT16 hig;
  char part2[2];
} thmb_tga_hd=
  {
    {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    80,  /* !!! width and height must be in little endian */
    60,
    {0x18, 0x20}
  };

int read_dc2_file(FILE *infd, FILE *outfp, UINT8 *ccd[LINES])
{
  int line, column;
  long fsize;
  int thmb, nof_thmb;

  fseek(infd, 0, SEEK_END);
  fsize= ftell(infd);

  if ( ((nof_thmb= fsize / 5120L) * 5120L) == fsize )
  {
#if FULL_FEATURED
    printf("\ncontains %d thumbnails\n", nof_thmb);
    if (nof_thmb > 50)
      return(3);

    thmb_tga_hd.hig= 64* nof_thmb;
    fwrite(&thmb_tga_hd, 1, sizeof(thmb_tga_hd), outfp);

    for (thmb= 0; thmb < nof_thmb; thmb++)
    {
      UINT16 thmb_max= 0;
      fseek(infd, thmb*5120L, SEEK_SET);

      for (line= 0; line < 60; line++)
      {
        size_t n = fread(ccd[line], 1, 80, infd);
        if (n < 80) {
          fprintf(stderr, "Short read! %ld < 80\n", n);
          exit(2);
        }
        for (column= 0; column< 80; column++)
          if (ccd[line][column] > thmb_max)
            thmb_max= ccd[line][column];
      }

      for (line= 0; line < 60; line++)
      {
        for (column= 0; column< 80; column++)
        {
          UINT16 thmb_dat;
          thmb_dat= ((UINT16) ccd[line][column] * 255) / thmb_max;
          thmb_dat= 255 - thmb_dat;
          thmb_dat= (thmb_dat*thmb_dat) / 255;
          thmb_dat= 255 - thmb_dat;

          putc(thmb_dat, outfp);
          putc(thmb_dat, outfp);
          putc(thmb_dat, outfp);
        }
      }

      for (; line < 64; line++)
      {
        for (column= 0; column< 80; column++)
        {
          putc(0, outfp);
          putc(0, outfp);
          putc(0, outfp);
        }
      }
    }
    return (20);
#else
    printf("\nThumbnail files not supported in this evaluation copy!\n");
    return (11);
#endif
  }
  else if ( fsize < 124928L )
  {
#if FULL_FEATURED
    fseek(infd, CAMERA_HEADER_SIZE/2, SEEK_SET);
    for (line= 0; line < LINES; line++)
    {
      for (column= 0; column< COLUMNS; column+=4)
      {
        size_t n = fread(&ccd[line][column], 1, 2, infd);
        if (n < 2) {
          fprintf(stderr, "Short read! %ld < 2\n", n);
          exit(2);
        }
        ccd[line][column+2]= ccd[line][column];
        ccd[line][column+3]= ccd[line][column+1];
      }
      ccd[line][2]= ccd[line][6];
      ccd[line][3]= ccd[line][7];
      ccd[line][0]= ccd[line][2];
      ccd[line][1]= ccd[line][3];
      ccd[line][COLUMNS-RIGHT_MARGIN+1]= ccd[line][COLUMNS-RIGHT_MARGIN-3];
      ccd[line][COLUMNS-RIGHT_MARGIN+2]= ccd[line][COLUMNS-RIGHT_MARGIN-2];
      for (column= 4; column< COLUMNS-3; column+=4)
      {
        ccd[line][column]= (ccd[line][column-2]+ccd[line][column+2])/2;
        ccd[line][column+1]= (ccd[line][column-1]+ccd[line][column+3])/2;
      }

      for (column= 0; column< COLUMNS; column++)
        if (ccd[line][column] < 2)
          ccd[line][column]= 2;
    }
#else
    printf("\nLow res files not supported in this evaluation copy!\n");
    return (10);
#endif
  }
  else
  {
    if ( fsize == 124928L )
      fseek(infd, CAMERA_HEADER_SIZE, SEEK_SET);
    else if ( fsize == 125056L )
      fseek(infd, CAMERA_HEADER_SIZE+128, SEEK_SET);
    else if ( fsize == 130048L )
      fseek(infd, CAMERA_HEADER_SIZE+5120, SEEK_SET);
    else
      return (2);

    for (line= 0; line < LINES; line++)
    {
      size_t n = fread(ccd[line], 1, COLUMNS, infd);
      if (n < COLUMNS) {
        fprintf(stderr, "Short read! %ld < %d\n", n, COLUMNS);
        exit(2);
      }

      for (column= 0; column< COLUMNS; column++)
        if (ccd[line][column] < 2)
          ccd[line][column]= 2;
    }
  }

  return (0);
}

void set_initial_interpolation(UINT8 *ccd[LINES],
                               short *horiz_ipol[LINES])
{
  int column, line;
  for (line = 0; line < LINES; line++)
  {
    horiz_ipol[line][LEFT_MARGIN]= ccd[line][LEFT_MARGIN + 1] * SCALE;
    horiz_ipol[line][COLUMNS-RIGHT_MARGIN-1]=  ccd[line][COLUMNS-RIGHT_MARGIN-2] * SCALE;
    for (column = LEFT_MARGIN+1; column < COLUMNS-RIGHT_MARGIN- 1; column++)
    {
      horiz_ipol[line][column] = ((short) ccd[line][column-1] + (short) ccd[line][column+1]) * (SCALE/2);
    }
  }
}

void ipol_horizontally(UINT8 *ccd[LINES], short *horiz_ipol[LINES])
{
  int column, line, i, init_col;
  for (line = TOP_MARGIN-1; line < LINES-BOTTOM_MARGIN+1; line++)
  {
    for (i = 0; i < HORIZ_IPOL; i++)
    {
      for (init_col= LEFT_MARGIN+1; init_col <= LEFT_MARGIN+2; init_col++)
      {
        for (column = init_col; column < COLUMNS-RIGHT_MARGIN-1; column+= 2)
        {
           horiz_ipol[line][column] = (short)
                     (((float)ccd[line][column - 1] / horiz_ipol[line][column - 1] +
                       (float)ccd[line][column + 1] / horiz_ipol[line][column + 1]) *
                              ccd[line][column] * (SCALE * SCALE / 2) + 0.5f);
        }
      }
    }
  }
}

void ipol_vertically(UINT8 *ccd[LINES], short *horiz_ipol[LINES],
                     short *red[LINES], short *green[LINES], short *blue[LINES])
{
  int column, line;
  for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++)
  {
    for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++)
    {
      long r2gb, g2b, rg2, rgb2, r, g, b;
      long this_ccd = ccd[line][column] * SCALE;
      long up_ccd   = ccd[line - 1][column] * SCALE;
      long down_ccd = ccd[line + 1][column] * SCALE;
      long this_horiz_ipol = horiz_ipol[line][column];
      long this_intensity = this_ccd + this_horiz_ipol;
      long up_intensity   = horiz_ipol[line-1][column] + up_ccd;
      long down_intensity = horiz_ipol[line+1][column] + down_ccd;
      long this_vert_ipol;
      if (line == TOP_MARGIN)
      {
        this_vert_ipol = (long) ((float)down_ccd / down_intensity * this_intensity + 0.5f);
      }
      else
        if (line == LINES - BOTTOM_MARGIN - 1)
        {
          this_vert_ipol= (long)((float)up_ccd / up_intensity * this_intensity + 0.5f);
        }
        else
        {
          this_vert_ipol= (long)(((float)up_ccd/up_intensity + (float)down_ccd/down_intensity) *
                            this_intensity / 2.0f + 0.5f);
        }
      if (line & 1)
      {
        if (column & 1)
        {
          r2gb = this_ccd;
          g2b = this_horiz_ipol;
          rg2 = this_vert_ipol;
          r = (2 * (r2gb - g2b) + rg2) / 5;
          g = (rg2 - r) / 2;
          b = g2b - 2 * g;
        }
        else
        {
          g2b = this_ccd;
          r2gb = this_horiz_ipol;
          rgb2 = this_vert_ipol;
          r = (3 * r2gb - g2b - rgb2) / 5;
          g = 2 * r - r2gb + g2b;
          b = g2b - 2 * g;
        }
      }
      else
      {
        if (column & 1)
        {
          rg2 = this_ccd;
          rgb2 = this_horiz_ipol;
          r2gb = this_vert_ipol;
          b = (3 * rgb2 - r2gb - rg2) / 5;
          g = (rgb2 - r2gb + rg2 - b) / 2;
          r = rg2 - 2 * g;
        }
        else
        {
          rgb2 = this_ccd;
          rg2 = this_horiz_ipol;
          g2b = this_vert_ipol;
          b = (g2b - 2 * (rg2 - rgb2)) / 5;
          g = (g2b - b) / 2;
          r = rg2 - 2 * g;
        }
      }
      if (r < 0) r = 0;
      if (g < 0) g = 0;
      if (b < 0) b = 0;
      red  [line][column] = (short) r;
      green[line][column] = (short) g;
      blue [line][column] = (short) b;
    }
  }
}

void adjust_color_and_saturation(short *red[LINES], short *green[LINES], short *blue[LINES])
{
  int line, column;
#if DEBUG
  long ri, gi, bi;
  long r_min = SMAX, g_min = SMAX, b_min = SMAX;
  long r_max =    0, g_max =    0, b_max =    0;
  long r_sum =    0, g_sum =    0, b_sum =    0;
#endif
  float sqr_saturation = (float) sqrt(saturation);
  for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++)
  {
    for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++)
    {
      float r = red  [line][column] * rfactor;
      float g = green[line][column] * gfactor;
      float b = blue [line][column] * bfactor;
      if (saturation != 1.0f)
      {
        float *min, *mid, *max, new_intensity;
        float intensity = r * RINTENSITY + g * GINTENSITY + b * BINTENSITY;
        if (r > g)
        {
          if (r > b)
          {
            max = &r;
            if (g > b)
            {
              min = &b;
              mid = &g;
            }
            else
            {
              min = &g;
              mid = &b;
            }
          }
          else
          {
            min = &g;
            mid = &r;
            max = &b;
          }
        }
        else
        {
          if (g > b)
          {
            max = &g;
            if (r > b)
            {
              min = &b;
              mid = &r;
            }
            else
            {
              min = &r;
              mid = &b;
            }
          }
          else
          {
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
      red  [line][column] = (short) (r + 0.5f);
      green[line][column] = (short) (g + 0.5f);
      blue [line][column] = (short) (b + 0.5f);
#if DEBUG
      ri= (long) (r + 0.5f);
      gi= (long) (g + 0.5f);
      bi= (long) (b + 0.5f);
      if (r_min > ri) r_min = ri;
      if (g_min > gi) g_min = gi;
      if (b_min > bi) b_min = bi;
      if (r_max < ri) r_max = ri;
      if (g_max < gi) g_max = gi;
      if (b_max < bi) b_max = bi;
      r_sum+= ri;
      g_sum+= gi;
      b_sum+= bi;
#endif
    }
  }
}


int lumi(UINT16 r, UINT16 g, UINT16 b)
{
  return ( (3*r)/10 + (6*g)/10 + b/10 );
}

/*
int min3(int x, int y, int z)
{
  return (x < y ? (x < z ? x : z) : (y < z ? y : z));
}

int max3(int x, int y, int z)
{
  return (x > y ? (x > z ? x : z) : (y > z ? y : z));
}
*/

void determine_limits(short *red[LINES], short *green[LINES],
                      short *blue[LINES], long *low_i_ptr, long *high_i_ptr)
{
  static unsigned int histogram[HISTOGRAM_STEPS + 1];
  int column, line;
  long nrm_perc= (long) (norm_percentage * 100);
  long i, s;
  long low_i = *low_i_ptr, high_i = *high_i_ptr;
  long max_i = 0;
  for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++)
  {
    for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++)
    {
      /* i = max3(red[line][column], green[line][column], blue[line][column]); */
      i = lumi(red[line][column], green[line][column], blue[line][column]);
      if (i > max_i) max_i = i;
    }
  }
  if (low_i == -1)
  {
    for (i = 0; i <= HISTOGRAM_STEPS; i++)
      histogram[i] = 0;
    for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++)
    {
      for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++)
      {
        /* i = min3(red[line][column], green[line][column], blue[line][column]); */
        i = lumi(red[line][column], green[line][column], blue[line][column]);
        histogram[i * HISTOGRAM_STEPS / max_i]++;
      }
    }
    s= 0;
    low_i= 0;
    for ( ; low_i<= HISTOGRAM_STEPS && s< NET_PIXELS*nrm_perc/10000; low_i++)
    {
      s += histogram[low_i];
    }
    low_i = (low_i * max_i + HISTOGRAM_STEPS / 2) / HISTOGRAM_STEPS;
    *low_i_ptr = low_i;
  }
  if (high_i == -1)
  {
    for (i = 0; i <= HISTOGRAM_STEPS; i++)
      histogram[i] = 0;
    for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++)
    {
      for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++)
      {
        /* i = max3(red[line][column], green[line][column], blue [line][column]); */
        i = lumi(red[line][column], green[line][column], blue [line][column]);
        histogram[i * HISTOGRAM_STEPS / max_i]++;
      }
    }
    s= 0;
    high_i = HISTOGRAM_STEPS;
    for ( ; high_i >= 0 && s < NET_PIXELS*nrm_perc/10000; high_i--)
    {
      s += histogram[high_i];
    }
    high_i = (high_i * max_i + HISTOGRAM_STEPS / 2) / HISTOGRAM_STEPS;
    *high_i_ptr = high_i;
  }
#if DEBUG
  printf(" low_i = %ld, high_i = %ld ", low_i, high_i);
#endif
}

UINT8 *make_gamma_table(int range)
{
  int i;
  double factor = pow(256.0, 1.0 / gamma_value) / range;
  UINT8 *gamma_table;
  if ((gamma_table = malloc(range * sizeof(UINT8))) == NULL)
  {
    fprintf(stderr, "Can't allocate memory for gamma table\n");
    exit(1);
  }
  for (i = 0; i < range; i++)
  {
    int g = (int) (pow((double)i * factor, gamma_value) + 0.5);
    if (g > 255) g = 255;
    gamma_table[i] = (UINT8) g;
  }
  return gamma_table;
}

int lookup_gamma_table(int i, long low_i, long high_i,
                       UINT8 gamma_table[])
{
  if (i <= (int) low_i)  return   0;
  if (i >= (int) high_i) return 255;
  return gamma_table[i - low_i];
}

void stretch(short *red[RES_LINES], short *green[RES_LINES],
             short *blue[RES_LINES], long low_i, long high_i,
             UINT8 *ccd[LINES])
{
#if DEBUG
  int  r_min= 255, g_min= 255, b_min= 255;
  int  r_max=   0, g_max=   0, b_max=   0;
  long r_sum=   0, g_sum=   0, b_sum=   0;
#endif
  int column, line, i;
  UINT8 *gamma_table = make_gamma_table((int) (high_i - low_i));

  for (line = TOP_MARGIN; line < LINES - BOTTOM_MARGIN; line++)
  {
    for (column = LEFT_MARGIN; column < COLUMNS - RIGHT_MARGIN; column++)
    {
      int r = lookup_gamma_table(red  [line][column], low_i, high_i, gamma_table);
      int g = lookup_gamma_table(green[line][column], low_i, high_i, gamma_table);
      int b = lookup_gamma_table(blue [line][column], low_i, high_i, gamma_table);

      if (ccd[line][column] >= max_ccd_val)
        if ( ccd[line][column-1] >= max_ccd_val ||
             ccd[line][column+1] >= max_ccd_val ||
             ccd[line-1][column] >= max_ccd_val ||
             ccd[line+1][column] >= max_ccd_val )
        {
          r= g= b= ccd[line][column];
        }

      if (r > 255) r = 255; else if (r < 0) r = 0;
      if (g > 255) g = 255; else if (g < 0) g = 0;
      if (b > 255) b = 255; else if (b < 0) b = 0;
      red  [line-TOP_MARGIN][column]= (short) r;
      green[line-TOP_MARGIN][column]= (short) g;
      blue [line-TOP_MARGIN][column]= (short) b;
#if DEBUG
      if (r_min > r) r_min = r;
      if (g_min > g) g_min = g;
      if (b_min > b) b_min = b;
      if (r_max < r) r_max = r;
      if (g_max < g) g_max = g;
      if (b_max < b) b_max = b;
      r_sum += r;
      g_sum += g;
      b_sum += b;
#endif
    }
  }

  free(gamma_table);

  for (line=RES_LINES-1; line >= 0; line--)
  {
    float fy = NET_LINES / (float) RES_LINES;
    float by, ey;
    int   byab, eyab;
    float teilzeile;
    UINT16 ufakt;
    UINT16 ofakt;

    by=fy*line;
    ey=fy*(line+1);

    byab=(int)by;
    eyab=(int)ey;
    if ((float)eyab==ey) eyab--;

    teilzeile = eyab/fy;
    ufakt = (UINT16) ((teilzeile-line)*256);
    ofakt = (UINT16) ((line+1-teilzeile)*256);

    for (column=LEFT_MARGIN; column< COLUMNS-RIGHT_MARGIN; column++)
    {
      UINT16 erg_blau = 128;
      UINT16 erg_gruen= 128;
      UINT16 erg_rot  = 128;

      if(byab!=eyab)
      {
        for (i=eyab; i>=byab; i--)
        {
          UINT16 sum_blau  = blue[i][column];
          UINT16 sum_gruen = green[i][column];
          UINT16 sum_rot   = red[i][column];

          if(i==byab)
          {
            sum_blau  *= ufakt;
            sum_gruen *= ufakt;
            sum_rot   *= ufakt;
          }
          else
          {
            sum_blau  *= ofakt;
            sum_gruen *= ofakt;
            sum_rot   *= ofakt;
          }
          erg_blau += sum_blau;
          erg_gruen+= sum_gruen;
          erg_rot  += sum_rot;
        }
        blue [line][column]= (short) (erg_blau/256);
        green[line][column]= (short) (erg_gruen/256);
        red  [line][column]= (short) (erg_rot/256);
      }
      else
      {
        blue [line][column]= blue [byab][column];
        green[line][column]= green[byab][column];
        red  [line][column]= red  [byab][column];
      }
    }      /* Ende Spalte Zielbild */
  }        /* Ende Zeile  Zielbild */

#if DEBUG
  fprintf(stderr, "r: min= %3d, max= %3d, avg= %3d\n", r_min, r_max, r_sum/NET_PIXELS);
  fprintf(stderr, "g: min= %3d, max= %3d, avg= %3d\n", g_min, g_max, g_sum/NET_PIXELS);
  fprintf(stderr, "b: min= %3d, max= %3d, avg= %3d\n", b_min, b_max, b_sum/NET_PIXELS);
#endif
}

void sharpen(short *red[RES_LINES], short *green[RES_LINES], short *blue[RES_LINES])
{
  int r, g, b, f11, f[3][3];
  int  j, l, fakt;
  int typ= 1, percent;

  percent= opt_lev*3;

  if (percent <= 0) percent = 1;
  else if (percent > 50) percent = 50;

  fakt= 100/percent - 1;

  switch(typ)
  {
    case 0:
      f11 = fakt + 4;
      f[0][0] =  0; f[0][1] =  -1; f[0][2] =  0;
      f[1][0] = -1; f[1][1] = f11; f[1][2] = -1;
      f[2][0] =  0; f[2][1] =  -1; f[2][2] =  0;
      break;

    case 1:
      f11 = fakt + 8;
      f[0][0] = -1; f[0][1] =  -1; f[0][2] = -1;
      f[1][0] = -1; f[1][1] = f11; f[1][2] = -1;
      f[2][0] = -1; f[2][1] =  -1; f[2][2] = -1;
      break;

    default:
      break;
   }

  for(l=1; l < (RES_LINES-1); l++)
  {
    for(j=LEFT_MARGIN+1; j < (COLUMNS-RIGHT_MARGIN-1); j++)
    {
      r= red[l][j]* f11 - red[l-1][j-1] - red[l-1][j] - red[l-1][j+1] -
                          red[l  ][j-1]               - red[l  ][j+1] -
                          red[l+1][j-1] - red[l+1][j] - red[l+1][j+1];
      g= green[l][j]* f11 - green[l-1][j-1] - green[l-1][j] - green[l-1][j+1] -
                            green[l  ][j-1]                 - green[l  ][j+1] -
                            green[l+1][j-1] - green[l+1][j] - green[l+1][j+1];
      b= blue[l][j]* f11 - blue[l-1][j-1] - blue[l-1][j] - blue[l-1][j+1] -
                           blue[l  ][j-1]                - blue[l  ][j+1] -
                           blue[l+1][j-1] - blue[l+1][j] - blue[l+1][j+1];
      if (fakt > 1)
      {
        r/= fakt;
        g/= fakt;
        b/= fakt;
      }
      if (r > 255) r = 255; else if (r < 0) r = 0;
      if (g > 255) g = 255; else if (g < 0) g = 0;
      if (b > 255) b = 255; else if (b < 0) b = 0;

      red  [l][j]= (short) r;
      green[l][j]= (short) g;
      blue [l][j]= (short) b;
    }
  }
}

static struct
{
  char part1[12];
  UINT16 wid;
  UINT16 hig;
  char part2[2];
} tga_header=
  {
    {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    NET_COLUMNS-4,  /* !!! width and height must be in little endian */
    RES_LINES,
    {0x18, 0x20}
  };

void output_rgb(short *red[RES_LINES], short *green[RES_LINES],
                short *blue[RES_LINES], FILE *outfp)
{
  int column, line;

  fwrite(&tga_header, 1, sizeof(tga_header), outfp);

  for (line = 0; line < RES_LINES; line++)
  {
    for (column = LEFT_MARGIN+2; column < COLUMNS-RIGHT_MARGIN-2; column++)
    {
      short r= red  [line][column];
      short g= green[line][column];
      short b= blue [line][column];
      putc(b, outfp);
      putc(g, outfp);
      putc(r, outfp);
    }
  }
}

int event(void)
{
  printf(".");
  fflush(stdout);
#ifdef BC  /* Borland C */
  if (kbhit())
    if ( (getch()) == 0x1B )
    {
      while (kbhit())
        getch();
      return (1);
    }
#endif
  return (0);
}

int dc2totga(FILE *infp, FILE *outfp,
             UINT8 *ccd[RES_LINES], short *horiz_ipol[RES_LINES],
             short *red[RES_LINES], short *green[RES_LINES], short *blue[RES_LINES])
{
  /* clock_t t1= clock(); */
  int err;

  printf(" ");
  if ((err= read_dc2_file(infp, outfp, ccd)) > 0)
    return (err);

  /* Decode raw CCD data to RGB */
  set_initial_interpolation(ccd, horiz_ipol);
  if (event()) return (1);
  ipol_horizontally(ccd, horiz_ipol);
  if (event()) return (1);
  ipol_vertically(ccd, horiz_ipol, red, green, blue);
  if (event()) return (1);
  adjust_color_and_saturation(red, green, blue);
  if (event()) return (1);

  /* Determine lower and upper limit using histogram */
  low_i = high_i = -1;
  if (low_i == -1 || high_i == -1)
  {
    determine_limits(red, green, blue, &low_i, &high_i);
  }
  if (event()) return (1);

  /* stretch high */
  stretch(red, green, blue, low_i, high_i, ccd);
  if (event()) return (1);

  /* sharpen tga_file */
  if (opt_lev > 0)
    sharpen(red, green, blue);
  if (event()) return (1);

  /* Output to file */
  output_rgb(red, green, blue, outfp);

  /* printf(" %3ld", clock()-t1); */
  return (0);
}

void usage(void)
{
  printf("\nDC2toTGA converts Kodak DC20 raw data files to Targa24 image files \n");
#if !FULL_FEATURED
  printf("Evaluation copy of ");
#endif
  printf("%s", VERSION_STRING);
  printf("\n");
  printf("\nAuthor: Oliver.Hartmann@t-online.de");
  printf("\n        based on cmttoppm.c written by hideki@yk.rim.or.jp\n");
  printf("\ncommand line parameters: [filename] [-options]\n");
  printf("\noptions: ");
  printf("-h : help (shows all options)\n");
}

void help(void)
{
  usage();
  printf("         -a : convert all dc_xxx.dc2 files in the current directory\n");
  printf("         -b<val> : set blue factor to <val>\n");
  printf("         -g<val> : set green factor to <val>\n");
  printf("         -n<val> : set norm_percentage to <val>\n");
  printf("         -o<lev> : optimize (level= 0..10)\n");
  printf("         -r<val> : set red factor to <val>\n");
  printf("         -s<val> : set saturation to <val>\n");
  printf("         -v<val> : set gamma value to <val>\n");
  printf("         -x<val> : start with file dc_<val>.dc2\n");
  printf("\nhttp://home.t-online.de/home/Oliver.Hartmann/dc20secr.htm\n");
#if !FULL_FEATURED
  printf("\nEvaluation copy\n");
#endif
}

void cmdline(int argc, char *argv[], int exit_flag)
{
  int i;
  char *ptr;

  for(i=argc-1;i>0;i--)
  {
    ptr = argv[i];
    if ( *ptr == '-' )
    {
      ptr++;
      switch(toupper(*ptr))
      {
        case 'A': all_files= 1;
                  break;
        case 'B': bfactor= (float) atof(++ptr);
                  break;
        case 'C': max_ccd_val= (UINT8) atof(++ptr);
                  break;
        case 'G': gfactor= (float) atof(++ptr);
                  break;
        case 'N': norm_percentage= (float) atof(++ptr);
                  break;
        case 'O': opt_lev= (int) atof(++ptr);
                  break;
        case 'R': rfactor= (float) atof(++ptr);
                  break;
        case 'S': saturation= (float) atof(++ptr);
                  break;
        case 'V': gamma_value= (float) atof(++ptr);
                  break;
        case 'X': fcnt= (int) atof(++ptr);
                  sprintf(filename, "dc_%03d.dc2", fcnt);
                  break;
        case 'H':
        case '?': help();
                  if (exit_flag)
                    exit(0);
                  break;
        default:  if ( *ptr >= 0x30 && *ptr <= 0x39 )
                  {
                    fcnt= (int) atof(ptr);
                    sprintf(filename, "dc_%03d", fcnt);
                  }
                  break;
      }
    }
    else
      sscanf(ptr, "%s", filename);
  }
}

#define MAX_INP_LINE   128
#define MAX_ARG_CNT     20

void pars_input(void)
{
  int i, argc= 1;
  char *argv[MAX_ARG_CNT];
  char inp_line[MAX_INP_LINE];

  for (i= 0; i<MAX_INP_LINE-1; i++)
  {
    inp_line[i]= getc(stdin);
    if (inp_line[i] == 0xA)
      break;
  }
  inp_line[i]= 0;

  argv[0]= inp_line;
  if (inp_line[0] != ' ')
  {
    argv[argc]= inp_line;
    argc++;
  }
  for (i= 1; i<MAX_INP_LINE; i++)
  {
    if (inp_line[i] == 0)
      break;
    if ( inp_line[i] != ' ' && inp_line[i-1] == ' ' )
    {
      inp_line[i-1]= 0;
      argv[argc]= &inp_line[i];
      argc++;
    }
  }

  cmdline(argc, argv, 0);
}

void show_params(void)
{
  printf("\n");
  printf("red factor= %4.2f, green factor= %4.2f, blue factor = %4.2f\n",
         rfactor, gfactor, bfactor);
  printf("saturation= %4.2f, gamma_value = %4.2f, norm_percent= %.2f",
         saturation, gamma_value, norm_percentage);
  printf(", opt_lev= %d", opt_lev);
  printf("\n");
}

void prog_exit(void)
{
  int line;

  for (line= RES_LINES-1 ; line >= 0 ; line--)
  {
    free(ccd[line]);
    free(horiz_ipol[line]);
    free(red[line]);
    free(green[line]);
    free(blue[line]);
  }

  exit(0);
}

void main(int argc, char *argv[])
{
  int line;
  FILE *infp;
  FILE *outfp;
  char infn[127]= { 0 }, outfn[127]= { 0 }, *fext, *ctmp;
  int err;

  cmdline(argc, argv, 1);
  usage();

  show_params();

  while (!filename[0] && !all_files)
  {
    printf("\ncommand line: ");
    pars_input();
    show_params();
  }

  if (filename[0])
    strcpy(infn, filename);

  if (!all_files)
  {
    if ( (strstr(infn, ".dc2") == NULL) &&
         (strstr(infn, ".DC2") == NULL) &&
         (strstr(infn, ".img") == NULL) &&
         (strstr(infn, ".IMG") == NULL) &&
         (strstr(infn, ".cmt") == NULL) &&
         (strstr(infn, ".CMT") == NULL) )
      strcat(infn, ".dc2");
    if ((infp = fopen(infn, "rb")) == NULL)
    {
      fprintf(stderr, "Can't open: %s\n",infn);
        exit(1);
    }
    fclose(infp);
  }

  for (line= 0; line < RES_LINES; line++)
  {
    if ( (ccd[line]= malloc(COLUMNS*sizeof(char))) == NULL )
      break;
    if ( (horiz_ipol[line]= malloc(COLUMNS*sizeof(short))) == NULL )
      break;
    if ( (red[line]= malloc(COLUMNS*sizeof(short))) == NULL )
      break;
    if ( (green[line]= malloc(COLUMNS*sizeof(short))) == NULL )
      break;
    if ( (blue[line]= malloc(COLUMNS*sizeof(short))) == NULL )
      break;
  }
  if (line < RES_LINES)
  {
    printf("Can't allocate enough memory!\n");
    for ( ; line >= 0 ; line--)
    {
      free(ccd[line]);
      free(horiz_ipol[line]);
      free(red[line]);
      free(green[line]);
      free(blue[line]);
    }
    exit(1);
  }

  printf("\n");

  for (;;)
  {
    if (all_files)
      for (; fcnt<=999; fcnt++)
      {
        sprintf(infn, "dc_%03d.dc2", fcnt);
        if((infp=fopen(infn,"rb")) != NULL)
        {
          fclose(infp);
          break;
        }
        sprintf(infn, "DC_%03d.DC2", fcnt);
        if((infp=fopen(infn,"rb")) != NULL)
        {
          fclose(infp);
          break;
        }
        sprintf(infn, "dc_%03d.img", fcnt);
        if((infp=fopen(infn,"rb")) != NULL)
        {
          fclose(infp);
          break;
        }
        sprintf(infn, "DC_%03d.IMG", fcnt);
        if((infp=fopen(infn,"rb")) != NULL)
        {
          fclose(infp);
          break;
        }
        sprintf(infn, "dc_%03d.cmt", fcnt);
        if((infp=fopen(infn,"rb")) != NULL)
        {
          fclose(infp);
          break;
        }
        sprintf(infn, "DC_%03d.CMT", fcnt);
        if((infp=fopen(infn,"rb")) != NULL)
        {
          fclose(infp);
          break;
        }
      }

    if ((infp = fopen(infn, "rb")) == NULL)
    {
      break;
    }

    strcpy(outfn, infn);
    fext= &outfn[strlen(outfn)];
    ctmp= strstr(outfn, ".");
    while (ctmp != 0)
    {
      fext= ctmp;
      ctmp= strstr(&fext[1], ".");
    }
    strcpy(fext, ".tga");
    printf("Output-File: ");
    printf("%s", outfn);
    fflush(stdout);
    if ((outfp = fopen(outfn, "w+b")) == NULL)
    {
      fprintf(stderr, "Can't open: %s\n",outfn);
        break;
    }

    if ( (err= dc2totga(infp, outfp, ccd, horiz_ipol, red, green, blue)) > 0 )
    {
      if (err < 10)
      {
        printf("\n\nAborted!");
        break;
      }
    }

    printf("\n");
    fclose(infp);
    fclose(outfp);

    if (!all_files)
      break;

    fcnt++;
  }

  prog_exit();
}
