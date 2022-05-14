library:
	gcc -o main mainLib.c -lm

system:
	gcc -o main mainSys.c -lm

test:
	@echo ------------------------system
	@make -s system
	@./main
	@echo ------------------------library
	@make -s library
	@./main
