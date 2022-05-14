library:
	gcc -o main mainLib.c

system:
	gcc -o main mainSys.c

test:
	@echo ------------------------system
	@make -s system
	@./main ../../eng.txt out.txt
	@echo ------------------------library
	@make -s library
	@./main ../../eng.txt out.txt
