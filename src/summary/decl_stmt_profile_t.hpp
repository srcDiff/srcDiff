#ifndef INCLUDED_DECL_STMT_PROFILE_T_HPP
#define INCLUDED_DECL_STMT_PROFILE_T_HPP

#include <profile_t.hpp>

#include <identifier_profile_t.hpp>

#include <type_query.hpp>
#include <versioned_string.hpp>
#include <summary_manip.hpp>

class decl_stmt_profile_t : public profile_t {

    private:

    public:

	    std::vector<std::shared_ptr<profile_t>> specifiers;
        std::shared_ptr<profile_t> type;
        versioned_string name;

        std::shared_ptr<profile_t> init;

        decl_stmt_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent) : profile_t(type_name, uri, operation, parent) {}

        virtual const versioned_string & get_name() const {
            return name;
        }

        virtual void set_name(const std::shared_ptr<identifier_profile_t> & name, const boost::optional<versioned_string> & parent) {

            if(*parent == "decl") this->name = name->name;

        }

        virtual void add_child_change(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

        	if(is_specifier(profile->type_name) && parent == "decl") specifiers.push_back(profile);

            child_change_profiles.insert(std::lower_bound(child_change_profiles.begin(), child_change_profiles.end(), profile), profile);

        }

        virtual summary_output_stream & summary(summary_output_stream & out, size_t summary_types) const {
            if(operation != SRCDIFF_COMMON) {

                if(out.depth() == 0) return out;

                out << '\'' << (name.has_original() ? name.original() : name.modified()) << '\'';
                return out;

            }

            out.begin_line();

            if(out.depth() != 0) {
                out << "member '" << name << "':\n";
            }

            std::string type_impact = (type->operation != SRCDIFF_COMMON || type->is_modified) ? "false" : "true";
            std::string name_impact = name.is_common() ? "false" : "true";
            std::string init_impact = (init->operation != SRCDIFF_COMMON || init->is_modified) ? "false" : "true";

            out << manip::bold() << "Impact" << manip::normal() << ": ";
            out << manip::bold() << "Type"     << manip::normal() << " = " << manip::bold() <<  type_impact << manip::normal();
            out << '\t';
            out << manip::bold() << "Name" << manip::normal() << " = " << manip::bold() << name_impact << manip::normal();
            out << '\t';
            out << manip::bold() << "Init"    << manip::normal() << " = " << manip::bold() << init_impact << manip::normal();            
            out.end_line();


            return out;
        }


};

#endif
