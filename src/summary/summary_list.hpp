#ifndef INCLUDED_SUMMARY_LIST_HPP
#define INCLUDED_SUMMARY_LIST_HPP

#include <profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <call_profile_t.hpp>
#include <expr_stmt_profile_t.hpp>
#include <change_entity_map.hpp>
#include <summary_t.hpp>

#include <cstdlib>
#include <vector>
#include <memory>
#include <map>
#include <list>

class summary_list {

public:

private:

protected:

    std::list<summary_t *> summaries_;
    std::set<versioned_string> output_identifiers;

private:

    std::string get_type_string(const std::shared_ptr<profile_t> & profile) const;

    bool is_block_summary(const std::string & type, bool is_replacement) const;
    void statement_dispatch(const std::shared_ptr<profile_t> & profile, size_t & child_pos);
    void block(const std::shared_ptr<profile_t> & profile);

    void identifiers(std::map<std::string, std::set<versioned_string>> profile_declarations,
                     std::map<std::string, std::map<versioned_string, size_t>> profile_identifiers);
    void replacement(const std::shared_ptr<profile_t> & profile, size_t & pos);
    void interchange(const std::shared_ptr<profile_t> & profile);

    void jump(const std::shared_ptr<profile_t> & profile);
    void else_clause(const std::shared_ptr<profile_t> & profile);
    void conditional(const std::shared_ptr<profile_t> & profile);
    bool identifier_check(const std::shared_ptr<profile_t> & profile, const std::set<versioned_string> & identifier_set,
                          std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const;
    void ternary(const std::shared_ptr<profile_t> & profile, const std::set<versioned_string> & identifier_set,
                 bool & condition_modified, bool & then_clause_modified, bool & else_clause_modified,
                 std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const;
    void expr_statistics(const std::shared_ptr<profile_t> & profile, const std::set<versioned_string> & identifier_set,
                         std::vector<std::shared_ptr<call_profile_t>> & deleted_calls,
                         std::vector<std::shared_ptr<call_profile_t>> & inserted_calls,
                         std::vector<std::shared_ptr<call_profile_t>> & modified_calls,
                         std::vector<std::shared_ptr<call_profile_t>> & renamed_calls,
                         std::vector<std::shared_ptr<call_profile_t>> & modified_argument_lists,
                         std::vector<std::shared_ptr<profile_t>> & deleted_other,
                         std::vector<std::shared_ptr<profile_t>> & inserted_other,
                         std::vector<std::shared_ptr<profile_t>> & modified_other,
                         size_t & number_arguments_deleted,
                         size_t & number_arguments_inserted,
                         size_t & number_arguments_modified,
                         bool & identifier_rename_only,
                         std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const;
    void common_expr_stmt(const std::shared_ptr<profile_t> & expr_stmt_profile);
    void call_sequence(const std::shared_ptr<profile_t> & profile, size_t number_rename,
                                          size_t number_arguments_deleted, size_t number_arguments_inserted, size_t numbe_arguments_modified,
                                          size_t number_argument_lists_modified,
                                          bool identifier_rename_only, const std::set<std::reference_wrapper<const versioned_string>> & identifier_renames);
    void expr_stmt(const std::shared_ptr<profile_t> & profile);
    void decl_stmt(const std::shared_ptr<profile_t> & profile);
    void exception(const std::shared_ptr<profile_t> & profile);
    void label(const std::shared_ptr<profile_t> & profile);

public:

    summary_list();
    ~summary_list();

    void function_body(const profile_t & profile);

    const std::list<summary_t *> & summaries() const;
    std::list<summary_t *> & summaries();

};

#endif
