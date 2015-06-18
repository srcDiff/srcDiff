#ifndef INCLUDED_IDENTIFIER_UTILITIES
#define INCLUDED_IDENTIFIER_UTILITIES

#include <versioned_string.hpp>
#include <srcdiff_type.hpp>

#include <shortest_edit_script.hpp>

#include <string>
#include <vector>

class identifier_utilities {

private:

    const versioned_string identifier_;

    bool is_split;
    std::vector<std::string> original_identifiers;
    std::vector<std::string> modified_identifiers;

    boost::optional<versioned_string> trimmed_;
    boost::optional<versioned_string> diffed_;
    boost::optional<std::vector<std::pair<std::string, srcdiff_type>>> list_;

    bool is_complex;

private:

    bool is_identifier_char(char character) const {

        return character == '_' || isalnum(character);

    }

    std::vector<std::string> split_complex_identifier(const std::string & identifier_) const {

        std::vector<std::string> split_identifiers;
        for(std::string::size_type pos = 0; pos < identifier_.size(); ++pos) {

            std::string::size_type start_pos = pos;
            bool is_identifier_character = is_identifier_char(identifier_[start_pos]);
            while(pos < identifier_.size() && is_identifier_char(identifier_[pos]) == is_identifier_character)
                ++pos;

            split_identifiers.push_back(identifier_.substr(start_pos, pos - start_pos));

            --pos;

        }

        return split_identifiers;

    }

    static const void * str_accessor(int index, const void * structure, const void * context) {

        return &(*(const std::vector<std::string> *)structure)[index];

    }

    static int str_compare(const void * item_one, const void * item_two, const void * context) {

        return (*(const std::string *)item_one) == (*(const std::string *)item_two);

    }

public:

	identifier_utilities(const versioned_string & identifier_, bool is_single = false) : identifier_(identifier_), is_split(false), is_complex(!is_single) {

        if(is_single) {

            is_split = true;
            diffed_ = identifier_;
            trimmed_ = identifier_;

        }

    }

    const versioned_string & identifier() const {

        return identifier_;

    }

    bool complex() const {

        return is_complex;

    }

    void split_identifier() {

        if(!is_split) {

            original_identifiers = split_complex_identifier(identifier_.original());
            modified_identifiers = split_complex_identifier(identifier_.modified());
            is_split = true;

        }

    }

    const std::vector<std::pair<std::string, srcdiff_type>> & list() {

        if(list_) return *list_;

        split_identifier();

        class shortest_edit_script ses(str_compare, str_accessor, nullptr);
        ses.compute(&original_identifiers, original_identifiers.size(), &modified_identifiers, modified_identifiers.size());

        list_ = std::vector<std::pair<std::string, srcdiff_type>>();
        edit * edit_script = ses.get_script();
        int original_pos = 0, modified_pos = 0;
        for(edit * edits = edit_script; edits != nullptr; edits = edits->next) {

            if(edits->operation == SESDELETE) {

                while(original_pos < edits->offset_sequence_one) {

                    list_->emplace_back(original_identifiers[original_pos++], SRCDIFF_COMMON);
                    ++modified_pos;

                }

                for(int pos = 0; pos < edits->length; ++pos) {

                    list_->emplace_back(original_identifiers[original_pos++], SRCDIFF_DELETE);

                }

            } else {

                while(modified_pos < edits->offset_sequence_two) {

                    list_->emplace_back(modified_identifiers[modified_pos++], SRCDIFF_COMMON);
                    ++original_pos;

                }

                for(int pos = 0; pos < edits->length; ++pos)
                    list_->emplace_back(modified_identifiers[modified_pos++], SRCDIFF_INSERT);

            }

        }

        return *list_;

    }

    const versioned_string & trim() const {

        return *trimmed_;

    }

    const versioned_string & trim(bool is_call = false) {

        if(trimmed_) return *trimmed_;
 
        split_identifier();

        if(original_identifiers.size() > 1 || modified_identifiers.size() > 1)
            is_complex = true;

        if(!is_call || original_identifiers.size() <= 2 || modified_identifiers.size() <= 2) {

            trimmed_ = identifier_;
            return *trimmed_;

        }

        original_identifiers.pop_back();
        original_identifiers.pop_back();

        modified_identifiers.pop_back();
        modified_identifiers.pop_back();

        versioned_string ident;

        for(const std::string & original : original_identifiers)
            ident.append(original.c_str(), original.size(), SRCDIFF_DELETE);

        for(const std::string & modified : modified_identifiers)
            ident.append(modified.c_str(), modified.size(), SRCDIFF_INSERT);

        trimmed_ = ident;

        return *trimmed_;

    }

    const versioned_string & diff() const {

        return *trimmed_;

    }

    const versioned_string & diff() {

        if(diffed_) return *diffed_;

        split_identifier();

        size_t pos = 0;
        std::string::size_type start_pos = 0;
        while(pos < original_identifiers.size() && pos < modified_identifiers.size() && original_identifiers[pos] == modified_identifiers[pos]) {

            start_pos += original_identifiers[pos].size();
            ++pos;

        }

        size_t end_offset = 1;
        std::string::size_type end_original = identifier_.original().size(), end_modified = identifier_.modified().size();
        while(end_offset <= original_identifiers.size() && end_offset <= modified_identifiers.size()
              && original_identifiers[original_identifiers.size() - end_offset] == modified_identifiers[modified_identifiers.size() - end_offset]) {

            end_original -= original_identifiers[original_identifiers.size() - end_offset].size();
            end_modified -= modified_identifiers[modified_identifiers.size() - end_offset].size();
            ++end_offset;

        }

        versioned_string ident;

        if(start_pos >= end_original && start_pos >= end_modified) {

            diffed_ = ident;
            return *diffed_;

        }

        if(start_pos < end_original) ident.set_original(identifier_.original().substr(start_pos, end_original - start_pos));
        if(start_pos < end_modified) ident.set_modified(identifier_.modified().substr(start_pos, end_modified - start_pos));

        diffed_ = ident;

        return *diffed_;

    }

    bool operator==(const identifier_utilities & other) const {

        return trimmed_ == other.trimmed_ && diffed_ == other.diffed_;

    }

    bool operator!=(const identifier_utilities & other) const {

        return !(*this == other);

    }

    bool operator<(const identifier_utilities & other) const {

        if(trimmed_)
            return trimmed_ < other.trimmed_;
        return diffed_ < other.diffed_;

    }

};

#endif
