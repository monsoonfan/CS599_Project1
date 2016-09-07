#############################################
# Simple makefile for execution of project1
# set path = ($path /cygdrive/c/cygwin64/bin)
#############################################
# all    - compile the project
# run_it - compile AND run the project in a default testing configuration

all: ppmrw.c
	gcc ppmrw.c -o ppmrw

clean:
	rm ppmrw.exe

run_it.%:
	gcc $*.c -o $*
	./$*
