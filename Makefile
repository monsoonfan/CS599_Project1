#############################################
# Simple makefile for execution of project1
# set path = ($path /cygdrive/c/cygwin64/bin)
#############################################
# all    - compile the project
# test1  - clean, compile and run in simple test config
# run_it - generic target to compile AND run

all: ppmrw.c
	gcc ppmrw.c -o ppmrw

clean:
	rm ppmrw.exe

#test1: clean
test1:
	gcc ppmrw.c -o ppmrw
	./ppmrw 3 int_test.txt out_test.txt

run_it.%:
	gcc $*.c -o $*
	./$*

