#ifndef INCLUDED_REPLACE_SUMMARY_T_HPP
#define INCLUDED_REPLACE_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class replace_summary_t : public summary_t {

    private:

    	size_t number_original;
    	std::string original_type;
    	size_t number_comments_original;

    	size_t number_modified;
    	std::string modified_type;
    	size_t number_comments_modified;

    public:

        replace_summary_t(size_t number_original, std::string original_type, size_t number_comments_original,
        					  size_t number_modified, std::string modified_type, size_t number_comments_modified)
            : summary_t(REPLACE, SRCDIFF_COMMON),
              number_original(number_original), original_type(original_type), number_comments_original(number_comments_original),
              number_modified(number_modified), modified_type(modified_type), number_comments_modified(number_comments_modified) {}


        virtual bool compare(const summary_t & summary) const {

            //const replace_summary_t & replace_summary = dynamic_cast<const replace_summary_t &>(summary);
            return false;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            out.begin_line();

            if(number_original == 0 || number_modified == 0) {

                if(number_original) {

                    if(number_original == 1)
                        out << get_article(original_type) << ' ' << manip::bold() << original_type << manip::normal();
                    else
                        out << std::to_string(number_original) << ' ' << manip::bold() << original_type << 's' << manip::normal();

                    if(number_original == 1)
                        out << " was replaced with ";
                    else
                        out << " were replaced with ";

                    if(number_comments_modified == 1)
                        out << "a " << manip::bold() << "comment" << manip::normal();
                    else
                        out << std::to_string(number_comments_modified) << manip::bold() << " comments" << manip::normal();


                } else {


                    if(number_comments_original == 1)
                        out << "a " << manip::bold() << "comment" << manip::normal();
                    else
                        out << std::to_string(number_comments_original) << manip::bold() << " comments" << manip::normal();

                    if(number_comments_original == 1)
                        out << " was replaced with ";
                    else
                        out << " were replaced with ";

                    if(number_modified == 1)
                        out << get_article(modified_type) << ' ' << manip::bold() << modified_type << manip::normal();
                    else
                        out << std::to_string(number_modified) << ' ' << manip::bold() << modified_type << 's' << manip::normal();

                }

                out.end_line();

                return out;
            }

            if(number_original == 1)
                out << get_article(original_type) << ' ' << manip::bold() << original_type << manip::normal();
            else
                out << std::to_string(number_original) << ' ' << manip::bold() << original_type << 's' << manip::normal();

            if(number_comments_original == 1)
                out << " and a " << manip::bold() << "comment" << manip::normal();
            else if(number_comments_original > 1)
                out << " and " << std::to_string(number_comments_original) << manip::bold() << " comments" << manip::normal();


            if((number_original + number_comments_original) == 1)
                out << " was";
            else
                out << " were";

            out << " replaced with ";

            if(number_modified == 1)
                out << (original_type == modified_type ? "another" : get_article(modified_type)) << ' ' << manip::bold() << modified_type << manip::normal();
            else
                out << std::to_string(number_modified) << (original_type == modified_type ? " other " : " ") << manip::bold() << modified_type << 's' << manip::normal();

            if(number_comments_modified == 1)
                out << " and a " << manip::bold() << "comment" << manip::normal();
            else if(number_comments_modified > 1)
                out << " and " << std::to_string(number_comments_modified) << manip::bold() << " comments" << manip::normal();

            out.end_line();

            return out;

        }

};

#endif
