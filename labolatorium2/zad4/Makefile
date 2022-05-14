library:
	gcc -o main mainLib.c

system:
	gcc -o main mainSys.c

test:
	@echo ------------------------system
	@make -s system
	@./main
	@echo ------------------------library
	@make -s library
	@./main
