#ifndef INCLUDED_FUNCTION_PROFILE_T_HPP
#define INCLUDED_FUNCTION_PROFILE_T_HPP

#include <profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <conditional_profile_t.hpp>
#include <if_profile_t.hpp>
#include <call_profile_t.hpp>
#include <identifier_profile_t.hpp>
#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>
#include <text_summary.hpp>
#include <table_summary.hpp>
#include <move_handler.hpp>
#include <summary_manip.hpp>
#include <summary_list.hpp>

#include <cctype>

class class_profile_t;

class function_profile_t : public profile_t {

    private:

    public:

        std::shared_ptr<profile_t> return_type;
        versioned_string name;

        std::multimap<srcdiff_type, std::string> specifiers;
        std::vector<std::shared_ptr<parameter_profile_t>> parameters;
        change_entity_map<call_profile_t> member_initializations;

        size_t total_statements;
        int cyclomatic_complexity_change;

        change_entity_map<class_profile_t> local_classes;

    private:

    public:

        function_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent)
            : profile_t(type_name, uri, operation, parent), total_statements(0), cyclomatic_complexity_change(0) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(is_function_type(*parent)) this->name = name;

        }

        virtual void add_descendant_change(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

            const std::string type_name = profile->type_name.is_common() ? std::string(profile->type_name) : profile->type_name.original();

            if(is_parameter(type_name)) parameters.push_back(reinterpret_cast<const std::shared_ptr<parameter_profile_t> &>(profile));
            else if(is_call(type_name) && parent == "member_init_list") member_initializations.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<call_profile_t> &>(profile));
            else if(is_specifier(type_name) && is_function_type(parent)) specifiers.emplace(profile->operation, profile->raw);
            else if(is_class_type(type_name)) local_classes.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<class_profile_t> &>(profile));

            descendant_change_profiles.insert(std::lower_bound(descendant_change_profiles.begin(), descendant_change_profiles.end(), profile), profile);

        }

        void increment_cyclomatic_complexity_change() {

            ++cyclomatic_complexity_change;

        }

        void decrement_cyclomatic_complexity_change() {

            --cyclomatic_complexity_change;

        }

        std::string get_cyclomatic_complexity_change() const {

            if(cyclomatic_complexity_change > 0)  return "+" + std::to_string(cyclomatic_complexity_change);
            return std::to_string(cyclomatic_complexity_change);

        }

        std::string get_other_change_count() const {

            bool is_name_change = !name.is_common();

            bool is_return_type_change = bool(return_type) && return_type->syntax_count > 0;

            size_t number_specifier_operations = specifiers.size();

            size_t number_parameters = parameters.size();

            size_t number_member_initializations_deleted = 0, number_member_initializations_inserted = 0, number_member_initializations_modified = 0;
            member_initializations.count_operations(number_member_initializations_deleted, number_member_initializations_inserted, number_member_initializations_modified);

            size_t count = (is_name_change ? 1 : 0) + (is_return_type_change ? 1 : 0) + number_specifier_operations + number_parameters
                + number_member_initializations_deleted + number_member_initializations_inserted + number_member_initializations_modified;

            return std::to_string(count);

        }

        /** @todo may need to add rest of things that can occur here between parameter list and block */
        virtual summary_output_stream & summary(summary_output_stream & out, size_t summary_types) const {

            if(operation != SRCDIFF_COMMON) {

                out << '\'' << (name.has_original() ? name.original() : name.modified()) << '\'';
                return out;

            }


            size_t statement_count = this->statement_count, statement_churn = this->statement_churn;
            move_handler m_handler(statement_count, statement_churn);
            m_handler.gather_candidates(descendant_change_profiles);
            m_handler.detect();

            out.begin_line() << type_name << " '" << name << "':\n";
            out.pad() << "  ";
            out << manip::bold() << "Impact" << manip::normal() << ": ";
            out << manip::bold() << "Statement"     << manip::normal() << " = " << manip::bold() << this->statement_churn              << manip::normal();
            out << '\t';
            out << manip::bold() << "Non-Statement" << manip::normal() << " = " << manip::bold() << get_other_change_count()           << manip::normal();
            out << '\t';
            out << manip::bold() << "Cyclomatic"    << manip::normal() << " = " << manip::bold() << get_cyclomatic_complexity_change() << manip::normal();            
            out << '\n';

            out.increment_depth();

            if(syntax_count == 0) {

                size_t non_syntax_changes = whitespace_count + comment_count;

                out.begin_line() << "only ";

                if(non_syntax_changes == 1) out << "a single ";

                out << (whitespace_count != 0 ? "whitespace " : "");

                out << (whitespace_count != 0 && comment_count != 0 ? "and " : "");

                out << (comment_count != 0 ? "comment " : "") << "change";

                out << (non_syntax_changes == 1 ? "\n" : "s\n");

                out.decrement_depth();

                return out;

            }
          
            // get counts and set flags
            bool is_return_type_change = bool(return_type) && return_type->syntax_count > 0;

            size_t number_member_initializations_deleted = 0, number_member_initializations_inserted = 0, number_member_initializations_modified = 0;
            member_initializations.count_operations(number_member_initializations_deleted, number_member_initializations_inserted, number_member_initializations_modified);

            if(is_summary_type(summary_types, summary_type::TEXT)) {

                text_summary text;

                if(!name.is_common()) out.begin_line() << manip::bold() << "function name change" << manip::normal()
                                                       << " from '" <<name.original() << "' to '" << name.modified() << "'\n";

                if(is_return_type_change) {

                    out.begin_line() << manip::bold() << "return type change" << manip::normal() << '\n';

                }

                text.parameter(out, parameters);

                for(std::map<srcdiff_type, std::string>::const_iterator citr = specifiers.lower_bound(SRCDIFF_DELETE); citr != specifiers.upper_bound(SRCDIFF_DELETE); ++citr)
                    out.begin_line() << manip::bold() << citr->second << manip::normal() << " specifier was deleted\n";
                for(std::map<srcdiff_type, std::string>::const_iterator citr = specifiers.lower_bound(SRCDIFF_INSERT); citr != specifiers.upper_bound(SRCDIFF_INSERT); ++citr)
                    out.begin_line() << manip::bold() << citr->second << manip::normal() << " specifier was inserted\n";              
                for(std::map<srcdiff_type, std::string>::const_iterator citr = specifiers.lower_bound(SRCDIFF_COMMON); citr != specifiers.upper_bound(SRCDIFF_COMMON); ++citr)
                    out.begin_line() << manip::bold() << citr->second << manip::normal() << " specifier was modified\n";

                if(is_summary_type(summary_types, summary_type::TEXT) && (number_member_initializations_deleted || number_member_initializations_inserted || number_member_initializations_modified))
                    text.member_initialization(out, number_member_initializations_deleted, number_member_initializations_inserted, number_member_initializations_modified);

                summary_list list;
                list.function_body(*this);
                text.function_body(out, list.summaries());

            }

            local_classes.summarize_pure(out, summary_types, SRCDIFF_DELETE);
            local_classes.summarize_pure(out, summary_types, SRCDIFF_INSERT);
            local_classes.summarize_modified(out, summary_types);

            out.decrement_depth();

            return out;

        }

};

#endif
