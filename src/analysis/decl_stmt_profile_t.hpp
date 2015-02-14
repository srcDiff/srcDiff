#ifndef INCLUDED_DECL_STMT_PROFILE_T_HPP
#define INCLUDED_DECL_STMT_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class decl_stmt_profile_t : public profile_t {

    private:

    public:

        versioned_string type;
        versioned_string name;

        decl_stmt_profile_t(std::string type_name, srcdiff_type operation) : profile_t(type_name, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            if(*parent == "type") type = name;
            else this->name = name;

        }

        virtual std::ostream & summary(std::ostream & out) const {

            if(operation != SRCDIFF_COMMON) {

                out << '\'' << (name.has_original() ? name.original() : name.modified()) << '\'';
                return out;

            }

            pad(out) << type_name << " '" << name << "':\n";

            ++depth;

            // type
            if(!type.is_common()) pad(out) << "Declaration statement type change: " << type.original() << " -> " << type.modified() << '\n';

            // name
            if(!name.is_common()) pad(out) << "Declaration statement name change: " << name.original() << " -> " << name.modified() << '\n';

            --depth;

            return out;

        }

};

#endif