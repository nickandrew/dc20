SRC = main.c send_pck.c get_info.c init_dc20.c get_thumb.c read_data.c \
	thumbs_to_file.c get_pic.c pics_to_file.c hash_mark.c erasemem.c \
	snapshot.c wait_till_ready.c toggle_res.c
OBJ = main.o send_pck.o get_info.o init_dc20.o get_thumb.o read_data.o \
	thumbs_to_file.o get_pic.o pics_to_file.o hash_mark.o erasemem.o \
	snapshot.o wait_till_ready.o toggle_res.o

CFLAGS=-Wall

%.o : %.c
	gcc $(CFLAGS) -c -g $<

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
