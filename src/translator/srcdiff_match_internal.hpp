#ifndef INCLUDED_SRCDIFF_MATCH_INTERNAL
#define INCLUDED_SRCDIFF_MATCH_INTERNAL

#include <construct.hpp>
#include <srcdiff_measure.hpp>
#include <srcml_nodes.hpp>
#include <memory>

std::string get_decl_name(const srcml_nodes & nodes, int start_pos);
std::vector<std::string> get_call_name(const srcml_nodes & nodes, int start_pos);
std::string get_class_type_name(const srcml_nodes & nodes, int start_pos);
bool is_single_call_expr(const srcml_nodes & nodes, int start_pos);
std::string get_for_condition(const srcml_nodes & nodes, int start_pos);
int name_list_similarity(std::vector<std::string> name_list_original, std::vector<std::string> name_list_modified);
std::shared_ptr<construct> get_first_child(const construct & set);
bool is_child_if(const construct & child);
bool if_stmt_has_else(const construct & set);
bool conditional_has_block(const construct & set);
std::shared_ptr<construct> get_first_child(const construct & set);
std::string get_condition(const srcml_nodes & nodes, int start_pos);
bool if_block_equal(const construct & set_original, const construct & set_modified);
std::string get_case_expr(const srcml_nodes & nodes, int start_pos);
bool for_control_matches(const construct & set_original, const construct & set_modified);
bool is_single_name_expr(const srcml_nodes & nodes, int start_pos);
std::shared_ptr<construct> get_first_expr_child(const srcml_nodes & nodes, int start_pos);

#endif
