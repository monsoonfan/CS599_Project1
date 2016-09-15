#############################################
# Simple makefile for execution of project1
# set path = ($path /cygdrive/c/cygwin64/bin)
#############################################
# all    - compile the project
# test1  - clean, compile and run in simple test config
# run_it - generic target to compile AND run

all: ppmrw.c
	gcc ppmrw.c -o ppmrw

# assuming VERBOSE is set to 1 in ppmrw.c
verbose: ppmrw.c
	gcc ppmrw.c -o ppmrw_v

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

make test_all:
	gcc ppmrw.c -o ppmrw
	./ppmrw 3 p3test.ppm p3out3_test.ppm
	./ppmrw 6 p3test.ppm p6out3_test.ppm
	./ppmrw 7 p3test.ppm p7out3_test.ppm

	./ppmrw 3 p6test.ppm p3out6_test.ppm
	./ppmrw 6 p6test.ppm p6out6_test.ppm
	./ppmrw 7 p6test.ppm p7out6_test.ppm

	./ppmrw 3 p7test.ppm p3out7_test.ppm
	./ppmrw 6 p7test.ppm p6out7_test.ppm
	./ppmrw 7 p7test.ppm p7out7_test.ppm

make test_checker_all:
	gcc ppmrw.c -o ppmrw
	./ppmrw 3 p3checker_noalpha.ppm p3out3_test.ppm
	./ppmrw 6 p3checker_noalpha.ppm p6out3_test.ppm
	./ppmrw 7 p3checker_noalpha.ppm p7out3_test.ppm

	./ppmrw 3 p6checker_noalpha.ppm p3out6_test.ppm
	./ppmrw 6 p6checker_noalpha.ppm p6out6_test.ppm
	./ppmrw 7 p6checker_noalpha.ppm p7out6_test.ppm

	./ppmrw 3 p7checker_noalpha.ppm p3out7_test.ppm
	./ppmrw 6 p7checker_noalpha.ppm p6out7_test.ppm
	./ppmrw 7 p7checker_noalpha.ppm p7out7_test.ppm


run_it.%:
	gcc $*.c -o $*
	./$*

