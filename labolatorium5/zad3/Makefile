compile:
	gcc -o consumer consumer.c -Wall
	gcc -o producent producent.c -Wall

test:
	make compile
	./producent pipe 0 prodFile1.txt 2 &
	./producent pipe 1 prodFile2.txt 2 &
	./producent pipe 2 prodFile3.txt 2 &
	./consumer pipe output.txt 2