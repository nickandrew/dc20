#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>
#include "dc20_hif.h"

#define  Kb  1024

/* GLOBAL VARIABLES ***************************************/
unsigned char inp_buff[INP_BUFF_SIZE];
unsigned char sts_res= 0, sts_bat= 0;
unsigned char sts_pic_cnt= 0, sts_pic_rem= 0;
unsigned char dc_type= 0x25;
unsigned char com_dev[128]= "/dev/ttyS0";


/* LOCAL VARIABLES ****************************************/
speed_t com_speed= B9600;
long com_baud, baud_save= 9600;
struct termios tty_param, old_tty;
int com_hdl;

unsigned char cmd_init[]  = { 8, 0x41, 0, 0x96, 0, 0, 0, 0, 0x1A };
unsigned char cmd_status[]= { 8, 0x7F, 0, 0, 0, 0, 0, 0, 0x1A };
unsigned char cmd_shot[]  = { 8, 0x77, 0, 0, 0, 0, 0, 0, 0x1A };
unsigned char cmd_ldpic2[]= { 8, 0x51, 0, 0, 1, 0, 0, 0, 0x1A };  /* header + picture */
unsigned char cmd_ldpic1[]= { 8, 0x55, 0, 0, 1, 0, 0, 0, 0x1A };  /* header only */
unsigned char cmd_thumbn[]= { 8, 0x56, 0, 0, 1, 0, 0, 0, 0x1A };
unsigned char cmd_erase[] = { 8, 0x7A, 0, 0, 0, 0, 0, 0, 0x1A };
unsigned char cmd_lores[] = { 8, 0x71, 0, 1, 0, 0, 0, 0, 0x1A };
unsigned char cmd_hires[] = { 8, 0x71, 0, 0, 0, 0, 0, 0, 0x1A };

/* LOCAL FUNCTIONS *****************************************/

void pause(int millisec)
{

  long wait, goal;

  wait = ( (long) millisec * (long) CLOCKS_PER_SEC + 500) / 1000;
  goal = wait + (long) clock();
  while ( goal > (long) clock() )
    ;

}

/* hardware specific functions *****************************/

void cxmit(unsigned char c)
{
  if (write(com_hdl, &c, 1) != 1)
    perror("write");
}

unsigned char crcv(int *error)
{
  unsigned char c;

  if (read(com_hdl, &c, 1) == 1)
    return c;
  else
    *error+= ERR_NO_DATA;

  return 0xFF;
}

int read_from_com(int nof_bytes)
{
  unsigned int i, k, retry= 5;
  unsigned char chksum;

  while (retry--)
  {
    chksum= 0;

    /* i= read(com_hdl, inp_buff, nof_bytes); */ 
    for (i= 0; i< nof_bytes; )
    {
      if ((k= read(com_hdl, &inp_buff[i], nof_bytes-i)) < 1)
        break;
      i+= k;
    }    

    if (i == nof_bytes)
    {

      for (i= 0; i < nof_bytes; i++)
        chksum^= inp_buff[i];

      if ( chksum == 0 )
	return nof_bytes;
      else
        i= nof_bytes - 1;
    }

    if (retry)
    {
      cxmit(0xE3);
    }
  }

  return i;
}

void set_com_baud(long baud)
{
  com_baud= baud;

  switch (baud)
  {
    case  9600 : com_speed= B9600; break; 
    case  19200: com_speed= B19200; break;
    case  38400: com_speed= B38400; break;
    case  57600: com_speed= B57600; break;
    case 115200: com_speed= B115200; break;
    default:     com_speed= B9600; baud= 9600; 
  }

  cfsetospeed(&tty_param, com_speed);
  cfsetispeed(&tty_param, com_speed);

  if (tcsetattr(com_hdl, TCSANOW, &tty_param) == -1) 
  {
    perror("tcsetattr");
  }
}

void toggle_baud_rate(void)
{
  if (com_baud > 9600)
  {
    baud_save= com_baud;
    set_com_baud(9600);
  }
  else
  {
    set_com_baud(baud_save);
  }
}

void initcom(int com_nr, long baud)
{
  switch (com_nr)
  {
    case 1:  strcpy(com_dev, "/dev/ttyS0") ; break;
    case 2:  strcpy(com_dev, "/dev/ttyS1") ; break;
    case 3:  strcpy(com_dev, "/dev/ttyS2") ; break;
    case 4:  strcpy(com_dev, "/dev/ttyS3") ; break;
    default: strcpy(com_dev, "/dev/ttyS0") ; break;
  }

  if ((com_hdl = open(com_dev, O_RDWR)) == -1) 
  {
    perror("open");
    fprintf(stderr, "Could not open %s for read/write.\n", com_dev);
    exit(-1);
  }

  cfmakeraw(&tty_param);
  tty_param.c_oflag &= ~CSTOPB;
  tty_param.c_cflag |= PARENB;
  tty_param.c_cflag &= ~PARODD;
  tty_param.c_cc[VMIN] = 0;
  tty_param.c_cc[VTIME] = 15;
  cfsetospeed(&tty_param, B9600);
  cfsetispeed(&tty_param, B9600);

  tcgetattr(com_hdl, &old_tty);
  
  if (tcsetattr(com_hdl, TCSANOW, &tty_param) == -1) 
  {
    perror("tcsetattr");
    fprintf(stderr, "Could not init %s for camera access.\n", com_dev);
    close(com_hdl);
    exit(-1);
  }

  set_com_baud(baud);

}

void close_com(void)
{
  tcsetattr(com_hdl, TCSANOW, &old_tty);
  close(com_hdl);
}
/* end of hardware specific functions *******************************/


/* GLOBAL FUNCTIONS ****************************************/

int send_cmd(unsigned char *cmd)
{
  int i, error= 0;

  pause(100);

  for (i=1; i<=cmd[0]; i++)
  {
    cxmit(cmd[i]);
  }

  if (crcv(&error) != 0xD1)
    if (dc_type == 0x20)
      error+= ERR_DATA_WRONG;

  return error;
}

int init_dc20(int com_nr, long baud)
{
  int wait= 3;
  int error=0;

  initcom(com_nr, 9600);

  while ( (error=(send_cmd(cmd_init)))!=0 && --wait) ;

  if (error)
    return error;

  switch (baud)
  {
    case  19200: cmd_init[3]= 0x19; cmd_init[4]= 0x20; break;
    case  38400: cmd_init[3]= 0x38; cmd_init[4]= 0x40; break;
    case  57600: cmd_init[3]= 0x57; cmd_init[4]= 0x60; break;
    case 115200: cmd_init[3]= 0x11; cmd_init[4]= 0x52; break;
    default:     cmd_init[3]= 0x96; cmd_init[4]= 0x00; baud= 9600;
  }

  wait= 3;
  while ( (error=(send_cmd(cmd_init)))!=0 && --wait) ;
  if (error)
    return error;

  set_com_baud(baud);

  wait= 3; 
  while ( (error=(send_cmd(cmd_init)))!=0 && --wait) ;

  return error;
}


void close_dc20(void)
{
  /* reset to 9600Baud */
  cmd_init[3]= 0x96; 
  cmd_init[4]= 0x00;
  send_cmd(cmd_init);

  set_com_baud(9600L);
  close_com();
}

int get_status(void)
{
  int wait= 10;
  int error=0, dmy;

  while ( (error=(send_cmd(cmd_init)))!=0 && --wait) ;

  if (error)
    return error;

  wait= 10;
  while ( (error=(send_cmd(cmd_status)))!=0 && --wait) ;
  if (error)
    return error;

  if (read_from_com( 257) != 257)
  {
    return ERR_PACKET_WRONG;
  }  

  cxmit(0xD2);

  crcv(&error);

  dc_type= inp_buff[1];
  if (dc_type == 0x20)
  {
    sts_res= inp_buff[23];
    sts_bat= inp_buff[29];
    sts_pic_cnt= inp_buff[9];
    sts_pic_rem= inp_buff[11];
  }
  else
  {
    sts_res= inp_buff[11];
    sts_bat= inp_buff[29];
    sts_pic_cnt= inp_buff[17] + inp_buff[19];
    if (sts_res == RES_HIGH)
      sts_pic_rem= inp_buff[21];
    else
      sts_pic_rem= inp_buff[23];
  }

  return error;
}

int take_picture(void)
{
  unsigned char rcv;
  unsigned wait= 3;
  int error= 0;

  if ((error= get_status())!=0)
    return error;

  if (sts_pic_rem==0)
    return ERR_IMPOSSIBLE;

  send_cmd(cmd_shot);

  for (;;)
  {
    rcv=crcv(&error);
    if (error)
      break;
  } 

  /* now photo is taken */

  wait= 10;
  error= 0;
  while ( ((rcv=crcv(&error)) != 0) && wait-- )
    error= 0;
  
  if ( (rcv != 0) || error )
  {
    return ERR_NO_RESPONSE;
  }

  /* camera is ready again */

  error= get_status();

  return error;
}

int erase_dc20_memory(void)
{
  char rcv;
  unsigned wait= 30;
  int error= 0;

  if ((error= get_status())!=0)
    return error;

  if (sts_pic_cnt==0)
    return ERR_IMPOSSIBLE;

  if ((error= send_cmd(cmd_erase))!=0)
    return error;

  while ( ((rcv=crcv(&error)) != 0) && wait-- )
    error= 0;

  if ( (rcv != 0) || error )
  {
    return ERR_NO_RESPONSE;
  }

  if ((error= get_status())!=0)
    return error;

  return error;
}

int toggle_resolution(void)
{
  int error= 0;

  if ((error= get_status())!=0)
    return error;

  if (dc_type == 0x25)
    return ERR_NO_DC25_SUPP;

  if (sts_pic_cnt)
    return ERR_IMPOSSIBLE;

  if (sts_res == RES_LOW)
  {
    if ((error= send_cmd(cmd_hires))!=0)
      return error;
  }
  else
  {
    if ((error= send_cmd(cmd_lores))!=0)
      return error;
  }

  error= 0;
  for (;;)
  {
    crcv(&error);
    if (error)
      break;
  } 

  if ((error= get_status())!=0)
    return error;

  return error;
}

int load_thumbnails(FILE *ofp)
{
  int i, blk, error= 0;

  if ((error= get_status())!=0)
    return error;

  if (dc_type == 0x25)
    return ERR_NO_DC25_SUPP;

  if (sts_pic_cnt==0)
    return ERR_IMPOSSIBLE;

  for (i=1; i<= sts_pic_cnt; i++)
  {
    cmd_thumbn[4]= (char) i;

    if ((error= send_cmd(cmd_thumbn))!=0)
      return error;

    for (blk= 0; blk< 5; blk++)
    {
      if (read_from_com( Kb+1) != Kb+1)
        return ERR_PACKET_WRONG;

      if (fwrite(inp_buff, 1, Kb, ofp) != Kb)
        return ERR_DATA_SAVE;

      if (blk%2 == 0)
      { 
        printf(".");
        fflush(stdout);
      }

      cxmit(0xD2);
    }
    crcv(&error);
  }

  return error;
}

int download_picture(int pic_no, FILE *ofp)
{
  int blk, max_blk, error= 0;

  if ((error= get_status())!=0)
    return error;

  if (sts_pic_cnt < pic_no)
    return ERR_IMPOSSIBLE;

  cmd_ldpic2[4]= (char) pic_no;

  if ((error= send_cmd(cmd_ldpic2))!=0)
    return error;

  if (read_from_com( Kb+1) != Kb+1)
    return ERR_PACKET_WRONG;

  if (fwrite(inp_buff, 1, Kb, ofp) != Kb)
    return ERR_DATA_SAVE;

  cxmit(0xD2);

  if (inp_buff[4] == RES_HIGH)
    max_blk= 122;
  else
    max_blk= 61;

  for (blk= 1; blk< max_blk; blk++)
  {
    if (read_from_com( Kb+1) != Kb+1)
      return ERR_PACKET_WRONG;

    if (fwrite(inp_buff, 1, Kb, ofp) != Kb)
      return ERR_DATA_SAVE;

    if (blk%2 == 0)
    { 
      printf(".");
      fflush(stdout);
    }
    
    cxmit(0xD2);
  }

  crcv(&error);

  return error;
}


int load_image_infos(int img_inf[MAX_PICT][5])
{
  int i, error= 0;

  if ((error= get_status())!=0)
    return error;

  for (i=1; i<= sts_pic_cnt; i++)
  {
    if (i > MAX_PICT)
      break;

    cmd_ldpic1[4]= (char) i;

    if ((error= send_cmd(cmd_ldpic1))!=0)
      return error;

    if (read_from_com( 257) != 257)
      return ERR_PACKET_WRONG;

    img_inf[i][0]= inp_buff[16];
    img_inf[i][1]= inp_buff[17];
    img_inf[i][2]= inp_buff[34];
    img_inf[i][3]= inp_buff[35];
    img_inf[i][4]= inp_buff[4];

    cxmit(0xD2);

    crcv(&error);
  }

  return error;
}
