#ifndef INCLUDED_REPLACEMENT_SUMMARY_T_HPP
#define INCLUDED_REPLACEMENT_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class replacement_summary_t : public summary_t {

    private:

    	size_t number_original;
    	std::string original_type;
    	size_t number_comments_original;

    	size_t number_modified;
    	std::string modified_type;
    	size_t number_comments_modified;

    public:

        replacement_summary_t(size_t number_original, std::string original_type, size_t number_comments_original,
        					  size_t number_modified, std::string modified_type, size_t number_comments_modified)
            : summary_t(REPLACEMENT, SRCDIFF_COMMON),
              number_original(number_original), original_type(original_type), number_comments_original(number_comments_original),
              number_modified(number_modified), modified_type(modified_type), number_comments_modified(number_comments_modified) {}

};

#endif
