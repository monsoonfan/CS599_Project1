#include <stdio.h>
#include <stdlib.h>

// will need a buffer/struct
// typedef struct { .. } Pixel ;
// Pixel *buffer;
// allocate some memory for the buffer
// then write out the image data from buffer

// Git usage tips:
// 1. Even commit history
// 2. Atomic commits (represents a single feature, bug fix, etc...)
// 3. Participation (a bit tricky, because he also likes pair-programming)
// 4. No JUNK! (temp files like .bk, ~ files, object files, IDE config files, compiled exe)
// 5. Definitely add supporting text files, test files, etc... The test image for this
//    project will be an exception to the no-binary files rule
int main() {
  // this is how he writes it but the pointer isn't part of the type unless you typedef it
  //FILE* fh = fopen("filename.ppm","r");
  FILE *fh = fopen("filename.ppm","r");
  // could then use fread
  // could use fgetc to read char by char (int storage space will use
  int c = fgetc(fh); // use to get ascii data, this will be inside of a loop, may need
                     // to multiply get integer, etc and loop until buffer is full
                     // remember, we want to use the image buffer in next assignments
  if (c != 'P') {
    // this prints to STDOUT, but correct convention is to print to STDERR
    // printf("Error: file is not PPM file!\n");
    // fprintf is your friend, will also 
    fprintf(stderr,"Error: file is not PPM file!\n");
  }

  // Another example:
  // Now, open a new FILE and then you can print numbers/chars etc
  FILE *fh2 = fopen("output.example","w");
  unsigned char gray = 30;
  fprintf(fh2, "P6\n");
  fprintf(fh2, "%d " ,gray);
  // fwrite will write binary data

  // Arguments:
  // argc is the count, argv is the array of arguments
}
