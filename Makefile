all:
	gcc -g -O0 -pthread *.c -o app

clean:
	rm -f app
