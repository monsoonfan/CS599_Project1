# README - ppmrw.c
# Mitchell Ralston - rmr5

# Application
ppmrw is a simple ppm read/write utility that supports P3/P6/P7 formats
It does support a P7 that has alpha data

usage:  ppmrw 7 input.ppm output.ppm

The above will read input.ppm (3/6/7 formats) and write out P7 format to output.ppm
If the input was P7 with alpha channel data, then a p7 output will also have alpha
channel data. All other outputs will be RGB only.

# Makefile targets of interest
Within the Makefile in this repository are 3 targets you might want to run, details of each
are in the Makefile, but here are short descriptions:


       make test_all_simple (takes simple 15x25 P3/P6/P7 images and writes all 3 formats from each)
       make test_my_image   (the image I created in P3/P6 formats and writes all 3 formats from each)
       make test_alpha      (demonstrates the alpha channel capability by writing 3 formats from a
                             P7 image with alpha, and writes 3 formats from each of the results)

I then use emacs to view all files together:
       emacs *alpha_test.ppm

... and quickly scan through them by killing each buffer as I see it.

Other targets are there for my testing and may not run due to missing files.
