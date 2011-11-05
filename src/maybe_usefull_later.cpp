// check if sub statement
bool is_sub_statement(struct reader_buffer * rbuf, int start) {

  /*
    C-Preprocessor
    cpp:directive, cpp:file, cpp:include, cpp:define, cpp:undef, cpp:line, cpp:if, cpp:ifdef, cpp:ifndef, cpp:else, cpp:elif, cpp:endif, cpp:then, cpp:pragma, cpp:error
  */
  return false;

  if((xmlReaderTypes)rbuf->diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "expr") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "init") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "incr") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "condition") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "name") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "type") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "index") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "decl") == 0)
    return true;

  return false;

}


// check if sub statement
bool is_statement(struct reader_buffer * rbuf, int start) {


  return false;

  if((xmlReaderTypes)rbuf->diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "if") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "then") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "else") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "while") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "do") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "switch") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "case") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "default") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "for") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "break") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "continue") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "comment") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "block") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "expr_stmt") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "decl_stmt") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "goto") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "label") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "typedef") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "asm") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "macro") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "enum") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "empty_stmt") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "namespace") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "template") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "using") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "extern") == 0)
    return true;

  return false;

}

bool is_function_related(struct reader_buffer * rbuf, int start) {

  /*
    function, function_decl, specifier, return, call, parameter_list, param, argument_list, argument
  */

  if((xmlReaderTypes)rbuf->diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "function") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "function_decl") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "specifier") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "return") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "call") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "parameter_list") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "param") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "argument_list") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "argument") == 0)
    return true;

  return false;
}

bool is_structure_related(struct reader_buffer * rbuf, int start) {

  /*
    struct, struct_decl, union, union_decl
  */

  if((xmlReaderTypes)rbuf->diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "struct") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "struct_decl") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "union") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "union_decl") == 0)
    return true;

  return false;
}

bool is_preprocessor_related(struct reader_buffer * rbuf, int start) {

  /*
    cpp:directive, cpp:file, cpp:include, cpp:define, cpp:undef, cpp:line,
    cpp:if, cpp:ifdef, cpp:ifndef, cpp:else, cpp:elif, cpp:endif, cpp:then, cpp:pragma, cpp:error
  */
  if((xmlReaderTypes)rbuf->diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;


  return false;

}

