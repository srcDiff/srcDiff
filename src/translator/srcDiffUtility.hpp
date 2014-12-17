#ifndef INCLUDED_SRCDIFFUTILITY_HPP
#define INCLUDED_SRCDIFFUTILITY_HPP

#include <shortest_edit_script.h>

#include <srcDiffOptions.hpp>
#include <node_sets.hpp>
#include <xmlrw.hpp>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

bool is_change(edit * edit_script);

// diff node accessor function
const void * node_set_index(int idx, const void *s, const void * context);

const void * node_index(int idx, const void *s, const void * context);

int node_index_compare(const void * node1, const void * node2, const void * context);

bool attribute_compare(xAttr * attr1, xAttr * attr2);

// diff node comparison function
int node_compare(xNode * node1, xNode * node2);

bool is_white_space(const xNodePtr node);

bool is_new_line(const xNodePtr node);

bool is_text(const xNodePtr node);

// diff node comparison function
int node_set_syntax_compare(const void * e1, const void * e2, const void * context);

const char * find_attribute(const xNodePtr node, const char * attr_name);

bool is_interchangeable_match(const std::string & old_tag, const std::string & new_tag);

bool reject_match(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new);

bool reject_similarity(int similarity, int difference, int text_old_length, int text_new_length,
  std::vector<xNodePtr> & nodes_old, node_set * set_old, std::vector<xNodePtr> & nodes_new, node_set * set_new);
#endif
