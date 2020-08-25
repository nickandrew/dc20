all: dc20term dc2totga
 
CFLAGS=-Wall

dc20term: dc20_ui.o dc20_hif.o
	gcc -o $@ dc20_ui.o dc20_hif.o

dc2totga: dc2totga.c
	gcc -O3 -o dc2totga dc2totga.c -lm

clean:
	rm -f dc20_ui.o dc20_hif.o dc2totga.o dc20term dc2totga

depend:
	makedepend -o.o $(CFLAGS) $(SRC)
