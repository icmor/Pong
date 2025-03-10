all: compile run

run: pong
	./pong

compile: pong.c
	gcc -o pong pong.c -lGL -lglut -lGLU -lm
