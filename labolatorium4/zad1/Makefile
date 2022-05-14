fork:
	make clear
	gcc -o main -Wall mainFork.c

mexec:
	make clear
	gcc -o exec -Wall exec.c
	gcc -o main -Wall mainExec.c

testFork:
	make -s fork
	./main ignore
	./main handler
	./main mask
	./main pending

testExec:
	make -s mexec
	./main ignore
	./main handler
	./main mask
	./main pending

test:
	@echo ---------FORK
	@make testFork
	@echo ---------EXEC
	@make testExec

clear:
	rm exec
	rm main
