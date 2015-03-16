#ifndef INCLUDED_TYPE_QUERY_HPP
#define INCLUDED_TYPE_QUERY_HPP

#include <string>

bool is_simple_type(const std::string & type_name);
bool is_function_type(const std::string & type_name);
bool is_class_type(const std::string & type_name);
bool is_template(const std::string & type_name);
bool is_condition_type(const std::string & type_name);
bool is_catch(const std::string & type_name);
bool is_call(const std::string & type_name);
bool is_decl_stmt(const std::string & type_name);
bool is_preprocessor_special(const std::string & type_name);
bool is_expr(const std::string & type_name);
bool is_lambda(const std::string & type_name);
bool is_parameter(const std::string & type_name);
bool is_specifier(const std::string & type_name);
bool is_expr_stmt(const std::string & type_name);
bool is_identifier(const std::string & type_name);
bool is_argument(const std::string & type_name);

bool has_then_clause(const std::string & type_name);
bool has_body(const std::string & type_name);

#endif
