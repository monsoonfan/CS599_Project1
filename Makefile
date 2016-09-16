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

#############################################
# targets for testing each of the simple test configs
#############################################

# test_my_image
# - takes my created
test_my_image:
	gcc ppmrw.c -o ppmrw
	./ppmrw 3 my_image_p3.ppm p3_from_p3_my_image.ppm
	./ppmrw 6 my_image_p3.ppm p6_from_p3_my_image.ppm
	./ppmrw 7 my_image_p3.ppm p7_from_p3_my_image.ppm

	./ppmrw 3 my_image_p6.ppm p3_from_p6_my_image.ppm
	./ppmrw 6 my_image_p6.ppm p6_from_p6_my_image.ppm
	./ppmrw 7 my_image_p6.ppm p7_from_p6_my_image.ppm
	echo "*** Done with test_my_image, try 'ls -lrt *_my_image.ppm' to list files ***"


# test_alpha
# - takes a 500x600 checkerboard image from the pixmap.c program that Dr. P demonstrated in class that has
#   been placed into a P7 file, writes all 3 formats from it (the P7 image will have alpha), and then
#   writes all 3 formats from the resultant P3 and P6 formats (these P7 images built from P3/6 will not have alpha)
test_alpha:
	gcc ppmrw.c -o ppmrw
	./ppmrw 3 p7checker_alpha.ppm p3out_from_P7alpha_test.ppm
	./ppmrw 6 p7checker_alpha.ppm p6out_from_P7alpha_test.ppm
	./ppmrw 7 p7checker_alpha.ppm p7out_from_P7alpha_test.ppm

	./ppmrw 3 p3out_from_P7alpha_test.ppm p3out_from_P3alpha_test.ppm
	./ppmrw 6 p3out_from_P7alpha_test.ppm p6out_from_P3alpha_test.ppm
	./ppmrw 7 p3out_from_P7alpha_test.ppm p6out_from_P3alpha_test.ppm

	./ppmrw 3 p6out_from_P7alpha_test.ppm p3out_from_P6alpha_test.ppm
	./ppmrw 6 p6out_from_P7alpha_test.ppm p6out_from_P6alpha_test.ppm
	./ppmrw 7 p6out_from_P7alpha_test.ppm p7out_from_P6alpha_test.ppm
	echo "*** Done with test_alpha, try 'ls -lrt *alpha_test.ppm' to list files ***"

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

make test_all_simple:
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
	echo "*** Done with test_all_simple, try 'ls -lrt *[367]_test.ppm' to list files ***"

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

	./ppmrw 7 p7checker_alpha.ppm p7outalpha7_test.ppm


run_it.%:
	gcc $*.c -o $*
	./$*

