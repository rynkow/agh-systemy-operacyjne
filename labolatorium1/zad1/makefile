static:
	make clear
	gcc -g -c library.c -o library.o -Wall
	ar rcs liblibrary.a library.o
	gcc -c main.c -o main.o -Wall
	gcc -o main main.o -L ./ -l library -Wall
	
shared:
	make clear
	gcc -shared -fPIC -o liblibrary.so library.c -Wall
	gcc main.c -L ./ -l library -o main -Wl,-rpath,`pwd` -Wall


run:
	./main 2 create_table 2 merge_files a.txt:b.txt a.txt:b.txt remove_row 1 1 remove_row 2 1 remove_block 1
	
longrun:
	./main 100 create_table 100 merge_files longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt remove_row 0 0 remove_block 1
	
clear:
	rm -f *.o
	rm -f main
	rm -f *.a
	rm -f *.so
