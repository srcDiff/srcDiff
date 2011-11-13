#ifndef INCLUDED_SRCDIFFUTILITY_HPP
#define INCLUDED_SRCDIFFUTILITYS_HPP

#include "shortest_edit_script.h"

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

bool is_change(struct edit * edit_script);

// diff node accessor function
const void * node_set_index(int idx, const void *s);

bool attribute_compare(xmlAttrPtr attr1, xmlAttrPtr attr2);

// diff node comparison function
int node_compare(xmlNode * node1, xmlNode * node2);

bool is_white_space(xmlNodePtr node);

bool is_new_line(xmlNodePtr node);

bool is_text(xmlNodePtr node);

// diff node comparison function
int node_set_syntax_compare(const void * e1, const void * e2);

#endif
