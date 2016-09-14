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
---------------------------------------------------------------------------------------
*/

// libraries
#include <stdio.h>
#include <stdlib.h>

// global variables
int VERBOSE = 0; // controls logfile message level
int CURRENT_CHAR = 'a';
int PREV_CHAR = '\n';

typedef struct Pixel {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} Pixel ;

Pixel *pixel_map;

typedef struct PPM_file_struct {
  char magic_number;
  int lines;
  int width;
  int height;
  int alpha;
  //Pixel *pixel_map; // this is a problem, would need to dynamically malloc this thing to be efficient
  FILE* fh_in;
} PPM_file_struct ;

// functions
//void readPPM (PPM_file_struct *infile);
int  readPPM       (char *infile,      PPM_file_struct *input                          );
void convertFormat (int   format,      PPM_file_struct *input, PPM_file_struct *output );
void writePPM      (char *outfile,     PPM_file_struct *input                          );
void message       (char message_code[],char message[]                                 );
//void message (char channel[], char message_code[], char message[]);
void reportPPMStruct (PPM_file_struct *input);
void reportPixelMap (Pixel *pm);
void help ();
int getChunk (PPM_file_struct *input);
void advanceToChunk (PPM_file_struct *input);

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
  
  // Open the input file and traverse it, storing the image to buffer
  PPM_file_struct input_file_data;
  readPPM(infile,&input_file_data);
  
  // TMP DBG: report the struct
  reportPPMStruct(&input_file_data);
  
  // Make the conversion
  // TODO: ditch this proc, do the conversion during writePPM only
  PPM_file_struct output_file_data;
  convertFormat(output_magic_number,&input_file_data,&output_file_data);

  // Write the contents of the new file in desired format, remember to use pointer arithmetic to traverse
  writePPM(outfile,&output_file_data);

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
  if(message_code == "Error") {
    fprintf(stderr,"%s: %s\n",message_code,message);
  } else {
    printf("%s: %s\n",message_code,message);
  }
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

suggested to get bytes at a time, ??
3 = fscanf
6 = fread
6 = fwrite

P7 = header is more free-form and has an end-of-header directive
custom tuple types, but we just need to be able to do RGB and RBGA, anything else, just 
return Error unsupported type
we can assume just one byte for this project, but throw an error if it's more than one
but you better check for anything that could go wrong, each line of code, throw an error to stderr

fread error checking (don't exceed max val and don't hit EOF)
  ----------------
*/
int readPPM (char *infile, PPM_file_struct *input) {
  // opening message
  printf("Scanning %s for info ...\n",infile);
  input->fh_in = fopen(infile,"r");

  // variables for this func with some initial settings
  int first_char = fgetc(input->fh_in);
  //  CURRENT_CHAR = first_char;
    CURRENT_CHAR = fgetc(input->fh_in);
  int linecount = 0;
  int charcount = 0;
  
  // ------------------------------- HEADER --------------------------------
  // First char should always be a P (else exit), if so next char is number and advance from there
  if (first_char != 'P') {
    message("Error","File format not recognized!");
    exit(-1);
  } else {
    input->magic_number = getChunk(input);
    // should be a newline, but doesn't have to be, so advance to end of first line until we hit one
    while(fgetc(input->fh_in) != '\n');
    linecount++;
  } // Done with first line
  

  // Now traverse any comment lines in one loop, and also account for P7
  CURRENT_CHAR = fgetc(input->fh_in);
  int done_with_comments = 0;
  while(CURRENT_CHAR != EOF) {
    charcount++;
    // Deal with the comment lines by advancing until we hit a new line where first char is not #
    if(!done_with_comments) { // do this if only once
      if(CURRENT_CHAR == '#') {
	while(!(PREV_CHAR == '\n' && CURRENT_CHAR != '#')) {
	  PREV_CHAR = CURRENT_CHAR;
	  CURRENT_CHAR = fgetc(input->fh_in);
	  if(CURRENT_CHAR == '\n') {linecount++;}
	}
      } else {
	message("Warning","No comment lines detected, please double-check file");
      }
      done_with_comments = 1;
    }
    
    // Now get the width/height, format checking as part of this
    input->width = getChunk(input);

    // Now height
    advanceToChunk(input);
    input->height = getChunk(input);
    
    // Need the alpha channel next
    // If the Maxval is less than 256, it is 1 byte. Otherwise, it is 2 bytes. The most significant byte is first.
    advanceToChunk(input);
    input->alpha = getChunk(input);
    
    message("Info","Completed processing header information.");
    reportPPMStruct(input);

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
  pixel_map = malloc(sizeof(Pixel) * input->width * input->height );
  int chunk_count = 0;
  int rgb_index = 0;
  int pm_index = 0;
  int total_pixels = (input->width) * (input->height) * 3;

  // This switch handles parsing the various input file formats for the image data
  switch(input->magic_number) {
  case(3):
    message("Info","  format version: 3");
    while(PREV_CHAR != EOF && chunk_count < total_pixels) {
      rgb_index = chunk_count % 3;
      advanceToChunk(input);
      int value = getChunk(input);
      switch(rgb_index) {
      case(0):
	pixel_map[pm_index].r = value;
	if(VERBOSE) {printf("  stored %d to pixel_map red\n",pixel_map[pm_index].r);}
	break;
      case(1):
	pixel_map[pm_index].g = value;
	if(VERBOSE) {printf("  stored %d to pixel_map green\n",pixel_map[pm_index].g);}
	break;
      case(2):
	pixel_map[pm_index].b = value;
	if(VERBOSE) {printf("  stored %d to pixel_map blue\n",pixel_map[pm_index].b);}
	pm_index++;
	break;
      }
      chunk_count++;
    }
    printf("read %d chunks\n",chunk_count);
    /*
    pixel_map[chunk_count+1].r = 0; // NULL terminators
    pixel_map[chunk_count+1].g = 0;
    pixel_map[chunk_count+1].b = 0;
    pixel_map[chunk_count+1].a = 0;
    */
    message("Info","Done reading PPM 3");
    //reportPixelMap(pixel_map);
    break;
  case(6):
    message("Info","  format version: 6");
    while(chunk_count < total_pixels) {
      int value[4];
      rgb_index = chunk_count % 3;
      fread(value,sizeof(Pixel),1,input->fh_in);
      switch(rgb_index) {
      case(0):
	pixel_map[pm_index].r = (char)value;
	if(VERBOSE) {printf("  stored %d to pixel_map red\n",pixel_map[pm_index].r);}
	break;
      case(1):
	pixel_map[pm_index].g = value;
	if(VERBOSE) {printf("  stored %d to pixel_map green\n",pixel_map[pm_index].g);}
	break;
      case(2):
	pixel_map[pm_index].b = value;
	if(VERBOSE) {printf("  stored %d to pixel_map blue\n",pixel_map[pm_index].b);}
	pm_index++;
	break;
      }
      chunk_count++;
    }
    break;
  case(7):
    message("Info","  format version: 7");
    break;
  }
  
  // ------------------------------- END IMAGE ------------------------------

  // finishing
  fclose(input->fh_in);
  message("Info","Done");
  return input->magic_number;
}

// convert from input format to output format
void convertFormat (int format, PPM_file_struct *input, PPM_file_struct *output) {
  //  switch(format):
  printf("Converting to format %d ...\n",format);
  output->magic_number = format;
  output->width        = input->width;
  output->height       = input->height;
  output->alpha        = input->alpha;
  message("Info","Done with conversion");
}

// write the output file in the new format
void writePPM (char *outfile, PPM_file_struct *output) {
  printf("Writing file %s...\n",outfile);
  FILE* fh_out = fopen(outfile,"wb");

  // -------------------------- write header ---------------------------------
  fprintf(fh_out,"P%d\n",output->magic_number);
  fprintf(fh_out,"# PPM file format %d\n",output->magic_number);
  fprintf(fh_out,"# written by ppmrw(rmr5)\n");
  fprintf(fh_out,"%d %d\n",output->width,output->height);
  fprintf(fh_out,"%d\n",   output->alpha);
  // ---------------------- done write header --------------------------------

  // -------------------------- write image ----------------------------------
  int pixel_index = 0;
  int modulo;
  switch(output->magic_number) {
  // this case is the P3 format
  case(3):
    message("Info","Outputting format 3");
    while(pixel_index < (output->width) * (output->height)) {      
      fprintf(fh_out,"%3d %3d %3d",pixel_map[pixel_index].r,pixel_map[pixel_index].g,pixel_map[pixel_index].b);
      modulo = (pixel_index + 1) % (output->width);
      if ( modulo == 0 ) {
	fprintf(fh_out,"\n");
      } else {
	fprintf(fh_out," ");
      }
      pixel_index++;
    }
    break;
  // this case is the P6 format
  case(6):
    message("Info","Outputting format 6");
    // TODO: this is writing but the image is broken somehow, need to fix
    fwrite(pixel_map, sizeof(Pixel), output->width * output->height, fh_out);
    break;
  // this case is the P7 format
  case(7):
    message("Info","Outputting format 7");
    break;
  default:
    message("Error","Unrecognized output format");
  }
  // ---------------------- done write image ---------------------------------

  fclose(fh_out);
  message("Info","Done ");
}

// helper function to visualize what's in a given PPM struct
void reportPPMStruct (PPM_file_struct *input) {
  message("Info","Contents of PPM struct:");
  printf("     magic_number: %d\n",input->magic_number);
  printf("     width:        %d\n",input->width);
  printf("     height:       %d\n",input->height);
  printf("     alpha:        %d\n",input->alpha);
}

void reportPixelMap (Pixel *pm) {
  int index = 0;
  int fail_safe = 0;
  while(index < sizeof(pm) && fail_safe < 1000) {
    printf("rPM: [%d] = [%d,%d,%d]\n",index,pm[index].r,pm[index].g,pm[index].b);
    index++;
    fail_safe++;
  }
}

// meant to be used with advanceToChunk function that basically skips whitespace
int getChunk (PPM_file_struct *input) {
  // if you do a getChunk from current, that's different from getChunk so manage it
  // Yes, always pass in a current character, and return the new current character (which is prev char)
    int tc_index = 0;
    int max_chars = 16;
    char tmp[max_chars];
    tmp[tc_index] = CURRENT_CHAR;
    
    do {
      //printf("DBG_gC: CURRENT_CHAR (%d)%c\n",tc_index,CURRENT_CHAR);
      CURRENT_CHAR = fgetc(input->fh_in);
      tmp[++tc_index] = CURRENT_CHAR;
      // fail safe
      if(tc_index > max_chars) {
	message("Error","File format error, more chars than expected without whitespace");
	exit(-1);
      }
    } while(CURRENT_CHAR != ' ' && CURRENT_CHAR != '\n' && CURRENT_CHAR != EOF) ;

    // finish up and return converted value
    tmp[tc_index] = 0;
    PREV_CHAR = CURRENT_CHAR;
    int value = atoi(tmp);
    if(value > 256 || value < 0) {
      message("Error","Unsupported byte sizes in image data, 0 to 256 is supported");
      exit(-1);
    }
    return value;
}

void advanceToChunk (PPM_file_struct *input) {
  while(CURRENT_CHAR == ' ' || CURRENT_CHAR == '\n') {
    CURRENT_CHAR = fgetc(input->fh_in);
  }
  PREV_CHAR = CURRENT_CHAR;
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
