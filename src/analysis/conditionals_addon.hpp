#ifndef INCLUDED_CONDITIONALS_ADDON_HPP
#define INCLUDED_CONDITIONALS_ADDON_HPP

#include <profile_t.hpp>
#include <if_profile_t.hpp>
#include <change_entity_map.hpp>

#include <iomanip>
#include <iostream>

class conditionals_addon {

    private:

    public:
        change_entity_map<profile_t> conditionals;

    public:

        conditionals_addon() {}

        std::ostream & conditional_text_summary(std::ostream & out, const size_t id,
                                                const std::vector<size_t> & descendant_profiles,
                                                const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

            for(size_t profile_pos : descendant_profiles) {

                const std::shared_ptr<profile_t> & profile = profile_list[profile_pos];

                if(!is_condition_type(profile->type_name) || (profile->operation == SRCDIFF_COMMON && profile->syntax_count == 0))
                    continue;

                const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
                const bool has_common = profile->has_common;

                if(profile->parent_id == id) {

                    if(profile->operation == SRCDIFF_COMMON) continue;

                    if(is_guard_clause) profile_t::begin_line(out) << "guard clause was ";
                    else                profile_t::begin_line(out) << profile->type_name << " statement was ";

                    out << (profile->operation == SRCDIFF_DELETE ? "removed from " : (has_common ? "added " : "added to "));

                    if(has_common) out << " around existing code in ";

                    out << "function\n";

                } else {

                    /** todo so if parent is deleted/inserted then should report as part of base or new document or say context */
                    std::function<std::string (const std::shared_ptr<profile_t> & profile)> get_article 
                        = [](const std::shared_ptr<profile_t> & profile) 
                    { 

                        const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
                        if(is_guard_clause) return "a";

                        const char letter = std::string(profile->type_name)[0];

                        if(letter == 'a' || letter == 'i' || letter == 'o' || letter == 'u')
                            return "an";
                        else
                            return "a";
                    };

                    const std::shared_ptr<profile_t> & parent_profile = profile_list[profile->parent_id];
                    const bool is_parent_guard_clause = parent_profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(parent_profile)->is_guard() : false;
                    const bool has_common = profile->has_common;

                    if(profile->operation != SRCDIFF_COMMON) {

                        if(is_parent_guard_clause) profile_t::begin_line(out) << "guard clause was modified ";
                        else                       profile_t::begin_line(out) << parent_profile->type_name << " statement was modified ";

                        out << (profile->operation == SRCDIFF_DELETE ? "removing " : "adding ");

                        out << get_article(profile) << ' ';

                        if(is_guard_clause) out << "guard clause";
                        else                out << profile->type_name << " statement";

                        if(has_common) out << (profile->operation == SRCDIFF_DELETE ? " from " : " ") << "around existing code";

                        out << '\n';

                    } else {

                        profile_t::begin_line(out);

                        if(parent_profile->operation != SRCDIFF_COMMON) out << "common ";

                        if(is_guard_clause) out << "guard clause within ";
                        else                out << profile->type_name << " statement within ";

                        if(parent_profile->operation != SRCDIFF_COMMON) out << (parent_profile->operation == SRCDIFF_DELETE ? "the deleted " : "the inserted ");
                        else if(!is_parent_guard_clause) out << get_article(parent_profile) << ' ';

                        if(is_parent_guard_clause) out << " guard clause ";
                        else                       out << parent_profile->type_name << " statement ";

                        out << "was modified\n";

                    }

                }

            }

            return out;

        }

        template <typename T>
        void count_operations(change_entity_map<T> map, size_t & number_deleted, size_t & number_inserted, size_t & number_modified) const {

            number_deleted  = map.count(SRCDIFF_DELETE);
            number_inserted = map.count(SRCDIFF_INSERT);
            number_modified = 0;
            std::for_each(map.lower_bound(SRCDIFF_COMMON), map.upper_bound(SRCDIFF_COMMON),
                [&number_modified](const typename change_entity_map<T>::pair & pair) { if(pair.second->syntax_count) ++number_modified; });

        }
        
        virtual void conditional_counts(srcdiff_type operation, size_t & guard_count, size_t & if_count, size_t & while_count, size_t & for_count,
                                        size_t & switch_count, size_t & do_count, size_t & foreach_count, size_t & forever_count) const {

            std::for_each(conditionals.lower_bound(operation), conditionals.upper_bound(operation),

                [&](const change_entity_map<profile_t>::pair & pair) {

                    if(operation == SRCDIFF_COMMON && pair.second->syntax_count == 0) return;

                    const std::string & type_name = operation == SRCDIFF_INSERT ? pair.second->type_name.modified() : pair.second->type_name.original();

                    switch(type_name[0]) {

                        case 'i': 
                            reinterpret_cast<const std::shared_ptr<if_profile_t> &>(pair.second)->is_guard() ? ++guard_count : ++if_count;
                            break;
                        case 'w': ++while_count;  break;
                        case 's': ++switch_count; break;
                        case 'd': ++do_count;     break;
                        case 'f':
                            type_name == "for" ? ++for_count : (type_name == "foreach" ? ++foreach_count : ++forever_count);
                            break;

                    }

                });

        }

        virtual std::ostream & output_header(std::ostream & out) const {

            return profile_t::pad(out) << std::setw(10) << std::left << "" << std::right << std::setw(9) << "Deleted" << std::setw(9) << "Inserted" << std::setw(9) << "Modified" << '\n';

        }

        virtual std::ostream & output_counts(std::ostream & out, const std::string & type, size_t deleted_count, size_t inserted_count, size_t modified_count) const {

            profile_t::pad(out) << std::setw(10) << std::left;
            out << type;
            out << std::right;
            out << std::setw(9) << deleted_count;
            out << std::setw(9) << inserted_count;
            out << std::setw(9) << modified_count;
            out << '\n';

            return out;
        }

        virtual std::ostream & output_all_conditional_counts(std::ostream & out, size_t number_deleted, size_t number_inserted, size_t number_modified) const {

            out << '\n';
            profile_t::profile_t::begin_line(out) << "Conditional Change Overview:\n";

            size_t guard_deleted = 0, if_deleted = 0, while_deleted = 0, for_deleted = 0, switch_deleted = 0, do_deleted = 0, foreach_deleted = 0, forever_deleted = 0;
            conditional_counts(SRCDIFF_DELETE, guard_deleted, if_deleted, while_deleted, for_deleted, switch_deleted, do_deleted, foreach_deleted, forever_deleted);

            size_t guard_inserted = 0, if_inserted = 0, while_inserted = 0, for_inserted = 0, switch_inserted = 0, do_inserted = 0, foreach_inserted = 0, forever_inserted = 0;
            conditional_counts(SRCDIFF_INSERT, guard_inserted, if_inserted, while_inserted, for_inserted, switch_inserted, do_inserted, foreach_inserted, forever_inserted);

            size_t guard_modified = 0, if_modified = 0, while_modified = 0, for_modified = 0, switch_modified = 0, do_modified = 0, foreach_modified = 0, forever_modified = 0;
            conditional_counts(SRCDIFF_COMMON, guard_modified, if_modified, while_modified, for_modified, switch_modified, do_modified, foreach_modified, forever_modified);

            ++profile_t::depth;
            output_header(out);
            if(guard_deleted   || guard_inserted   || guard_modified)   output_counts(out, "guard",   guard_deleted,   guard_inserted,   guard_modified);
            if(if_deleted      || if_inserted      || if_modified)      output_counts(out, "if",      if_deleted,      if_inserted,      if_modified);
            if(while_deleted   || while_inserted   || while_modified)   output_counts(out, "while",   while_deleted,   while_inserted,   while_modified);
            if(for_deleted     || for_inserted     || for_modified)     output_counts(out, "for",     for_deleted,     for_inserted,     for_modified);
            if(switch_deleted  || switch_inserted  || switch_modified)  output_counts(out, "switch",  switch_deleted,  switch_inserted,  switch_modified);
            if(do_deleted      || do_inserted      || do_modified)      output_counts(out, "do",      do_deleted,      do_inserted,      do_modified);
            if(foreach_deleted || foreach_inserted || foreach_modified) output_counts(out, "foreach", foreach_deleted, foreach_inserted, foreach_modified);
            if(forever_deleted || forever_inserted || forever_modified) output_counts(out, "forever", forever_deleted, forever_inserted, forever_modified);
            profile_t::pad(out) << std::setw(10) << std::left << "total" << std::right << std::setw(9) << number_deleted << std::setw(9) << number_inserted << std::setw(9) << number_modified << '\n';
            --profile_t::depth;

            return out;

        }

};

#endif
