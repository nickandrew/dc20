#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "dc20_hif.h"

#define VERSION_STRING  "(Version 2.11 / Oct 98)"

#define FULL_FEATURED  1

char filename[80]= "";
int com_nr= 1;
long baud= 9600;
int save_no= 0;
int max_fcnt= 100;
int video_mode= 0;

void show_error(int error)
{
  switch (error)
  {
    case ERR_DATA_WRONG:   printf("  Wrong data received!"); break;
    case ERR_NO_DATA:      printf("  No data received!"); break;
    case ERR_NO_CAMERA:    printf("  Camera not found!"); break;
    case ERR_PACKET_WRONG: printf("  Wrong data packet received!"); break;
    case ERR_NO_RESPONSE:  printf("  No response from camera!"); break;
    case ERR_IMPOSSIBLE:   printf("  Impossible!"); break;
    case ERR_DATA_SAVE:    printf("  Data save error!"); break;
    case ERR_USER_BREAK:   printf("  User Break!"); break;
    case ERR_NO_DC25_SUPP: printf("  Command for DC25 not supported!"); break;
    default:               printf("  Error: %2X", error);
  }
}

void show_status(int full)
{
  if (full)
  {
    printf("\nDC%02X camera with %ld baud at %s detected.",
             dc_type,            baud,     com_dev);
  }

  if (sts_res == RES_LOW)
    printf("\nResolution:  low");
  else
    printf("\nResolution: high");
  if (sts_bat)
    printf("\nBattery:    down");
  else
    printf("\nBattery:    good");
  printf("\n%2d photos taken \n%2d photos free", sts_pic_cnt, sts_pic_rem);
}

void program_exit(int exit_code)
{
  close_dc20();

  printf("\n\nLook at my 'DC20 Secrets' site!\n");
  printf("http://home.t-online.de/home/Oliver.Hartmann/dc20secr.htm\n");
  exit(exit_code);
}

void usage(void)
{
  printf("\nKodak DC20/DC25 RS-232 Terminal at %ld baud ", baud);
  printf("\n\n");
  printf("written by Oliver.Hartmann@t-online.de ");
  printf(VERSION_STRING);
  printf(" \n\n");
  printf("DC20term [-Bxxxxx] [-Cx] [-Vx]\n");
  printf("            |         |    |---- take x photos in video mode \n");
  printf("            |         |----COM-Port x (<1>, 2, 3, 4)         \n");
  printf("            |----Baud xxxxx (<9600>,19200,38400,57600,115200)\n");
}

void help(void)
{
  printf("\n");
  printf("COMMANDS: \n");
  printf("  D  - Download Pictures \n");
  printf("  E  - Erase Camera Memory\n");
  printf("  P  - Take Picture \n");
  printf("  Q  - Quit \n");
  printf("  R  - Toggle Resolution \n");
  printf("  S  - Show DC20 Status \n");
  printf("  T  - Load Thumbnails \n");
  printf("  X  - Download Pictures and Exit \n");
}

void cmdline(int argc, char *argv[])
{
  int i;
  char *ptr;

  for(i=argc-1;i>0;i--)
  {
    ptr = argv[i];
    if (*ptr == '-')
    {
      ptr++;
      switch(*ptr)
      {
        case 'c':
        case 'C': com_nr= atoi(++ptr);
                  break;
        case 'b':
        case 'B': baud= atol(++ptr);
                  break;
        case 'n':
        case 'N': max_fcnt= atoi(++ptr);
                  if (max_fcnt < 0)
                    max_fcnt= 100;
                  if (max_fcnt < 10)
                    max_fcnt*= 100;
                  break;
        case 'v':
        case 'V': video_mode= atoi(++ptr);
                  if (video_mode <= 0)
                    video_mode= 1;
                  if (video_mode > 999)
                    video_mode= 999;
                  break;
        case 'h':
        case 'H':
        case '?': usage();
                  program_exit(0);
                  break;
        default: break;
      }
    }
    else
      sscanf(ptr, "%s", filename);
  }
}

int main(int argc, char *argv[])
{
  int fcnt=0, i, error= 0;
  int first_cnt = -1;
  char key, fname[127];
  FILE *ofp;
  static int img_inf[MAX_PICT][5];

  cmdline(argc, argv);

  remove("dc2tga.cmd");

  usage();

  if (!video_mode)
    help();

  if ((error= init_dc20(com_nr, baud)) != 0)
  {
    printf("\nCamera not found at %s!", com_dev);
    printf("\nCheck the -C option and if camera is ON.");
    program_exit(-1);
  }

  if ((error= get_status()) != 0)
  {
    show_error(error);
    program_exit(-1);
  }
  else
    show_status(0);

  if (video_mode)
  {
#if FULL_FEATURED
    if (error == 0)
    {
      if (sts_pic_cnt > 0)
      {
        printf("\n\nThe camera memory isn't empty - Erase it and start again!");
        program_exit(1);
      }
      for (fcnt= 999; fcnt>0; fcnt--)
      {
        sprintf(fname, "dc_%03d.dc2", fcnt);
        if((ofp=fopen(fname,"rb")) != NULL)
        {
          fclose(ofp);  /* found last used name */
          break;
        }
      }
      /* video sequence *****************************************/
      for (i= 1; i <= video_mode; i++)
      {
        if ((error= take_picture()) != 0)
          show_error(error);

        if (fcnt == 999)
          printf("\nNo free filename found!");

        sprintf(fname, "dc_%03d.dc2", ++fcnt);
        if((ofp=fopen(fname,"rb")) != NULL)
        {
          fclose(ofp);
          printf("  Filename already exists!");
          program_exit(1);
        }
        if ((ofp= fopen(fname, "wb")) == NULL)
        {
          printf("  Can't open file!");
          program_exit(1);
        }
        printf("\n");

        if ((error= download_picture(1, ofp)) != 0)
          show_error(error);
        else
        {
          if (!save_no)
            save_no= fcnt;
          printf("\nPicture %d saved as %s", i, fname);
        }
        fclose(ofp);
        if (error)
        {
          remove(fname);
          program_exit(1);
        }

        if ((error= erase_dc20_memory()) != 0)
        {
          show_error(error);
          program_exit(1);
        }
      }
      program_exit(0);
    }
    else
      program_exit(1);
#else
  printf("\n\nVideo Mode not available in this evaluation copy!");
  program_exit(0);
#endif
  }

  printf("\n: ");

  for(;;)
  {
    int dpic= 0;
    fflush(stdout);
    key= toupper(getchar());
    switch (key)
    {
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case 'D':
      case 'X': for (fcnt= max_fcnt; fcnt>0; fcnt--)
                {
                  sprintf(fname, "dc_%03d.dc2", fcnt);
                  if((ofp=fopen(fname,"rb")) != NULL)
                  {
                    if (first_cnt < 0)
                      first_cnt= fcnt+1;
                    fclose(ofp);  /* found last used name */
                    break;
                  }
                  sprintf(fname, "dc_%03d.img", fcnt);
                  if((ofp=fopen(fname,"rb")) != NULL)
                  {
                    if (first_cnt < 0)
                      first_cnt= fcnt+1;
                    fclose(ofp);  /* found last used name */
                    break;
                  }
                }
                if ((error= get_status()) != 0)
                {
                  show_error(error);
                  break;
                }
                dpic= (int) key - 48;
                if ( (dpic > 0) && (dpic < 9) )
                  i= dpic;
                else
                {
                  i= 1;
                  dpic= 0;
                }
                for ( ; i<= sts_pic_cnt; i++)
                {
                  sprintf(fname, "dc_%03d.dc2", ++fcnt);
                  if((ofp=fopen(fname,"rb")) != NULL)
                  {
                    fclose(ofp);
                    printf("  Filename already exists!");
                    break;
                  }
                  if ((ofp= fopen(fname, "wb")) == NULL)
                  {
                    printf("  Can't open file!");
                    break;
                  }
                  printf("\n");
                  if ((error= download_picture(i, ofp)) != 0)
                    show_error(error);
                  else
                  {
                    if (!save_no)
                      save_no= fcnt;
                    printf("\nPicture %d saved as %s", i, fname);
                  }
                  fclose(ofp);
                  if (error)
                  {
                    remove(fname);
                    break;
                  }
                  if (dpic)
                    break;
                }
                if (key == 'X')
                {
#if FULL_FEATURED
                  if (first_cnt > 0)
                  {
                    if((ofp=fopen("dc2tga.cmd","wt")) != NULL)
                    {
                      fprintf(ofp, "-x%d -a\n", first_cnt);
                      fclose(ofp);
                    }
                  }
                  program_exit(0);   /* Quit */
#else
                  printf("\nNot available in this evaluation copy!");
#endif
                }
                break;
      case 'Q': program_exit(0);   /* Quit */
                break;
      case 'E': if ((error= erase_dc20_memory()) != 0)
                  show_error(error);
                else
                  show_status(0);
                break;
      case 'P': if ((error= take_picture()) != 0)
                  show_error(error);
                else
                  show_status(0);
                break;
      case 'R': if ((error= toggle_resolution()) != 0)
                   show_error(error);
                 else
                   show_status(0);
                break;
      case 'S': if ((error= get_status()) != 0)
                {
                  show_error(error);
                  break;
                }
                else
                  show_status(1);

                if ((error= load_image_infos(img_inf)) != 0)
                  show_error(error);
                else
                  for (i= 1; i<= sts_pic_cnt; i++)
                  {
                    int s;
                    char f[8];

                    if (i > MAX_PICT)
                      break;

                    if (img_inf[i][0] < 7)
                      strcpy(f, "f/4 ");
                    else
                      strcpy(f, "f/11");
                    if (img_inf[i][3] < 18)
                      s= 30;
                    else if (img_inf[i][3] < 26)
                      s= 60;
                    else if (img_inf[i][3] < 33)
                      s= 125;
                    else if (img_inf[i][3] < 40)
                      s= 250;
                    else if (img_inf[i][3] < 48)
                      s= 500;
                    else if (img_inf[i][3] < 56)
                      s= 1000;
                    else if (img_inf[i][3] < 62)
                      s= 2000;
                    else if (img_inf[i][3] < 67)
                      s= 4000;
                    else
                      s= 8000;
                    printf("\nPicture %2d: %s 1/%d", i, f, s);
                    if (img_inf[i][4] == 0)
                      printf(" (high res)");
                    else
                      printf(" (low res)");
                  }
                break;
      case 'T': if ((ofp= fopen("thumbnls.dc2", "wb")) == NULL)
                {
                  printf("  Can't open file!");
                  break;
                }
                printf("\n");
                if ((error= load_thumbnails(ofp)) != 0)
                  show_error(error);
                else
                  printf("\nAll thumbnails saved in thumbnls.dc2");
                fclose(ofp);
                break;
      case 0xA: printf("\n: ");
                break;
      case 0x0:
      case 0xD:
      case 32:
                break;
      default:  help();
    }
  }
}
