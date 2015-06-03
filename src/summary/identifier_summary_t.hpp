#ifndef INCLUDED_IDENTIFIER_SUMMARY_T_HPP
#define INCLUDED_IDENTIFIER_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <versioned_string.hpp>

class identifier_summary_t : public summary_t {

    private:

        versioned_string name;
        bool is_rename;

    public:

        identifier_summary_t(versioned_string name, bool is_rename)
            : summary_t(IDENTIFIER, SRCDIFF_COMMON), name(name), is_rename(is_rename) {}

        virtual bool compare(const summary_t & summary) const {

            //const identifier_summary_t & identifier_summary = dynamic_cast<const identifier_summary_t &>(summary);
            return false;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            out.begin_line();

            if(!is_rename) {

                out << "name change from '";
                out << name.original();
                out << "' to '";
                out << name.modified();
                out << '\'';

            } else {

                out << '\'';
                out << name.original();
                out << "' was renamed to '";
                out << name.modified();
                out << '\'';

            }

            out << '\n';

            return out;

        }

};

#endif
