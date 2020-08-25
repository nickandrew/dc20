SRC = main.c send_pck.c get_info.c init_dc20.c get_thumb.c read_data.c \
	thumbs_to_file.c get_pic.c pics_to_file.c hash_mark.c erasemem.c \
	snapshot.c wait_till_ready.c toggle_res.c
OBJ = main.o send_pck.o get_info.o init_dc20.o get_thumb.o read_data.o \
	thumbs_to_file.o get_pic.o pics_to_file.o hash_mark.o erasemem.o \
	snapshot.o wait_till_ready.o toggle_res.o

%.o : %.c
	gcc -c -g $<

all: dc20bin cmttoppm cmt2ppm

cmttoppm: cmttoppm.o
	cc -o $@ $@.o -lm

cmt2ppm: cmt2ppm.o
	cc -o $@ $@.o -lm

dc20bin: $(OBJ)
	cc -o $@ $(OBJ)

clean:
	rm -f *.o dc20bin cmttoppm cmt2ppm core *.cmt *.jpg *.pgm *~

depend:
	makedepend -o.o $(CFLAGS) $(SRC)

install:
	cp dc20     /usr/bin
	cp dc20bin  /usr/bin
	cp cmttoppm /usr/bin
	cp cmt2ppm  /usr/bin

rpm: clean
	cp dc20.gif /usr/src/redhat/SOURCES
	cp Specfile /usr/src/redhat/SPECS/dc20.spec
	(cd ..; tar cfz /usr/src/redhat/SOURCES/dc20.tgz dc20)
	(cd /usr/src/redhat/SPECS;rpm -ba dc20.spec)

# DO NOT DELETE

main.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
main.o: /usr/include/_G_config.h /usr/include/gnu/types.h
main.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stddef.h
main.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stdarg.h
main.o: /usr/include/stdio_lim.h /usr/include/termios.h
main.o: /usr/include/termbits.h /usr/include/sys/ttydefaults.h
main.o: /usr/include/fcntl.h /usr/include/fcntlbits.h
main.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
main.o: /usr/include/bytesex.h /usr/include/sys/select.h
main.o: /usr/include/selectbits.h dc20.h
get_info.o: dc20.h
init_dc20.o: /usr/include/stdio.h /usr/include/libio.h
init_dc20.o: /usr/include/features.h /usr/include/sys/cdefs.h
init_dc20.o: /usr/include/gnu/stubs.h /usr/include/_G_config.h
init_dc20.o: /usr/include/gnu/types.h
init_dc20.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stddef.h
init_dc20.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stdarg.h
init_dc20.o: /usr/include/stdio_lim.h /usr/include/sys/types.h
init_dc20.o: /usr/include/time.h /usr/include/endian.h /usr/include/bytesex.h
init_dc20.o: /usr/include/sys/select.h /usr/include/selectbits.h
init_dc20.o: /usr/include/termios.h /usr/include/termbits.h
init_dc20.o: /usr/include/sys/ttydefaults.h /usr/include/fcntl.h
init_dc20.o: /usr/include/fcntlbits.h
get_thumb.o: /usr/include/stdio.h /usr/include/libio.h
get_thumb.o: /usr/include/features.h /usr/include/sys/cdefs.h
get_thumb.o: /usr/include/gnu/stubs.h /usr/include/_G_config.h
get_thumb.o: /usr/include/gnu/types.h
get_thumb.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stddef.h
get_thumb.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stdarg.h
get_thumb.o: /usr/include/stdio_lim.h
read_data.o: /usr/include/stdio.h /usr/include/libio.h
read_data.o: /usr/include/features.h /usr/include/sys/cdefs.h
read_data.o: /usr/include/gnu/stubs.h /usr/include/_G_config.h
read_data.o: /usr/include/gnu/types.h
read_data.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stddef.h
read_data.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stdarg.h
read_data.o: /usr/include/stdio_lim.h
thumbs_to_file.o: /usr/include/fcntl.h /usr/include/features.h
thumbs_to_file.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
thumbs_to_file.o: /usr/include/fcntlbits.h /usr/include/sys/types.h
thumbs_to_file.o: /usr/include/gnu/types.h /usr/include/time.h
thumbs_to_file.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stddef.h
thumbs_to_file.o: /usr/include/endian.h /usr/include/bytesex.h
thumbs_to_file.o: /usr/include/sys/select.h /usr/include/selectbits.h
get_pic.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/features.h
get_pic.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
get_pic.o: /usr/include/_G_config.h /usr/include/gnu/types.h
get_pic.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stddef.h
get_pic.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stdarg.h
get_pic.o: /usr/include/stdio_lim.h
pics_to_file.o: /usr/include/unistd.h /usr/include/features.h
pics_to_file.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
pics_to_file.o: /usr/include/posix_opt.h /usr/include/gnu/types.h
pics_to_file.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stddef.h
pics_to_file.o: /usr/include/confname.h /usr/include/fcntl.h
pics_to_file.o: /usr/include/fcntlbits.h /usr/include/sys/types.h
pics_to_file.o: /usr/include/time.h /usr/include/endian.h
pics_to_file.o: /usr/include/bytesex.h /usr/include/sys/select.h
pics_to_file.o: /usr/include/selectbits.h
hash_mark.o: /usr/include/stdio.h /usr/include/libio.h
hash_mark.o: /usr/include/features.h /usr/include/sys/cdefs.h
hash_mark.o: /usr/include/gnu/stubs.h /usr/include/_G_config.h
hash_mark.o: /usr/include/gnu/types.h
hash_mark.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stddef.h
hash_mark.o: /usr/lib/gcc-lib/i386-redhat-linux/2.7.2.3/include/stdarg.h
hash_mark.o: /usr/include/stdio_lim.h
