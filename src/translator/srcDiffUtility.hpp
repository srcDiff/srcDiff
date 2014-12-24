#ifndef INCLUDED_SRCDIFFUTILITY_HPP
#define INCLUDED_SRCDIFFUTILITY_HPP

#include <shortest_edit_script.h>

#include <srcdiff_options.hpp>
#include <node_sets.hpp>
#include <xmlrw.hpp>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

bool is_change(edit * edit_script);

bool is_white_space(const xNodePtr node);

bool is_new_line(const xNodePtr node);

bool is_text(const xNodePtr node);

const char * find_attribute(const xNodePtr node, const char * attr_name);

bool is_interchangeable_match(const std::string & old_tag, const std::string & new_tag);

#endif
