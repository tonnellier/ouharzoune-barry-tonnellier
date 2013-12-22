mdazi: main.c main.h norme.h modif-dazibao.o
	gcc main.c -o mdazi -Wall

modif-dazibao.o: modif-dazibao.c
	gcc -c modif-dazibao.c -Wall

clean:
	rm mdazi
	rm modif-dazibao.o