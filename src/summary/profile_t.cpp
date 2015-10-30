#include <profile_t.hpp>

#include <class_profile_t.hpp>
#include <function_profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <decl_stmt_profile_t.hpp>
#include <unit_profile_t.hpp>

#include <type_query.hpp>

std::shared_ptr<profile_t> profile_t::unit_profile;

profile_t::profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation) :
    id(0), type_name(type_name), uri(uri), operation(operation), is_replace(false), move_id(0),
    parent(), summary_parent(), body(), summary_profile(), statement_count_original(0), statement_count_modified(0), statement_count(0), statement_churn(0), common_statements(0),
    is_modified(false), is_whitespace(false), is_comment(false), is_syntax(false),
    modified_count(0), whitespace_count(0), comment_count(0), syntax_count(0), total_count(0),
    left_hand_side(false), right_hand_side(false), raw() {}

profile_t::profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & summary_parent) :
    id(0), type_name(type_name), uri(uri), operation(operation), is_replace(false), move_id(0),
    parent(), summary_parent(summary_parent), body(), summary_profile(), statement_count_original(0), statement_count_modified(0), statement_count(0), statement_churn(0), common_statements(0),
    is_modified(false), is_whitespace(false), is_comment(false), is_syntax(false),
    modified_count(0), whitespace_count(0), comment_count(0), syntax_count(0), total_count(0),
    left_hand_side(false), right_hand_side(false), raw() {}

void profile_t::set_id(size_t id_count) {

    id = id_count;

}

void profile_t::set_operation(srcdiff_type operation) {

    this->operation = operation;
                
}

/** @todo need to add identifiers as soon as name encountered */
void profile_t::add_child(const std::shared_ptr<profile_t> & profile) {

    child_profiles.push_back(profile);

}

static void update_declarations(std::shared_ptr<profile_t> & body, const versioned_string & declaration) {

        if(declaration.is_common()) {

            body->declarations[declaration].insert(declaration);
            reinterpret_cast<std::shared_ptr<unit_profile_t> &>(profile_t::unit_profile)->identifier_to_declaration_profile[declaration].push_back(body);
            return;

        }

        if(declaration.has_original()) {

            body->declarations[declaration.original()].insert(declaration);
            reinterpret_cast<std::shared_ptr<unit_profile_t> &>(profile_t::unit_profile)->identifier_to_declaration_profile[declaration.original()].push_back(body);

        }

        if(declaration.has_modified()) {

            body->declarations[declaration.modified()].insert(declaration);
            reinterpret_cast<std::shared_ptr<unit_profile_t> &>(profile_t::unit_profile)->identifier_to_declaration_profile[declaration.modified()].push_back(body);

        }

}

void profile_t::add_declaration_identifier(const std::shared_ptr<profile_t> & profile) {

    if(is_class_type(profile->type_name)) {

        const std::shared_ptr<class_profile_t> & class_profile = reinterpret_cast<const std::shared_ptr<class_profile_t> &>(profile);
        update_declarations(profile->parent->body, class_profile->name);

    } else if(is_function_type(profile->type_name)) {

        const std::shared_ptr<function_profile_t> & function_profile = reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile);
        update_declarations(profile->parent->body, function_profile->name);

    } else if(is_parameter(profile->type_name)) {

        const std::shared_ptr<parameter_profile_t> & parameter_profile = reinterpret_cast<const std::shared_ptr<parameter_profile_t> &>(profile);
        update_declarations(body, parameter_profile->name);

    } else if(is_decl_stmt(profile->type_name)) {

        const std::shared_ptr<decl_stmt_profile_t> & decl_stmt_profile = reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile);
        update_declarations(body, decl_stmt_profile->name);

    }

}

void profile_t::set_name(const std::shared_ptr<identifier_profile_t> & name) {

    set_name(name, boost::optional<versioned_string>());

}

void profile_t::set_name(const std::shared_ptr<identifier_profile_t> & name UNUSED, const boost::optional<versioned_string> & parent UNUSED) {}

void profile_t::add_child_change(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED) {

    child_change_profiles.insert(std::lower_bound(child_change_profiles.begin(), child_change_profiles.end(), profile), profile);

}

void profile_t::add_descendant_change(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED) {

    descendant_change_profiles.insert(std::lower_bound(descendant_change_profiles.begin(), descendant_change_profiles.end(), profile), profile);
    
}

profile_t::impact_factor profile_t::calculate_impact_factor() const {

    double impact_factor_number = (double)syntax_count / descendant_change_profiles.size();

    if(impact_factor_number == 0)    return NONE;
    if(impact_factor_number <  0.1)  return LOW;
    if(impact_factor_number <  0.25) return MEDIUM;
    return HIGH;

}

const char * profile_t::get_impact_factor() const {

    impact_factor factor = calculate_impact_factor();

    switch(factor) {

        case NONE:   return "None";
        case LOW:    return "Low";
        case MEDIUM: return "Medium";
        case HIGH:   return "High";

    }

}

summary_output_stream & profile_t::summary(summary_output_stream & out, size_t summary_types UNUSED) const {

    out.pad() << type_name << ":"; 
    out << " Whitespace: " << whitespace_count;
    out << "\tComment: " << comment_count;
    out << "\tSyntax: " << syntax_count;
    out << "\tTotal: " << total_count;
    out << '\n';

    return out;

}

bool profile_t::operator<(const profile_t & profile) const {

    return id < profile.id;

}

bool operator<(const std::shared_ptr<profile_t> & profile_one, const std::shared_ptr<profile_t> & profile_two) {

    return profile_one->id < profile_two->id;

}

bool profile_t::operator==(const profile_t & profile) const {

    return id == profile.id;

}

bool operator==(const std::shared_ptr<profile_t> & profile_one, const std::shared_ptr<profile_t> & profile_two) {

    return profile_one->id == profile_two->id;

}

bool profile_t::operator==(int profile_id) const {

    return id == profile_id;

}

bool operator==(const std::shared_ptr<profile_t> & profile_one, size_t profile_id) {

    return profile_one->id == profile_id;

}

