/*
---------------------------------------------------------------------------------------
CS599 Project 1
R Mitchell Ralston (rmr5)
9/6/16
--------------------------
Implementation of Project1 as per assignment spec on BBLearn

Objectives of the for ppmrw.c:
----------------------------------------
1) read PPM images format P3, P6, P7 into memory
2) write PPM images format P3, P6, P7 from memory
3) check for the following errors and return non-zero on error
   - non-existent file
   - file not correct format (PPM)

Organization:
------------
- This code is organized into functions, with an eye to the future in hopes to use these functions later
- Would have included these functions in the form of a myLib.h, but my understanding is the assignment forbids such libraries
- The use of global variables will be employed to ease implementation
- functions have camelCase naming convention, variables have "_" based naming

Workflow:
--------
1) perform error checking on arguments
2) perform error checking on input file
3) read input file into memory
4) write output file
5) perform error checking on output file
   - 8 bits per channel
6) report statistics

Functions:
----------
  readFile     - read the PPM file
  writePPM     - write the PPM file
  checkInput   - check the input PPM file for any known errors
  checkOutput  - check the output file to make sure it was written (including 8-bits per channel/RGBA)
  message      - print a message to stdout (can be Info, Warn, Error)
  help         - print help message, usage, etc...

Questions:
1) best way to structure data such as pass-by-reference in C? Seems C is pass-by-value only
   Is this passing the address or a copy of the data?
---------------------------------------------------------------------------------------
*/

// libraries
#include <stdio.h>
#include <stdlib.h>

// global variables
typedef struct Pixel {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} Pixel ;

typedef struct PPM_file_struct {
  char magic_number;
  int lines;
  int width;
  int height;
  Pixel *pixel_map;
  FILE* fh_in;
} PPM_file_struct ;

// functions
//void scanFile (PPM_file_struct *infile);
void scanFile      (char *infile,      PPM_file_struct *input                         );
void readFile      (char *charaddress                                                 );
void convertFormat (int   format,      PPM_file_struct *input, PPM_file_struct *output);
void writeFile     (char *outfile,     PPM_file_struct *input                         );
void help ();

/*
  MAIN
*/
int main(int argc, char *argv[]) {

  // Argument checking done in main, as it's specific to this project
  // format: ppmrw 3 input.ppm output.ppm
  if (argc != 4) {
    help();
    return(1);
  }

  // Get parameters from arguments
  int output_magic_number = atoi(argv[1]);
  char *infile = argv[2];
  char *outfile = argv[3];
  printf("Converting file to file format %d ...\n",output_magic_number);
  printf("    Input : %s\n",infile);
  printf("    Output: %s\n",outfile);

  // Open the input file and traverse it, check its format and store height/width only
  PPM_file_struct input_file_data;
  scanFile(infile,&input_file_data);

  // Now actually read the contents of the file and store into buffer
  char *testchar;
  readFile(testchar);

  // Make the conversion
  PPM_file_struct output_file_data;
  convertFormat(output_magic_number,&input_file_data,&output_file_data);

  // Write the contents of the new file in desired format, remember to use pointer arithmetic to traverse
  writeFile(outfile,&output_file_data);

  // Verify correct output

  // Successful exit was reached
  return EXIT_SUCCESS;
}


/*
  FUNCTION IMPLEMENTATIONS
*/
void help () {
  printf("Error: Invalid arguments!\n");
  printf("Usage: ppmrw 3 input.ppm output.ppm\n");
}

// simply scan the file to initialize the data structure associated with it, finding it's width/height
// so that we can malloc most efficiently
void scanFile (char *infile, PPM_file_struct *input) {
  input->fh_in = fopen(infile,"r");
  printf("Scanning...\n");
  char current_char = getc(input->fh_in);
  while(current_char != EOF) {
    printf("Processing char: %c\n",current_char);
    fprintf(stdout, "(%c)", current_char);
    //    fprintf(fh_out, "%c", current_char);
    current_char = getc(input->fh_in);
  }
  fclose(input->fh_in);
}

// actually read the contents of the file and store them in memory
void readFile (char *charaddress) {
  *charaddress = 'z';
  printf("Reading... %c\n",*charaddress);
}

// convert from input format to output format
void convertFormat (int format, PPM_file_struct *input, PPM_file_struct *output) {
  //  switch(format):
  printf("Converting to format %d\n",format);
}

// write the output file in the new format
void writeFile (char *outfile, PPM_file_struct *input) {
  printf("Writing file %s...\n",outfile);
  FILE* fh_out = fopen(outfile,"wb");
  fclose(fh_out);
  printf("Done");
}
