PROG=main
CC=gcc
FLAGS=-Wno-deprecated -DGLFW_INCLUDE_GLU -lglfw -framework OpenGL

.DEFAULT_GOAL := run

${PROG}: ${PROG}.c
	${CC} ${PROG}.c -o ${PROG} ${FLAGS}

run: ${PROG}
	./${PROG}

.PHONY: clean
clean:
	rm -f $(obj) main
