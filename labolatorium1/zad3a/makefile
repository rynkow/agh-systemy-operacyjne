static: liblibrary.a main.o
	gcc -o main main.o -L ./ -l library -Wall
	
shared: liblibrary.so
	gcc main.c -L ./ -l library -o main -Wl,-rpath,`pwd` -Wall

dynamic: liblibrary.so
	gcc mainDynamic.c -o main -Wl,-rpath,`pwd` -ldl -Wall

main.o:
	gcc -c main.c -o main.o -Wall
	
liblibrary.a:
	gcc -g -c library.c -o library.o -Wall
	ar rcs liblibrary.a library.o
	
liblibrary.so:
	gcc -shared -fPIC -o liblibrary.so library.c -Wall

	
longrun:
	@./main 100 create_table 100 merge_files longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt remove_row 0 0 remove_block 1
	
mediumrun: 
	@./main 50 create_table 50 merge_files longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt  remove_row 0 0 remove_block 1

shortrun:
	@./main 10 create_table 10 merge_files longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt remove_row 0 0 remove_block 1

test:
	@echo ------------------ SHORT
	@make -s shortrun
	@echo ------------------ MEDIUM
	@make -s mediumrun
	@echo ------------------ LONG
	@make -s longrun
	
fulltest:
	@make -s clear
	@make -s static
	@echo ----------STATIC
	@make -s test
	@make -s clear
	@make -s shared
	@echo ----------SHARED
	@make -s test
	@make -s clear
	@make -s dynamic
	@echo ----------DYNAMIC
	@make -s test

clear:
	rm -f *.o
	rm -f main
	rm -f *.a
	rm -f *.so
