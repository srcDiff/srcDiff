/* 
   Class Definition to read in two files and output the differences as XPath expressions.

   Michael J. Decker
   mjd52@zips.uakron.edu
 */
#ifndef INCLUDED_DIFF_TRACE_READER_HPP
#define INCLUDED_DIFF_TRACE_READER_HPP

class diff_trace_reader {

private:
  // fields
  xmlTextReaderPtr old_reader;
  xmlTextReaderPtr new_reader;

public:
  // constuctors
  diff_trace_reader(const char * old_filename, const char * new_filename);

  // destructor
  ~diff_trace_reader();


  // methods
  void process_loop();

private:
  // methods
  void process_node(xmlTextReaderPtr reader);

};

#endif
