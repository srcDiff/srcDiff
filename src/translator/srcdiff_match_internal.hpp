#ifndef INCLUDED_SRCDIFF_MATCH_INTERNAL
#define INCLUDED_SRCDIFF_MATCH_INTERNAL

#include <element_list.hpp>
#include <srcdiff_measure.hpp>
#include <srcml_nodes.hpp>
#include <memory>

std::string get_name(const srcml_nodes & nodes, int name_start_pos);
std::string get_decl_name(const srcml_nodes & nodes, int start_pos);
std::string extract_name(const srcml_nodes & nodes, int start_pos);
void skip_tag(const srcml_nodes & nodes, int & start_pos);
std::vector<std::string> get_call_name(const srcml_nodes & nodes, int start_pos);
void top_level_name_seek(const srcml_nodes & nodes, int & start_pos);
std::string get_class_type_name(const srcml_nodes & nodes, int start_pos);
bool is_single_call_expr(const srcml_nodes & nodes, int start_pos);
std::string get_function_type_name(const srcml_nodes & nodes, int start_pos);
std::string get_for_condition(const srcml_nodes & nodes, int start_pos);
int name_list_similarity(std::vector<std::string> name_list_original, std::vector<std::string> name_list_modified);
element_t get_first_child(const element_t & set);
bool conditional_has_block(const element_t & set);
element_t get_first_child(const element_t & set);
std::string get_condition(const srcml_nodes & nodes, int start_pos);
bool for_control_matches(const element_t & set_original, const element_t & set_modified);

#endif
