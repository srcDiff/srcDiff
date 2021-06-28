#ifndef INCLUDED_SRCDIFF_MATCH_INTERNAL
#define INCLUDED_SRCDIFF_MATCH_INTERNAL

#include <node_sets.hpp>
#include <srcdiff_measure.hpp>
#include <srcml_nodes.hpp>
#include <memory>

std::string get_name(const srcml_nodes & nodes, int name_start_pos);
std::string get_decl_name(const srcml_nodes & nodes, int start_pos);
std::string extract_name(const srcml_nodes & nodes, int start_pos);
void skip_tag(const srcml_nodes & nodes, int & start_pos);
void top_level_name_seek(const srcml_nodes & nodes, int & start_pos);

#endif

