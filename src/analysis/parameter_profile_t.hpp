#ifndef INCLUDED_PARAMETER_PROFILE_T_HPP
#define INCLUDED_PARAMETER_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class parameter_profile_t : public profile_t {

    private:

    public:

        versioned_string type;
        versioned_string name;

        parameter_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation) : profile_t(type_name, uri, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(*parent == "type") type = name;
            else this->name = name;

        }

        virtual std::ostream & summary(std::ostream & out) const {

            pad(out) << type_name << " '" << name << "':\n";

            ++depth;

            // type
            if(!type.is_common()) pad(out) << "Parameter type change: " << type.original() << " -> " << type.modified() << '\n';

            // name
            if(!name.is_common()) pad(out) << "Parameter name change: " << name.original() << " -> " << name.modified() << '\n';

            --depth;

            return out;

        }

};

#endif
