#ifndef INCLUDED_FUNCTION_PROFILE_T_HPP
#define INCLUDED_FUNCTION_PROFILE_T_HPP

#include <profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>

#include <map>
#include <iomanip>

class function_profile_t : public profile_t {

    private:

    public:

        versioned_string return_type;
        versioned_string name;

        change_entity_map<parameter_profile_t> parameters;
        change_entity_map<profile_t>           conditionals;

    public:

        function_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation) : profile_t(type_name, uri, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(*parent == "type") return_type = name;
            else if(is_function_type(*parent)) this->name = name;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile) {

            const std::string type_name = profile->type_name.is_common() ? std::string(profile->type_name) : profile->type_name.original();

            if(is_parameter(type_name)) parameters.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<parameter_profile_t> &>(profile));
            else if(is_condition_type(type_name)) conditionals.emplace(profile->operation, profile);
            else child_profiles.push_back(profile->id);

        }

        virtual impact_factor calculate_impact_factor() const {

            size_t weak_modification = (return_type.is_common() ? 0 : 1) + (name.is_common() ? 0 : 1) + parameters.count(SRCDIFF_COMMON);

            size_t feature_modifications = parameters.count(SRCDIFF_DELETE) + parameters.count(SRCDIFF_INSERT);

            size_t behaviour_modifications = conditionals.count(SRCDIFF_COMMON) + conditionals.count(SRCDIFF_DELETE) + conditionals.count(SRCDIFF_INSERT);

            if((weak_modification + feature_modifications + behaviour_modifications) == 0) return NONE;
            if((feature_modifications + behaviour_modifications) ==  0)                    return LOW;
            if(feature_modifications < 3 && behaviour_modifications < 2)                   return MEDIUM;
            return HIGH;

        }

        virtual void conditional_counts(srcdiff_type operation, size_t & if_count, size_t & while_count, size_t & for_count,
                                        size_t & switch_count, size_t & do_count, size_t & foreach_count) const {

            std::for_each(conditionals.lower_bound(operation), conditionals.upper_bound(operation),

                [&](const change_entity_map<profile_t>::pair & pair) {

                    if(operation == SRCDIFF_COMMON && pair.second->syntax_count == 0) return;

                    const std::string & type_name = operation == SRCDIFF_INSERT ? pair.second->type_name.modified() : pair.second->type_name.original();

                    switch(type_name[0]) {

                        case 'i': ++if_count;     break;
                        case 'w': ++while_count;  break;
                        case 's': ++switch_count; break;
                        case 'd': ++do_count;     break;
                        case 'f': type_name == "for" ? ++for_count : ++foreach_count;   break;

                    }

                });


        }

        virtual std::ostream & output_header(std::ostream & out) const {

            return pad(out) << std::setw(10) << std::left << "" << std::right << std::setw(9) << "Deleted" << std::setw(9) << "Inserted" << std::setw(9) << "Modified" << '\n';

        }

        virtual std::ostream & output_counts(std::ostream & out, const std::string & type, size_t deleted_count, size_t inserted_count, size_t modified_count) const {

            pad(out) << std::setw(10) << std::left;
            out << type;
            out << std::right;
            out << std::setw(9) << deleted_count;
            out << std::setw(9) << inserted_count;
            out << std::setw(9) << modified_count;
            out << '\n';

            return out;
        }

        virtual std::ostream & output_all_conditional_counts(std::ostream & out, size_t number_deleted, size_t number_inserted, size_t number_modified) const {

            pad(out) << "Testing complexity change summary:\n";

            size_t if_deleted = 0, while_deleted = 0, for_deleted = 0, switch_deleted = 0, do_deleted = 0, foreach_deleted = 0;
            conditional_counts(SRCDIFF_DELETE, if_deleted, while_deleted, for_deleted, switch_deleted, do_deleted, foreach_deleted);

            size_t if_inserted = 0, while_inserted = 0, for_inserted = 0, switch_inserted = 0, do_inserted = 0, foreach_inserted = 0;
            conditional_counts(SRCDIFF_INSERT, if_inserted, while_inserted, for_inserted, switch_inserted, do_inserted, foreach_inserted);

            size_t if_modified = 0, while_modified = 0, for_modified = 0, switch_modified = 0, do_modified = 0, foreach_modified = 0;
            conditional_counts(SRCDIFF_COMMON, if_modified, while_modified, for_modified, switch_modified, do_modified, foreach_modified);

            ++depth;
            output_header(out);
            if(if_deleted      || if_inserted      || if_modified)      output_counts(out, "if",      if_deleted,      if_inserted,      if_modified);
            if(while_deleted   || while_inserted   || while_modified)   output_counts(out, "while",   while_deleted,   while_inserted,   while_modified);
            if(for_deleted     || for_inserted     || for_modified)     output_counts(out, "for",     for_deleted,     for_inserted,     for_modified);
            if(switch_deleted  || switch_inserted  || switch_modified)  output_counts(out, "switch",  switch_deleted,  switch_inserted,  switch_modified);
            if(do_deleted      || do_inserted      || do_modified)      output_counts(out, "do",      do_deleted,      do_inserted,      do_modified);
            if(foreach_deleted || foreach_inserted || foreach_modified) output_counts(out, "foreach", foreach_deleted, foreach_inserted, foreach_modified);
            pad(out) << std::setw(10) << std::left << "Total" << std::right << std::setw(9) << number_deleted << std::setw(9) << number_inserted << std::setw(9) << number_modified << '\n';
            --depth;

            return out;

        }

        virtual std::ostream & output_all_parameter_counts(std::ostream & out, size_t number_parameters_deleted, size_t number_parameters_inserted, size_t number_parameters_modified) const {

            pad(out) << "Parameter list changes:\n";

            ++depth;
            output_header(out);
            output_counts(out, "Parameters", number_parameters_deleted, number_parameters_inserted, number_parameters_modified);
            --depth;

            return out;

        }

        virtual std::ostream & summary(std::ostream & out) const {

            if(operation != SRCDIFF_COMMON) {

                out << '\'' << (name.has_original() ? name.original() : name.modified()) << '\'';
                return out;

            }

            pad(out) << type_name << " '" << name << "': Impact = " << get_impact_factor() << '\n'; 
            // out << " Whitespace: " << whitespace_count;
            // out << "\tComment: " << comment_count;
            // out << "\tSyntax: " << syntax_count;
            // out << "\tTotal: " << total_count;
            // out << '\n';

            ++depth;

            // function signature
            if(!name.is_common()) pad(out) << "Name changed: " << name.original() << " -> " << name.modified() << '\n';

            // behaviour change
            bool is_return_type_change = !return_type.is_common();
            size_t number_parameters_deleted  = parameters.count(SRCDIFF_DELETE);
            size_t number_parameters_inserted = parameters.count(SRCDIFF_INSERT);
            size_t number_parameters_modified = 0;
            std::for_each(parameters.find(SRCDIFF_COMMON), parameters.upper_bound(SRCDIFF_COMMON),
                [&number_parameters_modified](const change_entity_map<profile_t>::pair & pair) { if(pair.second->syntax_count) ++number_parameters_modified; });

            if(is_return_type_change || number_parameters_deleted || number_parameters_inserted || number_parameters_modified) pad(out) << "Signature change:\n";

            ++depth;

            if(is_return_type_change) pad(out) << "Return type changed: " << return_type.original() << " -> " << return_type.modified() << '\n';

            if(number_parameters_deleted || number_parameters_inserted || number_parameters_modified)
                output_all_parameter_counts(out, number_parameters_deleted, number_parameters_inserted, number_parameters_modified);

            --depth;

            // body summary
            size_t number_conditionals_deleted  = conditionals.count(SRCDIFF_DELETE);
            size_t number_conditionals_inserted = conditionals.count(SRCDIFF_INSERT);
            size_t number_conditionals_modified = 0;
            std::for_each(conditionals.lower_bound(SRCDIFF_COMMON), conditionals.upper_bound(SRCDIFF_COMMON),
                [&number_conditionals_modified](const change_entity_map<profile_t>::pair & pair) { if(pair.second->syntax_count) ++number_conditionals_modified; });
            if(number_conditionals_deleted || number_conditionals_inserted || number_conditionals_modified)
                output_all_conditional_counts(out, number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified);

            --depth;

            return out;

        }

};

#endif
