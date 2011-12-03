#include "srcDiffNested.hpp"
#include "shortest_edit_script.h"
#include "srcDiffWhiteSpace.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffDiff.hpp"

#include <string.h>

// global structures defined in main
extern std::vector<xNode *> nodes_old;
extern std::vector<xNode *> nodes_new;

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

// diff attribute
extern xAttr diff_type;
extern const char * change;
extern const char * whitespace;

// tags that can have something nested in them
const char * block_types[] = { "if", 0 };
//const char * block_types[] = { "block", "if", "while", "for", "function", 0 };

// tags that can be nested in something else (incomplete)
const char * nest_types[] = { "expr_stmt", "decl_stmt", 0 };
// const char * nest_types[] = { "block", "expr_stmt", "decl_stmt", 0 };

bool is_block_type(std::vector<int> * structure, std::vector<xNodePtr> & nodes) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp(nodes.at(structure->at(0))->ns->href, "http://www.sdml.info/srcML/src") != 0)
    return false;

  for(int i = 0; block_types[i]; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, block_types[i]) == 0)
      return true;

  return false;
}

bool is_nest_type(std::vector<int> * structure, std::vector<xNodePtr> & nodes) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

  for(int i = 0; nest_types[i]; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, nest_types[i]) == 0)
      return true;

  return false;
}

bool has_interal_block(std::vector<int> * structure, std::vector<xNodePtr> & nodes) {

  if(strcmp((const char *)nodes.at(structure->at(0))->name, "block") == 0)
    return false;

  for(unsigned int i = 1; i < structure->size(); ++i)
    if(strcmp((const char *)nodes.at(structure->at(i))->name, "block") == 0)
      return true;

  return false;
}

bool is_nestable(std::vector<int> * structure_one, std::vector<xNodePtr> & nodes_one
                 , std::vector<int> * structure_two, std::vector<xNodePtr> & nodes_two) {


  if(is_nest_type(structure_one, nodes_one) && is_block_type(structure_two, nodes_two)) {

    if(strcmp((const char *)nodes_one.at(structure_one->at(0))->name, "block") != 0) {

      return true;

    } else {

      if(has_interal_block(structure_two, nodes_two))
        return true;

    }
  }

  return false;
}

void output_nested(reader_state & rbuf_old, std::vector<int> * structure_old
                   , reader_state & rbuf_new ,std::vector<int> * structure_new
                   , int operation, writer_state & wstate) {

  output_white_space_prefix(rbuf_old, rbuf_new, wstate);

  if(operation == DELETE) {

    unsigned int start;
    unsigned int end;
    unsigned int start_pos;
    unsigned int end_pos;

      for(start = 0; start < structure_old->size()
            && ((xmlReaderTypes)nodes_old.at(structure_old->at(start))->type != XML_READER_TYPE_END_ELEMENT
                || strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "condition") != 0); ++start)
        ;

      ++start;

      start_pos = structure_old->at(start) + 1;
      end_pos = structure_old->back();

      if(strcmp((const char *)nodes_old.at(structure_old->at(0))->name, "if") == 0) {

        ++start_pos;
        --end_pos;

      }

    // output diff tag begin
    //output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);

      //diff_type.value = change;
      //diff_old_start.properties = &diff_type;
      //diff_new_start.properties = &diff_type;
      
    output_change(rbuf_old, start_pos, rbuf_new, rbuf_new.last_output, wstate);

    output_white_space_suffix(rbuf_old, rbuf_new, wstate);
    //for(unsigned int i = rbuf_old.last_output; i < start_pos; ++i)
    //output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);
    //rbuf_old.last_output = start_pos;

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(nodes_old, start_pos
                        , end_pos);

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(nodes_new,  structure_new->at(0)
                        , structure_new->back() + 1);

    output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    output_white_space_nested(rbuf_old, rbuf_new, DELETE, wstate);
    //markup_common(rbuf_old, end_pos, rbuf_new, rbuf_new.last_output, wstate);

    output_change(rbuf_old,  structure_old->back() + 1, rbuf_new, rbuf_new.last_output, wstate);
    //for(unsigned int i = end_pos; i < (structure_old->back() + 1); ++i)
    //output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    output_white_space_all(rbuf_old, rbuf_new, wstate);

    // output diff tag begin
    //output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

    //rbuf_new.last_output = structure_new->back() + 1;

    //markup_common(rbuf_old, rbuf_old.last_output, rbuf_new, rbuf_new.last_output, wstate);

  } else {

    unsigned int start;
    unsigned int end;
    unsigned int start_pos;
    unsigned int end_pos;

      for(start = 0; start < structure_new->size()
            && ((xmlReaderTypes)nodes_new.at(structure_new->at(start))->type != XML_READER_TYPE_END_ELEMENT
                || strcmp((const char *)nodes_new.at(structure_new->at(start))->name, "condition") != 0); ++start)
        ;

      ++start;

      start_pos = structure_new->at(start) + 1;
      end_pos = structure_new->back();

      if(strcmp((const char *)nodes_new.at(structure_new->at(0))->name, "if") == 0) {

        ++start_pos;
        --end_pos;

      }

      //diff_type.value = change;
      //diff_old_start.properties = &diff_type;
      //diff_new_start.properties = &diff_type;


    output_change(rbuf_old, rbuf_old.last_output, rbuf_new, start_pos, wstate);
    //for(unsigned int i = rbuf_old.last_output; i < start_pos; ++i)
    //output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    //rbuf_old.last_output = start_pos;
    output_white_space_suffix(rbuf_old, rbuf_new, wstate);

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(nodes_old,  structure_old->at(0)
                        , structure_old->back() + 1);

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(nodes_new, start_pos
                        , end_pos);

    output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    output_white_space_nested(rbuf_old, rbuf_new, INSERT, wstate);
    //markup_common(rbuf_old, end_pos, rbuf_new, rbuf_new.last_output, wstate);

    output_change(rbuf_old,  rbuf_old.last_output, rbuf_new, structure_new->back() + 1, wstate);
    //for(unsigned int i = end_pos; i < (structure_old->back() + 1); ++i)
    //output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    output_white_space_all(rbuf_old, rbuf_new, wstate);

    // output diff tag begin
    //output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

    //rbuf_new.last_output = structure_new->back() + 1;

    //markup_common(rbuf_old, rbuf_old.last_output, rbuf_new, rbuf_new.last_output, wstate);

  }

  //diff_old_start.properties = 0;
  //diff_new_start.properties = 0;

}

/*
void output_nested(reader_state & rbuf_old, std::vector<int> * structure_old
                   , reader_state & rbuf_new ,std::vector<int> * structure_new
                   , int operation, writer_state & wstate) {


  if(operation == DELETE) {

    // may need to markup common that does not output common blocks
    output_white_space_all(rbuf_old, rbuf_new, wstate);
    //markup_common(rbuf_old, structure_old->at(0), rbuf_new, rbuf_new.last_output, wstate);

    unsigned int start;
    unsigned int end;
    unsigned int start_pos;
    unsigned int end_pos;
    if(has_interal_block(structure_old, nodes_old) || strcmp((const char *)nodes_old.at(structure_old->at(0))->name, "block") == 0) {

      for(start = 0; start < structure_old->size() && strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "block") != 0; ++start)
        ;

      for(end = start + 1; end < structure_old->size() && strcmp((const char *)nodes_old.at(structure_old->at(end))->name, "block") != 0; ++end)
        ;

      start_pos = structure_old->at(start);
      end_pos = structure_old->at(end) - 1;

      if(strcmp((const char *)nodes_new.at(structure_new->at(0))->name, "block") != 0)
        start_pos += 2;
      else
        end_pos += 2;

    } else if(strcmp((const char *)nodes_old.at(structure_old->at(0))->name, "for") != 0){

      for(start = 0; start < structure_old->size()
            && ((xmlReaderTypes)nodes_old.at(structure_old->at(start))->type != XML_READER_TYPE_END_ELEMENT
                || strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "condition") != 0); ++start)
        ;

      ++start;

      start_pos = structure_old->at(start) + 1;
      end_pos = structure_old->back();

      if(strcmp((const char *)nodes_old.at(structure_old->at(0))->name, "if") == 0) {

        ++start_pos;
        --end_pos;

      }

    } else {

      for(start = 0; start < structure_old->size() && strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "incr") != 0; ++start)
        ;

      if(!(nodes_old.at(start)->extra & 0x1))
        for(start = 0; start < structure_old->size() && strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "incr") != 0; ++start)
          ;

      start += 3;
      start_pos = structure_old->at(start) + 1;
      end_pos = structure_old->back();

    }

    // output diff tag begin
    //output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);

    output_change(rbuf_old, start_pos, rbuf_new, rbuf_new.last_output, wstate);
    //for(unsigned int i = rbuf_old.last_output; i < start_pos; ++i)
    //output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);
    //rbuf_old.last_output = start_pos;

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(nodes_old, start_pos
                        , end_pos);

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(nodes_new,  structure_new->at(0)
                        , structure_new->back() + 1);

    output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    output_white_space_nested(rbuf_old, rbuf_new, DELETE, wstate);
    //markup_common(rbuf_old, end_pos, rbuf_new, rbuf_new.last_output, wstate);

    output_change(rbuf_old,  structure_old->back() + 1, rbuf_new, rbuf_new.last_output, wstate);
    //for(unsigned int i = end_pos; i < (structure_old->back() + 1); ++i)
    //output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    rbuf_old.last_output = structure_old->back() + 1;

    output_white_space_all(rbuf_old, rbuf_new, wstate);

    // output diff tag begin
    //output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

    //rbuf_new.last_output = structure_new->back() + 1;

    //markup_common(rbuf_old, rbuf_old.last_output, rbuf_new, rbuf_new.last_output, wstate);

  } else {

    // may need to markup common that does not output common blocks
    output_white_space_all(rbuf_old, rbuf_new, wstate);
    //markup_common(rbuf_old, structure_old->at(0), rbuf_new, rbuf_new.last_output, wstate);

    unsigned int start;
    unsigned int end;
    unsigned int start_pos;
    unsigned int end_pos;
    if(has_interal_block(structure_new, nodes_new) || strcmp((const char *)nodes_new.at(structure_new->at(0))->name, "block") == 0) {

      for(start = 0; start < structure_new->size() && strcmp((const char *)nodes_new.at(structure_new->at(start))->name, "block") != 0; ++start)
        ;

      for(end = start + 1; end < structure_new->size() && strcmp((const char *)nodes_new.at(structure_new->at(end))->name, "block") != 0; ++end)
        ;

      start_pos = structure_new->at(start);
      end_pos = structure_new->at(end) - 1;

      if(strcmp((const char *)nodes_old.at(structure_old->at(0))->name, "block") != 0)
        start_pos += 2;
      else
        end_pos += 2;

    } else if(strcmp((const char *)nodes_new.at(structure_new->at(0))->name, "for") != 0){

      for(start = 0; start < structure_new->size()
            && ((xmlReaderTypes)nodes_new.at(structure_new->at(start))->type != XML_READER_TYPE_END_ELEMENT
                || strcmp((const char *)nodes_new.at(structure_new->at(start))->name, "condition") != 0); ++start)
        ;

      ++start;

      start_pos = structure_new->at(start) + 1;
      end_pos = structure_new->back();

      if(strcmp((const char *)nodes_new.at(structure_new->at(0))->name, "if") == 0) {

        ++start_pos;
        --end_pos;

      }

    } else {

      for(start = 0; start < structure_new->size() && strcmp((const char *)nodes_new.at(structure_new->at(start))->name, "incr") != 0; ++start)
        ;

      if(!(nodes_new.at(start)->extra & 0x1))
        for(start = 0; start < structure_new->size() && strcmp((const char *)nodes_new.at(structure_new->at(start))->name, "incr") != 0; ++start)
          ;

      start += 3;
      start_pos = structure_new->at(start) + 1;
      end_pos = structure_new->back();

    }

    output_change(rbuf_old, rbuf_old.last_output, rbuf_new, start_pos, wstate);
    //for(unsigned int i = rbuf_old.last_output; i < start_pos; ++i)
    //output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    //rbuf_old.last_output = start_pos;

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(nodes_old,  structure_old->at(0)
                        , structure_old->back() + 1);

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(nodes_new, start_pos
                        , end_pos);

    output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    output_white_space_nested(rbuf_old, rbuf_new, INSERT, wstate);
    //markup_common(rbuf_old, end_pos, rbuf_new, rbuf_new.last_output, wstate);

    output_change(rbuf_old,  rbuf_old.last_output, rbuf_new, structure_new->back() + 1, wstate);
    //for(unsigned int i = end_pos; i < (structure_old->back() + 1); ++i)
    //output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    rbuf_new.last_output = structure_new->back() + 1;

    output_white_space_all(rbuf_old, rbuf_new, wstate);

    // output diff tag begin
    //output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

    //rbuf_new.last_output = structure_new->back() + 1;

    //markup_common(rbuf_old, rbuf_old.last_output, rbuf_new, rbuf_new.last_output, wstate);

  }
}
*/
