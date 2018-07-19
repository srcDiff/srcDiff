#ifndef INCLUDED_DECL_STMT_PROFILE_T_HPP
#define INCLUDED_DECL_STMT_PROFILE_T_HPP

#include <profile_t.hpp>

#include <identifier_profile_t.hpp>

#include <type_query.hpp>
#include <versioned_string.hpp>
#include <summary_manip.hpp>
#include <text_summary.hpp>

class decl_stmt_profile_t : public profile_t {

    private:

    public:

        std::multimap<srcdiff_type, std::string> specifiers;
        std::shared_ptr<profile_t> type;
        versioned_string name;

        std::shared_ptr<profile_t> init;
        std::vector<std::string> other;

        decl_stmt_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent)
        : profile_t(type_name, uri, operation, parent),
          specifiers(), type(), name(), init(), other() {}

        virtual const versioned_string & get_name() const {
            return name;
        }

        virtual void set_name(const std::shared_ptr<identifier_profile_t> & name, const boost::optional<versioned_string> & parent) {

            if(*parent == "decl") this->name = name->name;

        }

        virtual void add_child_change(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

        	if(is_specifier(profile->type_name) && parent == "decl") specifiers.emplace(profile->operation, profile->raw);

            child_change_profiles.insert(std::lower_bound(child_change_profiles.begin(), child_change_profiles.end(), profile), profile);

        }

        virtual summary_output_stream & summary(summary_output_stream & out, size_t summary_types) const {

            if(out.depth() != 0) {
                out.begin_line();
                if(manip::get_is_html()) out << "<span id=\"" + signature + "\" content=\"summary_link\">";
                out << manip::var() << name << manip::end_var();
                if(manip::get_is_html()) out << "</span>";
                out.end_line();
                return out;
            }

            out.begin_line();

            if(out.depth() != 0) {
                out << "member '" << name << "':\n";
            }

            bool type_change = type && (type->operation != SRCDIFF_COMMON || type->is_modified);
            bool init_change = init && (init->operation != SRCDIFF_COMMON || init->is_modified);

            std::string type_impact = type_change ? "true" : "false";
            std::string name_impact = name.is_common() ? "false" : "true";
            std::string init_impact = init_change ? "true" : "false";

            out << manip::bold() << "Impact" << manip::normal() << ": ";
            out << manip::bold() << "Type"     << manip::normal() << " = " << manip::bold() <<  type_impact << manip::normal();
            out << "   ";
            out << manip::bold() << "Name" << manip::normal() << " = " << manip::bold() << name_impact << manip::normal();
            out << "   ";
            out << manip::bold() << "Init"    << manip::normal() << " = " << manip::bold() << init_impact << manip::normal();            
            // out << "   ";
            // out << manip::bold() << "Other"    << manip::normal() << " = " << manip::bold() << init_impact << manip::normal();            
            out.end_line();
   
            out.increment_depth();

            if(is_summary_type(summary_types, summary_type::TEXT)) {

                text_summary text;

                if(!name.is_common()) {
                    out.begin_line() << manip::bold() << "name change" << manip::normal()
                                     << " from " << manip::var() << name.original() << manip::end_var() << " to " << manip::var() << name.modified() << manip::end_var();
                    out.end_line();
                }

                if(type_change) {

                    out.begin_line() << manip::bold() << "type change" << manip::normal();
                    out.end_line();

                }

                if(init_change) {

                    std::string op_name = init->operation == SRCDIFF_COMMON ? "modified" : (init->operation == SRCDIFF_DELETE ? "deleted" : "inserted");
                    out.begin_line() << manip::bold() << "init" << manip::normal() << " was "
                                     << manip::bold() << op_name << manip::normal();
                    out.end_line();

                }

                text.specifier(out, specifiers);

            }

            out.decrement_depth();

         return out;
        }


};

#endif
