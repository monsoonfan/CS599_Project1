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
  readPPM      - read the PPM file (including checking that its a valid PPM)
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
  int alpha;
  Pixel *pixel_map; // this is a problem, would need to dynamically malloc this thing to be efficient
  FILE* fh_in;
} PPM_file_struct ;

// functions
//void readPPM (PPM_file_struct *infile);
int  readPPM      (char *infile,      PPM_file_struct *input                          );
void convertFormat (int   format,      PPM_file_struct *input, PPM_file_struct *output);
void writeFile     (char *outfile,     PPM_file_struct *input                         );
void message       (char message_code[],char message[]          );
//void message (char channel[], char message_code[], char message[]);
void reportPPMStruct (PPM_file_struct *input);
void help ();
char getChunk (char current, char *chunk, PPM_file_struct *input);
char advanceToChunk (char current, PPM_file_struct *input);

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
  if (!readPPM(infile,&input_file_data)) {return 0;}

  // TMP DBG: report the struct
  reportPPMStruct(&input_file_data);

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
  ------------------------
  FUNCTION IMPLEMENTATIONS
  ------------------------
*/

/*
  --- message ---
  - 9/10/16
  - rmr5
  ---------------
  print a message to stdout consisting of a message code and a message to a given channel
  current valid channels to write to (stdout, stderr, etc) - will add fh later
  //void message (char channel[], char message_code[], char message[]) {
*/
void message (char message_code[], char message[]) {
  printf("%s: %s\n",message_code,message);
  //fprintf(stdout,"%s: %s\n",message_code,message);
  //  fprintf(channel,"%s: %s\n",message_code,message);
}

void help () {
  message("Error","Invalid arguments!");
  message("Usage","ppmrw 3 input.ppm output.ppm");
}

/*
  --- readPPM ---
  - 9/10/16
  - rmr5
  ----------------
  read the file to initialize the data structure associated with it, finding its width/height
  and use that to malloc efficiently, do any error checking that its a valid PPM file

  Here is the spec used (from sourceforge.net/doc/ppm.html:
  1) A "magic number" for identifying the file type. A ppm image's magic number is the two characters "P6".
  2) Whitespace (blanks, TABs, CRs, LFs).
  *  Also have to deal with comment lines! This spec says nothing of them.
  3) A width, formatted as ASCII characters in decimal.
  4) Whitespace.
  5) A height, again in ASCII decimal.
  6) Whitespace.
  7) The maximum color value (Maxval), again in ASCII decimal. Must be less than 65536 and more than zero.
  8) A single whitespace character (usually a newline).
  9) A raster of Height rows, in order from top to bottom. Each row consists of Width pixels, 
     in order from left to right. Each pixel is a triplet of red, green, and blue samples, in that order. 
     Each sample is represented in pure binary by either 1 or 2 bytes. If the Maxval is less than 256, it
     is 1 byte. Otherwise, it is 2 bytes. The most significant byte is first.
  ----------------
*/
int readPPM (char *infile, PPM_file_struct *input) {
  // opening message
  printf("Scanning %s for info ...\n",infile);
  input->fh_in = fopen(infile,"r");

  // variables for this func
  int first_char = getc(input->fh_in);
  int current_char = 'a';
  int prev_char = '\n';
  int linecount = 0;
  int charcount = 0;
  
  // ------------------------------- HEADER --------------------------------
  // First char should always be a P (else exit), if so next char is number and advance from there
  if (first_char != 'P') {
    message("Error","File format not recognized!");
    return EXIT_FAILURE;
  } else {
    input->magic_number = fgetc(input->fh_in);
    // should be a newline, but doesn't have to be, so advance to end of first line until we hit one
    while(fgetc(input->fh_in) != '\n');
    linecount++;
  }
  // Done with first line
  printf("DBG: Done with the first line\n");

  // Now traverse any comment lines in one loop
  current_char = fgetc(input->fh_in);
  int done_with_comments = 0;
  while(current_char != EOF) {
    charcount++;
    // Deal with the comment lines by advancing until we hit a new line where first char is not #
    if(!done_with_comments) { // do this if only once
      if(current_char == '#') {
	while(!(prev_char == '\n' && current_char != '#')) {
	  prev_char = current_char;
	  current_char = fgetc(input->fh_in);
	  if(current_char == '\n') {linecount++;}
	}
      } else {
	message("Warning","No comment lines detected, please double-check file");
      }
      printf("DBG: done with comments on line %d\n",linecount);
      done_with_comments = 1;
      //printf("DBG: advanced one char, prev: %c current: %c\n",prev_char,current_char);
    }
    
    // Now get the width/height, format checking as part of this
    // idea -  isspace(),isblank() on fgetc result?
    char *temp_chars = malloc(sizeof(char)*99);
    prev_char = getChunk(current_char,temp_chars,input);
    printf("DBG: Exited cc = (%c)\n",prev_char);

    // convert temp_chars index into int to store
    //    input->width = atoi(temp_chars);
    input->width = temp_chars[0];
    printf("DBG: done with width, stored %d\n",input->width);

    // Now height
    char *temp2 = malloc(sizeof(char)*99);
    prev_char = advanceToChunk(prev_char,input);
    prev_char = getChunk(prev_char,temp2,input);
    printf("DBG: Exited cc2 = (%c)\n",prev_char);
    input->height = temp2[0];
    printf("DBG: storing width/height %d,%d\n",input->width,input->height);
    
    // Need the alpha channel next
    // If the Maxval is less than 256, it is 1 byte. Otherwise, it is 2 bytes. The most significant byte is first.
    char *temp3 = malloc(sizeof(char)*99);
    prev_char = advanceToChunk(prev_char,input);
    prev_char = getChunk(prev_char,temp3,input);
    input->alpha = temp3[0];
    printf("DBG: storing alpha %d\n",input->alpha);
    
    // TODO: clean this mess up
    for(int i = 0;i < 5; i++) {
      printf("DBGf: tc[%d]: %c< t2[%d]: %c< %c<\n",i,temp_chars[i],i,temp2[i],temp3[i]);
    }
    free(temp_chars);
    free(temp2);
    message("Info","Completed processing header information.");
    break;
  }

  // ------------------------------- END HEADER ----------------------------

  // ------------------------------- BEGIN IMAGE ----------------------------
  // To read the raster/buffer info:
  // need a case statement to deal with 3/6/7 formats separately
  // start reading chars into an array until whitespace, then increment the column , store to pixmap, do it again+
  // continue until \n, then increment row , column back to 0 repeat 1) above
  // continue until EOF
  // fgets(buffer,BUFFSIZE,exif); <-- could be useful, although with varying sizes of info don't see how
  message("Info","Process image information...");
  input->pixel_map = malloc(sizeof(Pixel) * input->width * input->height);
  char *temp_data = malloc(sizeof(char) * 3); //255 will be highest value
  int chunk_count = 0;
  switch((int)input->magic_number) {
  case('3'):
    message("Info","  format version: 3");
    while(prev_char != EOF) {
      prev_char = advanceToChunk(prev_char,input);
      prev_char = getChunk(prev_char,temp_data,input);
      // convert chunk
      //      input.pixel_map.r = temp3[0];
      //      printf("  stored %c\n",input.pixel_map.r);
      // store chunk
      chunk_count++;
    }
    chunk_count--; // last chunk is ignored, it's EOF
    printf("read %d chunks\n",chunk_count);
    message("Info","Done reading PPM 3");
    break;
  case('6'):
    message("Info","  format version: 6");
    while(prev_char != EOF) {
      prev_char = advanceToChunk(prev_char,input);
      prev_char = getChunk(prev_char,temp_data,input);
      chunk_count++;
    }
    break;
  case('7'):
    message("Info","  format version: 7");
    while(prev_char != EOF) {
      prev_char = advanceToChunk(prev_char,input);
      prev_char = getChunk(prev_char,temp_data,input);
      chunk_count++;
    }
    break;
  }
  
  // ------------------------------- END IMAGE ------------------------------

  fclose(input->fh_in);
  return input->magic_number;
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
  message("Info","Done");
}

// helper function to visualize what's in a given PPM struct
void reportPPMStruct (PPM_file_struct *input) {
  message("Info","Contents of PPM struct:");
  printf("     magic_number: %d\n",input->magic_number);
  printf("     width:        %d\n",input->width);
  printf("     height:       %d\n",input->height);
  printf("     alpha:        %d\n",input->alpha);
}

// meant to be used with advanceToChunk function that basically skips whitespace
char getChunk (char current, char *chunk, PPM_file_struct *input) {
  // if you do a getChunk from current, that's different from getChunk so manage it
  // Yes, always pass in a current character, and return the new current character (which is prev char)
    int tc_index = 0;
    chunk[tc_index] = current;
    
    do {
      printf("DBG_gC: current (%d)%c\n",tc_index,current);
      current = fgetc(input->fh_in);
      chunk[++tc_index] = current;
      // fail safe
      if(tc_index > 99) {
	message("Error","File format error, more chars than expected without whitespace");
	return '~'; // return something that shouldnt be found it the file
      }
    } while(current != ' ' && current != '\n' && current != EOF) ;
    return current;
}

char advanceToChunk (char current, PPM_file_struct *input) {
  while(current == ' ' || current == '\n') {
    printf("DBG aTC: skipping (%c)\n",current);
    current = fgetc(input->fh_in);
  }
  return current;
}

/*
  OLD CODE for reference

// working code for traversing file char by char
  char current_char = getc(input->fh_in);
  while(current_char != EOF) {
    printf("Processing char: %c\n",current_char);
    fprintf(stdout, "(%c)", current_char);
    //    fprintf(fh_out, "%c", current_char);
    current_char = getc(input->fh_in);
  }
*/
