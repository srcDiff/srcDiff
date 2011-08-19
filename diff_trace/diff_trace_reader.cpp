/* 
   Method definitions to read in two files and output the differences as XPath expressions.

   Michael J. Decker
   mjd52@zips.uakron.edu
 */
#include <libxml/xmlreader.h>
#include "diff_trace_reader.hpp"

diff_trace_reader::diff_trace_reader(const char * old_filename, const char * new_filename) {

  old_reader = xmlNewTextReaderFilename(old_filename);
  new_reader = xmlNewTextReaderFilename(new_filename);
}

diff_trace_reader::~diff_trace_reader() {

  if(old_reader != NULL) 
    xmlFreeTextReader(old_reader);

  if(new_reader != NULL) 
    xmlFreeTextReader(new_reader);
}

void diff_trace_reader::process_loop() {

  if(old_reader != NULL && new_reader != NULL) {

    int ret_old = xmlTextReaderRead(old_reader);
    //int ret_new = xmlTextReaderRead(old_reader);
    while(ret_old == 1) {
      process_node(old_reader);
      ret_old = xmlTextReaderRead(old_reader);
    }
  }

}

void diff_trace_reader::process_node(xmlTextReaderPtr reader) {

  fprintf(stdout, "%s\n", xmlTextReaderName(reader));
}

int main(int argc, char * argv[]) {

  diff_trace_reader reader("test/examples/exampleold.c.xml", "test/examples/examplenew.c.xml");
  reader.process_loop();

  return 0;
}
