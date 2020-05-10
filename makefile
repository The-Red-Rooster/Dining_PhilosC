all: assignment5

assignment5: random.o assignment5.o
			gcc assignment5.o random.o -o assignment5 -lm

random.o: random.c random.h
			gcc -c random.c -o random.o

assignment5.o: assignment5.c
			gcc -c assignment5.c -o assignment5.o

run:
			./assignment5

clean:
			rm assignment5.o random.o assignment5
