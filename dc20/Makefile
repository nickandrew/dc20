SRC = main.c send_pck.c get_info.c init_dc20.c get_thumb.c read_data.c \
	thumbs_to_file.c get_pic.c pics_to_file.c hash_mark.c
OBJ = main.o send_pck.o get_info.o init_dc20.o get_thumb.o read_data.o \
	thumbs_to_file.o get_pic.o pics_to_file.o hash_mark.o

all: dc20 toggle_res cmt2ppm

dc20: $(OBJ)
	cc -o $@ $(OBJ)

toggle_res: toggle_res.o init_dc20.o send_pck.o get_info.o read_data.o
	cc -o $@ toggle_res.o init_dc20.o send_pck.o get_info.o read_data.o

cmt2ppm:	cmt2ppm.c
	cc -o cmt2ppm cmt2ppm.c -lm
clean:
	rm -f $(OBJ) dc20 core

depend:
	makedepend -o.o $(CFLAGS) $(SRC)
# DO NOT DELETE

main.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
main.o: /usr/include/sys/cdefs.h /usr/include/termios.h
main.o: /usr/include/features.h /usr/include/sys/types.h
main.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
main.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
main.o: /usr/include/sys/bitypes.h /usr/include/linux/termios.h
main.o: /usr/include/asm/termios.h /usr/include/asm/termbits.h
main.o: /usr/include/asm/ioctls.h /usr/include/asm/ioctl.h
main.o: /usr/include/fcntl.h /usr/include/gnu/types.h
main.o: /usr/include/linux/fcntl.h /usr/include/asm/fcntl.h dc20.h
get_info.o: dc20.h
init_dc20.o: /usr/include/stdio.h /usr/include/libio.h
init_dc20.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
init_dc20.o: /usr/include/sys/types.h /usr/include/linux/types.h
init_dc20.o: /usr/include/linux/posix_types.h /usr/include/asm/posix_types.h
init_dc20.o: /usr/include/asm/types.h /usr/include/sys/bitypes.h
init_dc20.o: /usr/include/termios.h /usr/include/features.h
init_dc20.o: /usr/include/linux/termios.h /usr/include/asm/termios.h
init_dc20.o: /usr/include/asm/termbits.h /usr/include/asm/ioctls.h
init_dc20.o: /usr/include/asm/ioctl.h /usr/include/fcntl.h
init_dc20.o: /usr/include/gnu/types.h /usr/include/linux/fcntl.h
init_dc20.o: /usr/include/asm/fcntl.h
get_thumb.o: /usr/include/stdio.h /usr/include/libio.h
get_thumb.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
read_data.o: /usr/include/stdio.h /usr/include/libio.h
read_data.o: /usr/include/_G_config.h /usr/include/sys/cdefs.h
thumbs_to_file.o: /usr/include/fcntl.h /usr/include/features.h
thumbs_to_file.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
thumbs_to_file.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
thumbs_to_file.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
thumbs_to_file.o: /usr/include/sys/bitypes.h /usr/include/gnu/types.h
thumbs_to_file.o: /usr/include/linux/fcntl.h /usr/include/asm/fcntl.h
get_pic.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
get_pic.o: /usr/include/sys/cdefs.h
pics_to_file.o: /usr/include/fcntl.h /usr/include/features.h
pics_to_file.o: /usr/include/sys/cdefs.h /usr/include/sys/types.h
pics_to_file.o: /usr/include/linux/types.h /usr/include/linux/posix_types.h
pics_to_file.o: /usr/include/asm/posix_types.h /usr/include/asm/types.h
pics_to_file.o: /usr/include/sys/bitypes.h /usr/include/gnu/types.h
pics_to_file.o: /usr/include/linux/fcntl.h /usr/include/asm/fcntl.h
