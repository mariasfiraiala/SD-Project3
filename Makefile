all: build

build:
	gcc main.c tree.c -o sd_fs -g

clean:
	rm *.o sd_fs

run:
	./sd_fs
