#ifndef INCLUDED_FUNCTION_PROFILE_T_HPP
#define INCLUDED_FUNCTION_PROFILE_T_HPP

#include <profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <conditional_profile_t.hpp>
#include <if_profile_t.hpp>
#include <call_profile_t.hpp>
#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>
#include <text_summary.hpp>
#include <table_summary.hpp>
#include <move_handler.hpp>
#include <summary_manip.hpp>
#include <summary_list.hpp>

#include <cctype>

class function_profile_t : public profile_t {

    private:

    public:

        versioned_string return_type;
        versioned_string name;

        boost::optional<srcdiff_type> const_specifier;

        change_entity_map<conditional_profile_t> conditionals;
        change_entity_map<parameter_profile_t>   parameters;
        change_entity_map<call_profile_t>        member_initializations;

        size_t total_statements;
        int cyclomatic_complexity_change;

    private:

    public:

        function_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent)
            : profile_t(type_name, uri, operation, parent), total_statements(0), cyclomatic_complexity_change(0) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(*parent == "type") return_type = name;
            else if(is_function_type(*parent)) this->name = name;

        }

        virtual void add_descendant(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

            const std::string type_name = profile->type_name.is_common() ? std::string(profile->type_name) : profile->type_name.original();

            if(is_parameter(type_name)) parameters.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<parameter_profile_t> &>(profile));
            else if(is_condition_type(type_name)) conditionals.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile));
            else if(is_call(type_name) && parent == "member_init_list") member_initializations.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<call_profile_t> &>(profile));
            else if(is_specifier(type_name) && parent == "function") const_specifier = profile->operation;
            
            descendant_profiles.insert(std::lower_bound(descendant_profiles.begin(), descendant_profiles.end(), profile), profile);

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

            bool is_return_type_change = !return_type.is_common();

            size_t number_parameters_deleted = 0, number_parameters_inserted = 0, number_parameters_modified = 0;
            parameters.count_operations(number_parameters_deleted, number_parameters_inserted, number_parameters_modified);

            size_t number_member_initializations_deleted = 0, number_member_initializations_inserted = 0, number_member_initializations_modified = 0;
            member_initializations.count_operations(number_member_initializations_deleted, number_member_initializations_inserted, number_member_initializations_modified);

            size_t count = (is_return_type_change ? 1 : 0) + number_parameters_deleted + number_parameters_inserted + number_parameters_modified
                + number_member_initializations_deleted + number_member_initializations_inserted + number_member_initializations_modified;

            return std::to_string(count);

        }

        /** @todo may need to add rest of things that can occur here between parameter list and block */
        virtual summary_output_stream & summary(summary_output_stream & out, size_t summary_types) const {

            if(operation != SRCDIFF_COMMON) {

                out << '\'' << (name.has_original() ? name.original() : name.modified()) << '\'';
                return out;

            }

            out.begin_line() << type_name << " '" << name << "':\n";
            out.pad() << "  ";
            out << manip::bold() << "Impact" << manip::normal() << " = " << manip::bold() << statement_churn << manip::normal() << " Statement" << (statement_churn == 1 ? "" : "s");
            out << '\t';
            out << manip::bold() << "Other" << manip::normal() << " = " << manip::bold() << get_other_change_count() << manip::normal();
            out << '\t';
            out << manip::bold() << "Cyclomatic" << manip::normal() << " = " << manip::bold() << get_cyclomatic_complexity_change() << manip::normal();            
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
            bool is_return_type_change = !return_type.is_common();
            size_t number_parameters_deleted = 0, number_parameters_inserted = 0, number_parameters_modified = 0;
            parameters.count_operations(number_parameters_deleted, number_parameters_inserted, number_parameters_modified);

            size_t number_member_initializations_deleted = 0, number_member_initializations_inserted = 0, number_member_initializations_modified = 0;
            member_initializations.count_operations(number_member_initializations_deleted, number_member_initializations_inserted, number_member_initializations_modified);

            if(is_summary_type(summary_types, summary_type::TEXT)) {

                text_summary text;

                if(!name.is_common()) out.begin_line() << "name changed: " << name.original() << " -> " << name.modified() << '\n';

                //if(is_return_type_change || number_parameters_deleted || number_parameters_inserted || number_parameters_modified) out.begin_line() << "Signature change:\n";

                if(is_return_type_change) out.begin_line() << "return type changed: " << return_type.original() << " -> " << return_type.modified() << '\n';

                if(number_parameters_deleted || number_parameters_inserted || number_parameters_modified)
                    text.parameter(out, number_parameters_deleted, number_parameters_inserted, number_parameters_modified);

                if(const_specifier) out.begin_line() << (*const_specifier == SRCDIFF_DELETE ? "deleted " : (*const_specifier == SRCDIFF_INSERT ? "inserted " : "moved ")) << "const specifier \n";

                if(is_summary_type(summary_types, summary_type::TEXT) && (number_member_initializations_deleted || number_member_initializations_inserted || number_member_initializations_modified))
                    text.member_initialization(out, number_member_initializations_deleted, number_member_initializations_inserted, number_member_initializations_modified);

                move_handler m_handler;
                m_handler.gather_candidates(descendant_profiles);
                m_handler.detect();

                summary_list list;
                list.function_body(*this);
                text.function_body(out, list.summaries());

            }

            if(is_summary_type(summary_types, summary_type::TABLE)) {

                table_summary table(conditionals);

                if(number_parameters_deleted || number_parameters_inserted || number_parameters_modified)
                    table.output_all_parameter_counts(out, number_parameters_deleted, number_parameters_inserted, number_parameters_modified);

                if(is_summary_type(summary_types, summary_type::TABLE) && (number_member_initializations_deleted || number_member_initializations_inserted || number_member_initializations_modified))
                    table.output_all_member_initialization_counts(out, number_member_initializations_deleted, number_member_initializations_inserted, number_member_initializations_modified);

                size_t number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified = 0;
                conditionals.count_operations(number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified);
                if(number_conditionals_deleted || number_conditionals_inserted || number_conditionals_modified)
                    table.output_all_conditional_counts(out, number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified);

            }

            out.decrement_depth();

            return out;

        }

};

#endif
