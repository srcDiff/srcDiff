#ifndef INCLUDED_IDENTIFIER_DIFF
#define INCLUDED_IDENTIFIER_DIFF

#include <versioned_string.hpp>

#include <string>
#include <vector>

class identifier_diff {

private:

	const versioned_string & identifier;
	boost::optional<versioned_string> diff;

private:

    bool is_identifier_char(char character) const {

        return character == '_' || isalnum(character);

    }

    std::vector<std::string> split_complex_identifier(const std::string & identifier) const {

        std::vector<std::string> split_identifiers;
        for(std::string::size_type pos = 0; pos < identifier.size(); ++pos) {

            std::string::size_type start_pos = pos;
            bool is_identifier_character = is_identifier_char(identifier[start_pos]);
            while(pos < identifier.size() && is_identifier_char(identifier[pos]) == is_identifier_character)
                ++pos;

            split_identifiers.push_back(identifier.substr(start_pos, pos - start_pos));

            --pos;

        }

        return split_identifiers;

    }

public:

	identifier_diff(const versioned_string & identifier) : identifier(identifier) {}

	const versioned_string & get_diff() const {

		assert(diff);

		return *diff;

	}

    void compute_diff() {

		std::vector<std::string> original_identifiers = split_complex_identifier(identifier.original());
        std::vector<std::string> modified_identifiers = split_complex_identifier(identifier.modified());

        size_t pos = 0;
        std::string::size_type start_pos = 0;
        while(pos < original_identifiers.size() && pos < modified_identifiers.size() && original_identifiers[pos] == modified_identifiers[pos]) {

            start_pos += original_identifiers[pos].size();
            ++pos;

        }

        size_t end_offset = 1;
        std::string::size_type end_original = identifier.original().size(), end_modified = identifier.modified().size();
        while(end_offset <= original_identifiers.size() && end_offset <= modified_identifiers.size()
              && original_identifiers[original_identifiers.size() - end_offset] == modified_identifiers[modified_identifiers.size() - end_offset]) {

            end_original -= original_identifiers[original_identifiers.size() - end_offset].size();
            end_modified -= modified_identifiers[modified_identifiers.size() - end_offset].size();
            ++end_offset;

        }

        versioned_string ident;

        if(start_pos >= end_original && start_pos >= end_modified) return;

        if(start_pos < end_original) ident.set_original(identifier.original().substr(start_pos, end_original - start_pos));
        if(start_pos < end_modified) ident.set_modified(identifier.modified().substr(start_pos, end_modified - start_pos));

        diff = ident;

    }

};

#endif
