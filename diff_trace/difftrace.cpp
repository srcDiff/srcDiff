/*
  difftrace

  Take two source code files and compute the difference using
  srcdiff, then form the absolute XPath using SAX.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

// includes
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <errno.h>
#include <libxml/parserInternals.h>
#include <getopt.h>

#include "difftraceapps.hpp"
#include "SAX2DiffTrace.hpp"

void output_help(int argc, char * argv[]);
void output_version(int argc, char * argv[]);
int process_args(int, char**, long & options);

/*
  Main method of program
*/
int main(int argc, char **argv) {

  // Create context for first file
  xmlParserCtxtPtr ctxt = xmlCreateURLParserCtxt(argv[1], XML_PARSE_COMPACT);

  long options = 0;

  int curarg = process_args(argc, argv, options); 

  // Create SAX object and add to context
  SAX2DiffTrace tracer(options);

  ctxt->_private = &tracer;
  
  // set up the SAX parser and add to context
  xmlSAXHandler sax = SAX2DiffTrace::factory();
  ctxt->sax = &sax;

  // process the document
  xmlParseDocument(ctxt);

  // null the SAX parser so can be freed
  ctxt->sax = NULL;

  // free the context
  xmlFreeParserCtxt(ctxt);
  
  return 0;
}

int process_args(int argc, char * argv[], long & options)
{
  // command line argument struct for getopt                                                                                                           
  struct option cliargs[] = {

    { HELP_FLAG, no_argument, NULL, HELP_FLAG_SHORT },
    { VERSION_FLAG, no_argument, NULL, VERSION_FLAG_SHORT },
    { WHITESPACE_FLAG, no_argument, NULL, WHITESPACE_FLAG_SHORT },
    { 0, 0, 0, 0}

  };

  // process all options
  while(1)
    {
      // get option
      int option_index = 0;
      int c = getopt_long(argc, argv, "hvl:e:s:", cliargs, &option_index);

      // finished
      if(c == -1)
        break;

      // error
      if(c == '?')
        {
          fprintf(stderr, "Try %s --%s for more information.\n", argv[0], HELP_FLAG);
          exit(INVALID_OPTION);
        }

      // process command line arguments
      switch(c)
        {
        case HELP_FLAG_SHORT:

          output_help(argc, argv);

          exit(SUCCESS);
          break;

        case VERSION_FLAG_SHORT:

          output_version(argc, argv);

          exit(SUCCESS);
          break;

        case WHITESPACE_FLAG_SHORT:

          options |= OPTION_WHITESPACE;

          exit(SUCCESS);
          break;


    default:
      //fprintf(stderr, "Invalid Option: %s\n", optarg);
      exit(INVALID_OPTION);
      break;
    };
}

// return position
return optind;

}

void output_help(int argc, char * argv[]) {

}

void output_version(int argc, char * argv[]) {

}
