// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file versioned_string.hpp
 *
 * @copyright Copyright (C) 2015-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_VERSIONED_STRING_HPP
#define INCLUDED_VERSIONED_STRING_HPP

#include <srcdiff_type.hpp>

#include <string>
#include <iostream>

#include <optional>
#include <cassert>

class versioned_string {

    private:

        static const std::string empty_str;

        std::optional<std::string> string_original;
        std::optional<std::string> string_modified;

        char separator;

    protected:

    public:
        static std::string normalize(const std::string & str, const std::string & sep);

        versioned_string(char separator = '|');
        versioned_string(std::string string, char separator = '|');
        versioned_string(std::string string_original, std::string string_modified, char separator = '|');

        bool is_common() const;
        bool has_original() const;
        bool has_modified() const;

        std::string & original();
        const std::string & original() const;
        std::string & modified();
        const std::string & modified() const;
        const std::string & first_active_string() const;

        void set_original(const std::string & string_original);
        void set_modified(const std::string & string_modified);
        void append(const std::string & str, enum srcdiff_type version);
        void append(const char * characters, size_t len, enum srcdiff_type version);
        void clear();

        versioned_string remove_spaces() const;
        versioned_string normalize_spaces() const;

        void swap(versioned_string & other);

        operator std::string() const;
        bool operator==(const std::string & str) const;
        bool operator!=(const std::string & str) const;
        bool operator==(const char * c_str) const;
        bool operator!=(const char * c_str) const;
        bool operator<(const versioned_string & v_str) const;
        std::string operator+(const std::string & str) const;
        std::string operator+(const char * c_str) const;
        versioned_string operator+(const versioned_string & v_str) const;

        versioned_string & operator+=(const versioned_string & v_str);

        friend std::ostream & operator<<(std::ostream & out, const versioned_string & string);
        friend std::string operator+(const std::string & str, const versioned_string & v_str);
        friend std::string operator+(const char * c_str, const versioned_string & v_str);

};



#endif
