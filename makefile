trumpjack: main.c main.h libjacktrump.o card.o
	gcc `pkg-config --cflags gtk+-3.0` -o trumpjack main.c main.h libjacktrump.o card.o `pkg-config --libs gtk+-3.0`

libjacktrump.o: libjacktrump.c libjacktrump.h
	gcc -c libjacktrump.c

card.o: card.c card.h
	gcc `pkg-config --cflags gtk+-3.0` -c card.c `pkg-config --libs gtk+-3.0`

clean:
	rm -f *.o
