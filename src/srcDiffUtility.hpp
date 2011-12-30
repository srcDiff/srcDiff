#ifndef INCLUDED_SRCDIFFUTILITY_HPP
#define INCLUDED_SRCDIFFUTILITYS_HPP

#include "shortest_edit_script.h"

#include "xmlrw.hpp"

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

bool is_change(struct edit * edit_script);

// diff node accessor function
const void * node_set_index(int idx, const void *s, const void * context);

const void * node_index(int idx, const void *s, const void * context);

int node_index_compare(const void * node1, const void * node2, const void * context);

bool attribute_compare(xAttr * attr1, xAttr * attr2);

// diff node comparison function
int node_compare(xNode * node1, xNode * node2);

bool is_white_space(xNodePtr node);

bool is_new_line(xNodePtr node);

bool is_text(xNodePtr node);

// diff node comparison function
int node_set_syntax_compare(const void * e1, const void * e2, const void * context);

#endif
