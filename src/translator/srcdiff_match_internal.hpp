#ifndef INCLUDED_SRCDIFF_MATCH_INTERNAL
#define INCLUDED_SRCDIFF_MATCH_INTERNAL

#include <construct.hpp>
#include <srcdiff_measure.hpp>
#include <srcml_nodes.hpp>
#include <memory>

std::string get_decl_name(const srcml_nodes & nodes, int start_pos);
std::vector<std::string> get_call_name(const srcml_nodes & nodes, int start_pos);
bool is_single_call_expr(const srcml_nodes & nodes, int start_pos);
int name_list_similarity(std::vector<std::string> name_list_original, std::vector<std::string> name_list_modified);
std::shared_ptr<construct> get_first_child(const construct & set);
std::string get_case_expr(const srcml_nodes & nodes, int start_pos);
bool is_single_name_expr(const srcml_nodes & nodes, int start_pos);
std::shared_ptr<construct> get_first_expr_child(const srcml_nodes & nodes, int start_pos);

#endif
