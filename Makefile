CC=gcc -ansi -std=c99 -I./include

build: src/PR_met_BSP.c
	${CC} -o PageRank src/PR_met_BSP.c lib/libmcbsp1.1.0.a -pthread -lrt
	rm -f PR_met_BSP.o

clean:
	rm -f PageRank
