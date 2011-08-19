/*
  SAX2DiffTraceSimple.cpp

  Implements SAX object and SAX methods for extracting an absolute XPath from
  srcML documents for differences between files

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

// includes
#include <cstring>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <iostream>

#include "SAX2DiffTraceSimple.hpp"

// helper method
int find_attribute_index(int nb_attributes, const xmlChar** attributes, const char* attribute);
std::string & trim_string(std::string & source);

/*
  Constructor
*/
SAX2DiffTraceSimple::SAX2DiffTraceSimple(int nedits, struct varray * ses, std::vector<struct diff_edit> columnoffsets, int difftype):
  numedits(nedits), editpos(0), diff_edits(ses), DIFF_TYPE(difftype), column_diffs(columnoffsets), elements(), labels() {
}

/*
  Factory for creating SAX parser
*/
xmlSAXHandler SAX2DiffTraceSimple::factory() {

  // create parser
  xmlSAXHandler sax = { 0 };

  //fprintf(stderr, "factory\n");
  
  // set methods
  sax.initialized    = XML_SAX2_MAGIC;
  sax.startDocument  = &SAX2DiffTraceSimple::startDocument;
  sax.endDocument    = &SAX2DiffTraceSimple::endDocument;
  sax.startElement   = &SAX2DiffTraceSimple::startElement;
  sax.endElement     = &SAX2DiffTraceSimple::endElement;
  sax.characters     = &SAX2DiffTraceSimple::characters;
  sax.comment        = NULL;

  // return parser
  return sax;
}

/*
  start document
*/
void SAX2DiffTraceSimple::startDocument(void * ctx) {

  // retrieve context
  SAX2DiffTraceSimple * pstate = (SAX2DiffTraceSimple *) ctx;

  // initialize state
  pstate->linecount = 0;
  pstate->columncount = 0;
  pstate->readsignature = false;
  pstate->waitoutput = false;
  
  // get next difference of appropriate type if it exists
  struct diff_edit * edit = get_next_diff(pstate);
  
  //if(edit)
  //fprintf(stderr, "Column Diff off:%d\n", pstate->column_diffs[pstate->editpos].off);

  //fprintf(stderr, "startDocument\n");
}

/*
  end document
*/
void SAX2DiffTraceSimple::endDocument(void * ctx) {

  // retrieve context
  SAX2DiffTraceSimple * pstate = (SAX2DiffTraceSimple *) ctx;
  
  //fprintf(stderr, "endDocument\n");
}

/* 
   characters

   Counts lines and columns. Locates diffs, and outputs information
*/
void SAX2DiffTraceSimple::characters(void * ctx, const xmlChar* ch, int len) {

  // retrieve context
  SAX2DiffTraceSimple * pstate = (SAX2DiffTraceSimple *) ctx;

  // process each character
  for(int pos = 0; pos < len; ++pstate->columncount, ++ch, ++pos)
  {

    // update column first
    //++pstate->columncount;

    // gather function signature
    if((pstate->readsignature || pstate->readconstruct) && (*(char *)ch) != '\n' && (*(char *)ch) != '\t') {

      pstate->function += *((char *)ch);
      pstate->function = trim_string(pstate->function);
    }

    // get current edit if exists
    struct diff_edit * edit = get_next_diff(pstate);

    // if there is another edit to catch
    if(edit) {

      // if reached the line and column of the difference set output
      if((edit->op == pstate->DIFF_TYPE) && (pstate->linecount == edit->off) && (pstate->columncount == pstate->column_diffs[pstate->editpos].off)) {

        //fprintf(stderr, "Difference: Line: %d Column: %d\n", pstate->linecount, pstate->columncount);

        // stall when reading function signature
        if(pstate->readsignature || pstate->readconstruct)
          pstate->waitoutput = true;

        // else output stack
        else {

          output_diff(pstate, pstate->elements.size());

          // find next difference if it exists
          ++pstate->editpos;
          edit = get_next_diff(pstate);

          //fprintf(stderr, "Column Diff off:%d len:%d\n", pstate->column_diffs[pstate->editpos].off, pstate->column_diffs[pstate->editpos].len);

        }
      }
    }

    // reset column and increment line count on newline
    if(*((char *)ch) == '\n') {
      ++pstate->linecount;
      //fprintf(stderr, "Line: %d\n", pstate->linecount);
      pstate->columncount = -1;

  }

  }

  //fprintf(stderr, "characters\n");
}

/*
  start unit element

  push elements on the appropriate stacks
*/
void SAX2DiffTraceSimple::startElement(void * ctx, const xmlChar * name, const xmlChar ** attrs) {

  // retrieve context
  SAX2DiffTraceSimple * pstate = (SAX2DiffTraceSimple *) ctx;

  // put tag on element stack and store an empty function
  pstate->elements.push_back(std::string((char *)name));

  // add the file name
  if(strcmp((char *)name, "unit") == 0) {
    int index = find_attribute_index(0, attrs, "filename") + 1;

    // if attribute does not exist
    if(index == 0)
      pstate->labels.push_back(std::string(""));
    else
      pstate->labels.push_back(std::string((char *)attrs[index]));
  }
  else
    pstate->labels.push_back(std::string(""));

  // if function tag start to collect the signature from characters
  if(strcmp((char *)name, "function") == 0 || strcmp((char *)name, "function_decl") == 0) {

    pstate->readsignature = true;
    pstate->function = std::string();
  }

  // if class or structure read 
  else if(strcmp((char *)name, "class") == 0 || strcmp((char *)name, "class_decl") == 0
     || strcmp((char *)name, "struct") == 0 || strcmp((char *)name, "struct_decl") == 0) {

    pstate->readconstruct = true;
    pstate->function = std::string();
  }

  // if unit or block start a new block and put on block stack
  if(strcmp((char *)name, "unit") == 0 || strcmp((char *)name, "block") == 0) {
    pstate->blocks.push_back(std::map<std::string, int>());
  }
  else {
    // if sub block element increment block count of element 
    if(strcmp((char *)name, "expr_stmt") == 0)
      ++pstate->blocks.back()["expr_stmt"];
    else if(strcmp((char *)name, "decl_stmt") == 0)
      ++pstate->blocks.back()["decl_stmt"];
    else if(strcmp((char *)name, "if") == 0)
      ++pstate->blocks.back()["if"];
    else if(strcmp((char *)name, "while") == 0)
      ++pstate->blocks.back()["while"];
    else if(strcmp((char *)name, "for") == 0)
      ++pstate->blocks.back()["for"];
    else if(strcmp((char *)name, "do") == 0)
      ++pstate->blocks.back()["do"];
    else if(strcmp((char *)name, "call") == 0)
      ++pstate->blocks.back()["call"];
  }

  //fprintf(stderr, "startElement\n");
}

/*
  end unit element

  pop elements from the appropriate stacks
*/
void SAX2DiffTraceSimple::endElement(void * ctx, const xmlChar * name) {

  // retrieve context
  SAX2DiffTraceSimple * pstate = (SAX2DiffTraceSimple *) ctx;

  // if end of parameter_list tag stop collecting function information from characters and add to element below
  if(strcmp((char *)name, "parameter_list") == 0) {
    std::string temp =  pstate->labels.back();
    pstate->labels.pop_back();
    pstate->labels.pop_back();
    pstate->labels.push_back(pstate->function);
    pstate->labels.push_back(temp);

    pstate->readsignature = false;
  }

  // if end of parameter_list tag stop collecting function information from characters and add to element below
  else if(pstate->readconstruct && strcmp((char *)name, "name") == 0) {

    std::string temp =  pstate->labels.back();
    pstate->labels.pop_back();
    pstate->labels.pop_back();
    pstate->labels.push_back(pstate->function);
    pstate->labels.push_back(temp);

    pstate->readconstruct = false;
  }

  // if unit or block remove a block from the block stack
  else if(strcmp((char *)name, "unit") == 0 || strcmp((char *)name, "block") == 0) {
    pstate->blocks.pop_back();
  }

  // output saved output
  if(pstate->waitoutput && (!pstate->readsignature && !pstate->readconstruct)) {
    
    //fprintf(stderr, "End Difference2: Line: %d Column: \n", pstate->linecount, pstate->columncount);

    int numelements = pstate->elements.size() - 1;
    
    pstate->waitoutput = false;
      
    output_diff(pstate, numelements);
      
    ++pstate->editpos;
  }

  // remove last element and name for that element
  pstate->elements.pop_back();
  pstate->labels.pop_back();

  //fprintf(stderr, "endElement\n");
}

// get the next difference for processing 
struct diff_edit * get_next_diff(SAX2DiffTraceSimple * pstate) {

  // get current edit if exists
  struct diff_edit * edit = NULL;

  if(pstate->editpos < pstate->numedits)
    edit = (diff_edit *)varray_get(pstate->diff_edits, pstate->editpos);

  // NOTE: Removed ((edit->op != pstate->DIFF_TYPE) || (edit->off < pstate->linecount))
  // Not sure what it was for
  while((pstate->editpos < pstate->numedits) && (edit->op != pstate->DIFF_TYPE)) {
    ++pstate->editpos;
    edit = (diff_edit *)varray_get(pstate->diff_edits, pstate->editpos);
  }

  if(pstate->editpos == pstate->numedits)
    edit = NULL;

  return edit;

}

void output_diff(SAX2DiffTraceSimple * pstate, int numelements) {

  // output stack formatted to be an XPath
  // if(pstate->elements.size() > 1)
  //if(pstate->elements[pstate->elements.size() - 1] == "parameter_list" && pstate->column_diffs[pstate->editpos].off == 0)
      if(pstate->DIFF_TYPE == DIFF_INSERT)
        fprintf(stdout,"Insert:\t");
      else
        fprintf(stdout,"Delete:\t");
      /*else if(pstate->DIFF_TYPE == DIFF_INSERT)
      fprintf(stdout,"Change:\t");
    else
      fprintf(stdout,"Change:\t");
  else 
      fprintf(stdout,"OutsideMethodChange\t");    
      */
    
      //fprintf(stderr, "NumElements: %d\n", numelements);
      int blockindex = -1;
      for(int i = 0; i < numelements; ++i) {
        if(strcmp(pstate->elements[i].c_str(), "unit") == 0 || strcmp(pstate->elements[i].c_str(), "block") == 0)
          ++blockindex;
        //fprintf(stderr, "Block Index: %d Block Size:%d\n", blockindex, pstate->blocks.size());
        if (pstate->labels[i][0] != 0) {
          if(pstate->elements[i] == "unit") {
            fprintf(stdout, "/src:%s[@filename='%s']", pstate->elements[i].c_str(), pstate->labels[i].c_str());
          }
          else if(pstate->elements[i] == "function" || pstate->elements[i] == "function_decl") {

            /*
              || pstate->elements[i] == "struct" || pstate->elements[i] == "struct_decl"
              || pstate->elements[i] == "class" || pstate->elements[i] == "class_decl") {
            */

            fprintf(stdout, "/src:%s[src:signature('%s')]", pstate->elements[i].c_str(), pstate->labels[i].c_str());
            //        break;
          }
          else if(pstate->elements[i] == "class" || pstate->elements[i] == "class_decl"
                  || pstate->elements[i] == "struct" || pstate->elements[i] == "struct_decl")

            fprintf(stdout, "/src:%s[src:construct('%s')]", pstate->elements[i].c_str(), pstate->labels[i].c_str());
        }

        else if(pstate->blocks[blockindex][pstate->elements[i]] > 0)
          fprintf(stdout, "/src:%s", pstate->elements[i].c_str());
        else
          fprintf(stdout, "/src:%s", pstate->elements[i].c_str());
      }

  fprintf(stdout, "\n");

}

// index of attribute in attributes
int find_attribute_index(int nb_attributes, const xmlChar** attributes, const char* attribute) {

  for (int i = 0, index = 0; attributes[index]; ++i, index += 2)
    if (strcmp((const char*) attributes[index], attribute) == 0)
      return index;

  return -1;
}

// trim spaces from end of string
std::string & trim_string(std::string & source) {

  //fprintf(stderr, "%s\n", source.c_str());

  int pos;
  for(pos = source.size() - 2; pos >= 0 && source[pos] == ' ' && source[pos + 1] == ' '; --pos);
  return source.erase(pos + 2);
}
