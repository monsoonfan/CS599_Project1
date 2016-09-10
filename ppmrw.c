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
- Will use camelCase variable and function conventions for this project
- The use of global variables will be employed to ease implementation

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
---------------------------------------------------------------------------------------
*/

// libraries
#include <stdio.h>
#include <stdlib.h>

// global variables

// functions
int readFile ();
void help ();

/*
  MAIN
*/
int main(int argc, char *argv[]) {

  // Argument checking
  // format: ppmrw 3 input.ppm output.ppm
  printf("DBG: %d\n",argc);
  printf("DBG: %s\n",argv[0]);
  if (argc != 4) {
    help();
    return(1);
  }

  // Get parameters from arguments
  int inputMagicNumber = atoi(argv[1]);
  printf("Converting file to file format %d ...\n",inputMagicNumber);
  printf("    Input : %s\n",argv[2]);
  printf("    Output: %s\n",argv[3]);

  // Open the input file and traverse it, write to the output file
  FILE* fh_in = fopen(argv[2],"r");
  FILE* fh_out = fopen(argv[3],"wb");
  char current_char = getc(fh_in);
  while(current_char != EOF) {
    printf("Processing char: %c\n",current_char);
    fprintf(stdout, "(%c)", current_char);
    fprintf(fh_out, "%c", current_char);
    current_char = getc(fh_in);
  }
  fclose(fh_in);
  fclose(fh_out);

  // Successful exit was reached
  return 0;
}

/*
  FUNCTION IMPLEMENTATIONS
*/
void help () {
  printf("Error: Invalid arguments!\n");
  printf("Usage: ppmrw 3 input.ppm output.ppm\n");
}
