include include.mk

build: multicorebsp/mcutil.o multicorebsp/mcinternal.o multicorebsp/mcbsp.o PR_met_BSP.o
	${CC} ${CFLAGS} -o PR_met_BSP PR_met_BSP.o multicorebsp/mcutil.o multicorebsp/mcinternal.o multicorebsp/mcbsp.o -lpthread

