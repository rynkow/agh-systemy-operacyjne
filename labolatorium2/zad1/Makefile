library:
	gcc -o main mainLib.c

system:
	gcc -o main mainSys.c

test:
	@echo ------------------------system
	@make -s system
	@./main ../../latin.txt ../../eng.txt
	@echo ------------------------library
	@make -s library
	@./main ../../latin.txt ../../eng.txt

testS:
	@make -s system
	@./main ../../latin.txt ../../eng.txt

testL:
	@make -s library
	@./main ../../latin.txt ../../eng.txt