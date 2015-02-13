#ifndef INCLUDED_UNIT_PROFILE_T_HPP
#define INCLUDED_UNIT_PROFILE_T_HPP

#include <profile_t.hpp>
#include <function_profile_t.hpp>
#include <versioned_string.hpp>

struct unit_profile_t : public profile_t {

    private:

    public:

        versioned_string file_name;

        std::multimap<srcdiff_type, std::shared_ptr<function_profile_t>> functions;

        unit_profile_t(std::string type_name, srcdiff_type operation) : profile_t(type_name, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            file_name = name;
            
            if(!file_name.has_modified())      operation = SRCDIFF_DELETE;
            else if(!file_name.has_original()) operation = SRCDIFF_INSERT;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile) {

            if(is_function_type(profile->type_name)) functions.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile));
            else child_profiles.push_back(profile->id);
            
        }

        virtual std::ostream & summary(std::ostream & out) const {

            out << type_name << " '" << file_name << "':"; 
            out << " Whitespace: " << whitespace_count;
            out << "\tComment: " << comment_count;
            out << "\tSyntax: " << syntax_count;
            out << "\tTotal: " << total_count;
            out << '\n';

            size_t num_deleted_function  = functions.count(SRCDIFF_DELETE);
            if(num_deleted_function) {

                out << '\n';

                out << "Deleted functions (" << num_deleted_function << "): { ";
                std::multimap<srcdiff_type, std::shared_ptr<function_profile_t>>::const_iterator citr = functions.find(SRCDIFF_DELETE);
                citr->second->summary(out);
                ++citr;
                for(; citr != functions.upper_bound(SRCDIFF_DELETE); ++citr) {

                    out << ", ";
                    citr->second->summary(out);

                }

                out << " }\n";

            }

            size_t num_inserted_function = functions.count(SRCDIFF_INSERT);
            if(num_inserted_function) {

                out << '\n';

                out << "Inserted functions (" << num_inserted_function << "): { ";
                std::multimap<srcdiff_type, std::shared_ptr<function_profile_t>>::const_iterator citr = functions.find(SRCDIFF_INSERT);
                citr->second->summary(out);
                ++citr;
                for(; citr != functions.upper_bound(SRCDIFF_INSERT); ++citr) {

                    out << ", ";
                    citr->second->summary(out);

                }

                out << " }\n";

            }

            size_t num_modified_function = functions.count(SRCDIFF_COMMON);
            if(num_modified_function) {

                out << '\n';

                out << "Modified functions: " << num_modified_function << '\n';
                for(std::multimap<srcdiff_type, std::shared_ptr<function_profile_t>>::const_iterator citr = functions.find(SRCDIFF_COMMON); citr != functions.upper_bound(SRCDIFF_COMMON); ++citr)
                    citr->second->summary(out);

            }

            return out;


        }

};

#endif