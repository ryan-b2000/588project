all:
	gcc -g -O0 -lm -lrt -pthread *.c -o app

clean:
	rm -f app
