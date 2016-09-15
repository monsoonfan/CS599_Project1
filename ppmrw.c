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
1) compiler warnings
ppmrw.c:321:26: warning: cast from pointer to integer of different size [-Wpointer-to-int-cast]

2) tricks for dealing with binary so I can see what my P6 conversions are not correct? Looks like a windows issue, pixmap also looks bad for me but was fine on mac.
(wasn't writing alpha, but interestingly, pixmap.c that we did in class is messed up until I put it's binary in ppm)

3) how to deal with tokens: (array of whatever fgetc returns)

* remember to use pointer arithmetic to traverse

Issues: (looks like any binary read is busted)
p7out7
p6out7
p3out7
---------------------------------------------------------------------------------------
*/

// libraries
#include <stdio.h>
#include <stdlib.h>

// typdefs
typedef struct Pixel {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  //  unsigned char a;
} Pixel ;

typedef struct PPM_file_struct {
  char magic_number;
  int lines;
  int width;
  int height;
  int alpha;
  int depth;
  char *tupltype;
  //Pixel *pixel_map; // this is a problem, would need to dynamically malloc this thing to be efficient
  FILE* fh_in;
} PPM_file_struct ;

// global variables
int CURRENT_CHAR        = 'a';
int PREV_CHAR           = '\n';
int OUTPUT_MAGIC_NUMBER = 0;
int VERBOSE             = 0; // controls logfile message level

// global data structures
PPM_file_struct  INPUT_FILE_DATA;
Pixel           *PIXEL_MAP;
PPM_file_struct  OUTPUT_FILE_DATA;


// functions
int  readPPM         (char *infile,          PPM_file_struct *input                          );
void writePPM        (char *outfile,         PPM_file_struct *input                          );
void message         (char message_code[],   char message[]                                  );

void reportPPMStruct (PPM_file_struct *input);
void reportPixelMap  (Pixel *pm             );
int  getNumber       (PPM_file_struct *input);
char getWord         (PPM_file_struct *input);
void skipWhitespace  (PPM_file_struct *input);
void skipLine        (PPM_file_struct *input);
void help            ();
void convertFormatAndWritePPMHeader  (FILE* fh              );
int computeDepth();
char computeTuplType();

/*
 ------------------------------------------------------------------
                                 MAIN
 ------------------------------------------------------------------
*/
int main(int argc, char *argv[]) {
  
  // Argument checking done in main, as it's specific to this project
  // format: ppmrw 3 input.ppm output.ppm
  if (argc != 4) {
    help();
    return(1);
  }
  
  // Get parameters from arguments
  OUTPUT_MAGIC_NUMBER = atoi(argv[1]);
  char *infile = argv[2];
  char *outfile = argv[3];
  printf("Converting file to format %d ...\n",OUTPUT_MAGIC_NUMBER);
  printf("    Input : %s\n",infile);
  printf("    Output: %s\n",outfile);
  
  // Open the input file and traverse it, storing the image to buffer
  readPPM(infile,&INPUT_FILE_DATA);
  
  // Write the contents of the new file in desired format
  writePPM(outfile,&INPUT_FILE_DATA);

  // Verify correct output

  // Successful exit was reached
  return EXIT_SUCCESS;
}
/*
 ------------------------------------------------------------------
                               END MAIN
 ------------------------------------------------------------------
*/




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
    exit(-1);
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
  CURRENT_CHAR = fgetc(input->fh_in);
  int linecount = 0;
  int charcount = 0;
  
  // ------------------------------- HEADER --------------------------------
  // First char should always be a P (else exit), if so next char is number and advance from there
  if (first_char != 'P') {
    message("Error","File format not recognized!");
  } else {
    input->magic_number = getNumber(input);
    linecount++;
    CURRENT_CHAR = fgetc(input->fh_in);
  } // Done with first line, advanced to 1st char of 2nd line
  
  // P3/P6 cases - Now traverse any comment lines in one loop
  if (input->magic_number == 3 ||input->magic_number == 6) {
    printf("DBG p3 or p6 header case\n");
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
	// always on the 1st char of the next line after comments
      }
      
      // Now get the width/height, format checking as part of this
      input->width = getNumber(input);
      
      // Now height
      skipWhitespace(input);
      input->height = getNumber(input);
      
      // Need the alpha channel next
      // If the Maxval is less than 256, it is 1 byte. Otherwise, it is 2 bytes. The most significant byte is first.
      skipWhitespace(input);
      input->alpha = getNumber(input);
      
      message("Info","Completed processing header information.");
      
      break;
    }
    // P7 case
  } else {
    // P7 headers have tokens to process
    int got_width    = 0;
    int got_height   = 0;
    int got_depth    = 0;
    int got_maxval   = 0;
    int got_tupltype = 0;
    int got_endhdr   = 0;
    int comment_lines= 0;
    
    while(CURRENT_CHAR != EOF && !got_endhdr) {
      //    char *token_name;
      char token_name[32];
      *token_name = getWord(input); // this is a hack, only getting first char of token but sould work
      //      printf("DBG tn[0]: (%c)\n",*token_name);
      // TODO: clean this up, was what I was trying when trying to match the whole token
      /*
	for (int i = 0; i < 32; i++) {printf("tn[%d]:%c\n",i,*(token_name+1));}
	printf("DBG tn[0]: (%c)\n",*token_name);
      */
      if (*token_name == '#') {
	skipLine(input);
      } else {
	skipWhitespace(input); // position at the first char of the token, switch will grab token and advance line
      }
      
      switch(*token_name) {
      case('W'):
	if(got_width) {message("Error","More than one WIDTH token!");}
	message("Info","Processing WIDTH token");
	input->width = getNumber(input);
	skipWhitespace(input);
	got_width = 1;
	break;
      case('H'):
	if(got_height) {message("Error","More than one HEIGHT token!");}
	message("Info","Processing HEIGHT token");
	input->height = getNumber(input);
	skipWhitespace(input);
	got_height = 1;
	break;
      case('D'):
	if(got_depth) {message("Error","More than one DEPTH token!");}
	message("Info","Processing DEPTH token");
	input->depth = getNumber(input);
	skipWhitespace(input);
	got_depth = 1;
	break;
      case('M'):
	if(got_maxval) {message("Error","More than one MAXVAL token!");}
	message("Info","Processing MAXVAL token");
	input->alpha = getNumber(input);
	skipWhitespace(input);
	got_maxval = 1;
	break;
      case('T'):
	if(got_tupltype) {message("Error","More than one TUPLTYPE token!");}
	message("Info","Processing TUPLTYPE token");
	input->tupltype = getWord(input);
	if(input->tupltype != 'R') {message("Error","Unsupported TUPLTYPE!");}
	got_tupltype = 1;
	break;
      case('E'):
	if(got_endhdr) {message("Error","Could not recognize P7 header!");}
	message("Info","DBG ENDHDR line reached");
	//	printf("DBG: set exit with cc (%c)(%c)(%c)\n",CURRENT_CHAR,PREV_CHAR,fgetc(input->fh_in));
	got_endhdr = 1;
	break;
      default:
	// TODO: clean this up, don't need a default case here because it's okay to not have a token, could be # line
	if(*token_name == '#') {
	  comment_lines++;
	} else {
	  message("Warning","Expected a token in P7 header file");
	}
	break;
      }
    }
    if (got_width && got_height && got_depth && got_maxval && got_tupltype) {
      message("Info","Done processing header");
      printf("Info: there were %d comment lines\n",comment_lines);
    } else {
      message("Error","Missing token(s), invalid header!");
    }
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
  PIXEL_MAP = malloc(sizeof(Pixel) * input->width * input->height );
  int number_count = 0;
  int rgb_index = 0;
  int pm_index = 0;
  int total_pixels = (input->width) * (input->height) * 3;

  // This switch handles parsing the various input file formats for the image data
  switch(input->magic_number) {
  case(3):
    message("Info","  format version: 3");
    while(PREV_CHAR != EOF && number_count < total_pixels) {
      rgb_index = number_count % 3;
      skipWhitespace(input);
      int value = getNumber(input);
      switch(rgb_index) {
      case(0):
	PIXEL_MAP[pm_index].r = value;
	if(VERBOSE) {printf("  stored[%d] %d to PIXEL_MAP red\n",pm_index,PIXEL_MAP[pm_index].r);}
	break;
      case(1):
	PIXEL_MAP[pm_index].g = value;
	if(VERBOSE) {printf("  stored[%d] %d to PIXEL_MAP green\n",pm_index,PIXEL_MAP[pm_index].g);}
	break;
      case(2):
	PIXEL_MAP[pm_index].b = value;
	if(VERBOSE) {printf("  stored[%d] %d to PIXEL_MAP blue\n",pm_index,PIXEL_MAP[pm_index].b);}
	pm_index++;
	break;
      }
      number_count++;
    }
    printf("read %d numbers\n",number_count);
    /*
    PIXEL_MAP[number_count+1].r = 0; // NULL terminators
    PIXEL_MAP[number_count+1].g = 0;
    PIXEL_MAP[number_count+1].b = 0;
    PIXEL_MAP[number_count+1].a = 0;
    */
    message("Info","Done reading PPM 3");
    //reportPixelMap(PIXEL_MAP);
    break;
  case(6):
    message("Info","  format version: 6");
    while(number_count < total_pixels) {
      //      int value[4];
      unsigned char value;
      rgb_index = number_count % 3;
      // TODO: fread error checking (don't exceed max val and don't hit EOF)
      if (!fread(&value,sizeof(Pixel)/3,1,input->fh_in)) {message("Error","Binary data read error");}
      switch(rgb_index) {
      case(0):
	PIXEL_MAP[pm_index].r = value;
	if(VERBOSE) {printf("  stored[%d](%d) %d to PIXEL_MAP red\n",pm_index,rgb_index,PIXEL_MAP[pm_index].r);}
	break;
      case(1):
	PIXEL_MAP[pm_index].g = value;
	if(VERBOSE) {printf("  stored[%d](%d) %d to PIXEL_MAP green\n",pm_index,rgb_index,PIXEL_MAP[pm_index].g);}
	break;
      case(2):
	PIXEL_MAP[pm_index].b = value;
	if(VERBOSE) {printf("  stored[%d](%d) %d to PIXEL_MAP blue\n",pm_index,rgb_index,PIXEL_MAP[pm_index].b);}
	pm_index++;
	break;
      }
      number_count++;
    }
    printf("Info: read %d bytes\n",number_count);
    break;
  case(7):
    message("Info","  format version: 7");
    printf("DBG: fread from cc(%d) pc(%d)\n",&CURRENT_CHAR,&PREV_CHAR);
    while(number_count < total_pixels) {
      //      int value[4];
      unsigned char value;
      rgb_index = number_count % 3;
      // TODO: fread error checking (don't exceed max val and don't hit EOF)
      fread(&value,sizeof(Pixel)/3,1,input->fh_in);
      switch(rgb_index) {
      case(0):
	PIXEL_MAP[pm_index].r = value;
	if(VERBOSE) {printf("  stored[%d](%d) %d to PIXEL_MAP red\n",pm_index,rgb_index,PIXEL_MAP[pm_index].r);}
	break;
      case(1):
	PIXEL_MAP[pm_index].g = value;
	if(VERBOSE) {printf("  stored[%d](%d) %d to PIXEL_MAP green\n",pm_index,rgb_index,PIXEL_MAP[pm_index].g);}
	break;
      case(2):
	PIXEL_MAP[pm_index].b = value;
	if(VERBOSE) {printf("  stored[%d](%d) %d to PIXEL_MAP blue\n",pm_index,rgb_index,PIXEL_MAP[pm_index].b);}
	pm_index++;
	break;
      }
      number_count++;
    }
    printf("Info: read %d bytes\n",number_count);
    break;
  }
  
  // ------------------------------- END IMAGE ------------------------------

  // finishing
  fclose(input->fh_in);
  message("Info","Done");
  reportPPMStruct(input);
  return input->magic_number;
}

int computeDepth() {
  if (INPUT_FILE_DATA.magic_number != 7) {
    // TODO: need correct logic here
    return 1; 
  } else {
    return INPUT_FILE_DATA.depth;
  }
}
char computeTuplType() {
    // TODO: need correct logic here
  if (INPUT_FILE_DATA.tupltype != "RBG_ALPHA") {
    // TODO: need correct logic here
    return "RGB"; 
  } else {
    return "RGB_ALPHA"; 
  }
}

// convert from input format to output format and write the header
void convertFormatAndWritePPMHeader (FILE* fh) {
  int magic_number = OUTPUT_MAGIC_NUMBER;

  // These values/header elements are the same regardless format
  printf("Converting to format %d ...\n",magic_number);
  fprintf(fh,"P%d\n",magic_number);
  fprintf(fh,"# PPM file format %d\n",magic_number);
  fprintf(fh,"# written by ppmrw(rmr5)\n");
  OUTPUT_FILE_DATA.magic_number = magic_number;
  OUTPUT_FILE_DATA.width        = INPUT_FILE_DATA.width;
  OUTPUT_FILE_DATA.height       = INPUT_FILE_DATA.height;
  OUTPUT_FILE_DATA.alpha        = INPUT_FILE_DATA.alpha;
  
  if (magic_number == 3 || magic_number == 6) {
    fprintf(fh,"%d %d\n",       OUTPUT_FILE_DATA.width,OUTPUT_FILE_DATA.height);
    fprintf(fh,"%d\n",          OUTPUT_FILE_DATA.alpha);
  } else if (magic_number == 7) {
    OUTPUT_FILE_DATA.depth      = computeDepth();
    //    OUTPUT_FILE_DATA.tupltype   = computeTuplType();
    OUTPUT_FILE_DATA.tupltype   = "RBG";
    
    fprintf(fh,"WIDTH %d\n",    OUTPUT_FILE_DATA.width);
    fprintf(fh,"HEIGHT %d\n",   OUTPUT_FILE_DATA.height);
    fprintf(fh,"DEPTH %d\n",    OUTPUT_FILE_DATA.depth);
    fprintf(fh,"MAXVAL %d\n",   OUTPUT_FILE_DATA.alpha);
    fprintf(fh,"TUPLTYPE %d\n", OUTPUT_FILE_DATA.tupltype);
    fprintf(fh,"ENDHDR\n");
  } else {
    message("Error","Trying to output unsupported format!\n");
  }
  message("Info","Done with conversion");
}

// write the output file in the new format
void writePPM (char *outfile, PPM_file_struct *input) {
  printf("Writing file %s...\n",outfile);
  FILE* fh_out = fopen(outfile,"wb");

  // -------------------------- write header ---------------------------------
  convertFormatAndWritePPMHeader(fh_out);
  // ---------------------- done write header --------------------------------

  // -------------------------- write image ----------------------------------
  int pixel_index = 0;
  int modulo;
  switch(OUTPUT_FILE_DATA.magic_number) {
  // P3 format
  case(3):
    message("Info","Outputting format 3");
    while(pixel_index < (OUTPUT_FILE_DATA.width) * (OUTPUT_FILE_DATA.height)) {      
      fprintf(fh_out,"%3d %3d %3d",PIXEL_MAP[pixel_index].r,PIXEL_MAP[pixel_index].g,PIXEL_MAP[pixel_index].b);
      modulo = (pixel_index + 1) % (OUTPUT_FILE_DATA.width);
      if ( modulo == 0 ) {
	fprintf(fh_out,"\n");
      } else {
	fprintf(fh_out," ");
      }
      pixel_index++;
    }
    break;
  // P6 format
  case(6):
    message("Info","Outputting format 6");
    // TODO: this is writing but the image is broken somehow, need to fix
    fwrite(PIXEL_MAP, sizeof(Pixel), OUTPUT_FILE_DATA.width * OUTPUT_FILE_DATA.height, fh_out);
    break;
  // P7 format
  case(7):
    message("Info","Outputting format 7");
    // TODO: this is writing but the image is broken somehow, need to fix
    fwrite(PIXEL_MAP, sizeof(Pixel), OUTPUT_FILE_DATA.width * OUTPUT_FILE_DATA.height, fh_out);
    break;
  default:
    message("Error","Unrecognized output format");
  }
  // ---------------------- done write image ---------------------------------

  fclose(fh_out);
  reportPPMStruct(&OUTPUT_FILE_DATA);
  message("Info","Done ");
}

// helper function to visualize what's in a given PPM struct
void reportPPMStruct (PPM_file_struct *input) {
  message("Info","Contents of PPM struct:");
  printf("     magic_number: %d\n",input->magic_number);
  printf("     width:        %d\n",input->width);
  printf("     height:       %d\n",input->height);
  if (input->magic_number == 7) {
  printf("     max_value:    %d\n",input->alpha);
  printf("     depth:        %d\n",input->depth);
  // TODO fix this
  printf("     tupltype:     %c\n",input->tupltype);
  } else {
    printf("     alpha:        %d\n",input->alpha);
  }
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

// meant to be used with skipWhitespace function that basically skips whitespace
int getNumber (PPM_file_struct *input) {
  // if you do a getNumber from current, that's different from getNumber so manage it
  // Yes, always pass in a current character, and return the new current character (which is prev char)
  int tc_index = 0;
  int max_chars = 16;
  char tmp[max_chars];
  tmp[tc_index] = CURRENT_CHAR;
  
  do {
    //printf("DBG_gN: CURRENT_CHAR (%d)%c\n",tc_index,CURRENT_CHAR);
    CURRENT_CHAR = fgetc(input->fh_in);
    tmp[++tc_index] = CURRENT_CHAR;
    // fail safe
    if(tc_index > max_chars) {
      message("Error","File format error, more chars than expected without whitespace");
    }
  } while(CURRENT_CHAR != ' ' && CURRENT_CHAR != '\n' && CURRENT_CHAR != EOF) ;
  //  CURRENT_CHAR = fgetc(input->fh_in);
  
  // finish up and return converted value
  tmp[tc_index] = 0;
  PREV_CHAR = CURRENT_CHAR;
  int value = atoi(tmp);
  // error checking
  if(value > 256 || value < 0) {
    message("Error","Unsupported byte sizes in image data, 0 to 256 is supported");
  }
  return value;
}

char getWord (PPM_file_struct *input) {
  int index = 0;
  int max_chars = 32; // large enough to deal with TUPLTYPE tokens
  char tmp[max_chars];
  //char *tmp;
  tmp[index] = CURRENT_CHAR;
  
  do {
    //printf("DBG_gW: CURRENT_CHAR (%d)%c\n",index,CURRENT_CHAR);
    CURRENT_CHAR = fgetc(input->fh_in);
    tmp[++index] = CURRENT_CHAR;
    if(index > max_chars) {
      message("Error","File format error, more chars than expected without whitespace");
    }
  } while(CURRENT_CHAR != ' ' && CURRENT_CHAR != '\n' && CURRENT_CHAR != EOF) ;
  //CURRENT_CHAR = fgetc(input->fh_in);

  // finish up and return converted value
  tmp[++index] = 0; // NULL terminator
  PREV_CHAR = CURRENT_CHAR;
  //printf("DBG: returning %d\n",*tmp);
  return *tmp;

}

void skipWhitespace (PPM_file_struct *input) {
  while(CURRENT_CHAR == ' ' || CURRENT_CHAR == '\n') {
    CURRENT_CHAR = fgetc(input->fh_in);
  }
  PREV_CHAR = CURRENT_CHAR;
}

void skipLine (PPM_file_struct *input) {
  //printf("DBG skipline\n");
  while(CURRENT_CHAR != '\n') {
    //printf("   skipping %c\n",CURRENT_CHAR);
    CURRENT_CHAR = fgetc(input->fh_in);
  }
  CURRENT_CHAR = fgetc(input->fh_in); // advance past the \n as getWord/Number are designed to work this way
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
