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

# targets for testing each of the simple test configs
test3to3:
	gcc ppmrw.c -o ppmrw
	./ppmrw 3 p3test.ppm p3out_test.ppm

test3to6:
	gcc ppmrw.c -o ppmrw
	./ppmrw 6 p3test.ppm p6out_test.ppm

test3to7:
	gcc ppmrw.c -o ppmrw
	./ppmrw 7 p3test.ppm p7out_test.ppm

test6to3:
	gcc ppmrw.c -o ppmrw
	./ppmrw 3 p6test.ppm p3out_test.ppm

test6to6:
	gcc ppmrw.c -o ppmrw
	./ppmrw 6 p6test.ppm p6out_test.ppm

test6to7:
	gcc ppmrw.c -o ppmrw
	./ppmrw 7 p6test.ppm p7out_test.ppm

test7to3:
	gcc ppmrw.c -o ppmrw
	./ppmrw 3 p7test.ppm p3out_test.ppm

test7to6:
	gcc ppmrw.c -o ppmrw
	./ppmrw 6 p7test.ppm p6out_test.ppm

test7to7:
	gcc ppmrw.c -o ppmrw
	./ppmrw 7 p7test.ppm p7out_test.ppm


run_it.%:
	gcc $*.c -o $*
	./$*

