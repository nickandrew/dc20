/* GLOBAL DEFINES *****************************************/
#define ERR_NO_DATA	   1
#define ERR_DATA_WRONG	   2
#define ERR_NO_CAMERA	   3
#define ERR_PACKET_WRONG   4
#define ERR_NO_RESPONSE    8
#define ERR_IMPOSSIBLE    16
#define ERR_DATA_SAVE     32
#define ERR_USER_BREAK    64
#define ERR_NO_DC25_SUPP 128

#define RES_HIGH   0
#define RES_LOW    1

#define  Kb  1024
#define  INP_BUFF_SIZE  ((unsigned) Kb+1)

#define MAX_PICT  32

/* GLOBAL VARIABLES ***************************************/
extern unsigned char inp_buff[INP_BUFF_SIZE];
extern unsigned char sts_res, sts_bat, sts_pic_cnt, sts_pic_rem;
extern unsigned char dc_type;
extern unsigned char com_dev[];

/* GLOBAL FUNCTIONS ****************************************/
extern int send_cmd(unsigned char *cmd);

extern int init_dc20(int com_nr, long baud);
extern void close_dc20(void);

extern int get_status(void);

extern int take_picture(void);

extern int erase_dc20_memory(void);

extern int toggle_resolution(void);

extern int load_thumbnails(FILE *ofp);

extern int download_picture(int pic_no, FILE *ofp);

extern int load_image_infos(int img_inf[MAX_PICT][5]);

