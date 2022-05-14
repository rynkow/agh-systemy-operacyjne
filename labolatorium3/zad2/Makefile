static:
	gcc -g -c library.c -o library.o -Wall
	ar rcs liblibrary.a library.o
	gcc -c main.c -o main.o -Wall -g
	gcc -o main main.o -L ./ -l library -Wall -g

longrun:
	@./main 100 create_table 100 merge_files longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt

mediumrun:
	@./main 50 create_table 50 merge_files longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt

shortrun:
	@./main 10 create_table 10 merge_files longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt longA.txt:longB.txt

test:
	@echo ------------------ SHORT
	@make -s shortrun
	@echo ------------------ MEDIUM
	@make -s mediumrun
	@echo ------------------ LONG
	@make -s longrun



