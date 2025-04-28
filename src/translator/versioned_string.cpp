// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file versioned_string.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <versioned_string.hpp>
// TODO: is this still used?
#ifdef __linux__
    #include <execinfo.h>
#endif
#include <sstream>
#include <iterator>

const std::string versioned_string::empty_str;

versioned_string::versioned_string(char separator)
    : string_original(), string_modified(), separator(separator) {}

versioned_string::versioned_string(std::string string, char separator)
    : string_original(string), string_modified(string), separator(separator) {}

versioned_string::versioned_string(std::string string_original, std::string string_modified, char separator) 
    : string_original(string_original), string_modified(string_modified), separator(separator) {}

bool versioned_string::is_common() const {

    return string_original == string_modified;

}

bool versioned_string::has_original() const {

    return bool(string_original);

}

bool versioned_string::has_modified() const {

    return bool(string_modified);

}

std::string & versioned_string::original() {

    assert(has_original());

    return *string_original;

}

const std::string & versioned_string::original() const {

    if(!has_original()) return empty_str;

    return *string_original;

}

std::string & versioned_string::modified() {

    assert(has_modified());

    return *string_modified;

}

const std::string & versioned_string::modified() const {

    if(!has_modified()) return empty_str;

    return *string_modified;

}

const std::string & versioned_string::first_active_string() const {

    if(has_original()) return original();

    if(has_modified()) return modified();

    return empty_str;

}

void versioned_string::set_original(const std::string & string_original) {

    this->string_original = string_original;

}

void versioned_string::set_modified(const std::string & string_modified) {

    this->string_modified = string_modified;

}

void versioned_string::append(const std::string & str, srcdiff::operation version) {
    append(str.c_str(), str.size(), version);
}

void versioned_string::append(const char * characters, size_t len, srcdiff::operation version) {

    if(len == 0) return;

    if(version != srcdiff::INSERT) {

        if(!bool(string_original)) string_original = std::string(characters, len);
        else string_original->append(characters, len);

    }

    if(version != srcdiff::DELETE) {

        if(!bool(string_modified)) string_modified = std::string(characters, len);
        else string_modified->append(characters, len);

    }

}

void versioned_string::clear() {

    string_original = std::optional<std::string>();
    string_modified = std::optional<std::string>();

}

std::string versioned_string::normalize(const std::string & str, const std::string & sep) {
    std::istringstream in(str);
    std::ostringstream out;
    std::copy(std::istream_iterator<std::string>(in), std::istream_iterator<std::string>(), std::ostream_iterator<std::string>(out, sep.c_str()));
    return out.str();
}

versioned_string versioned_string::remove_spaces() const {
    versioned_string str;
    if(string_original) {
        str.string_original = normalize(*string_original, "");
    }

    if(string_modified) {
        str.string_modified = normalize(*string_modified, "");
    }

    return str;
}

versioned_string versioned_string::normalize_spaces() const {
    versioned_string str;
    if(string_original) {
        str.string_original = normalize(*string_original, " ");
    }

    if(string_modified) {
        str.string_modified = normalize(*string_modified, " ");
    }

    return str;
}

void versioned_string::swap(versioned_string & other) {

    string_original.swap(other.string_original);
    string_modified.swap(other.string_modified);

}

versioned_string::operator std::string() const {

    if(is_common()) return original();

    return original() + separator + modified();

}

bool versioned_string::operator==(const std::string & str) const {

    return std::string(*this) == str;

}

bool versioned_string::operator!=(const std::string & str) const {

        return std::string(*this) != str;

}

bool versioned_string::operator==(const char * c_str) const {

    return std::string(*this) == c_str;

}

bool versioned_string::operator!=(const char * c_str) const {

        return std::string(*this) != c_str;

}

bool versioned_string::operator<(const versioned_string & v_str) const {

        return std::string(*this) < std::string(v_str);

}

std::string versioned_string::operator+(const std::string & str) const {


    return std::string(*this) + str;

}

std::string versioned_string::operator+(const char * c_str) const {


    return std::string(*this) + c_str;

}

versioned_string versioned_string::operator+(const versioned_string & v_str) const {
    versioned_string new_str(*this);
    new_str += v_str;
    return new_str;
}

versioned_string & versioned_string::operator+=(const versioned_string & v_str) {

    if(v_str.string_original) {
        append(*v_str.string_original, srcdiff::DELETE);
    }

    if(v_str.string_modified) {
        append(*v_str.string_modified, srcdiff::INSERT);
    }

    return *this;
}

std::ostream & operator<<(std::ostream & out, const versioned_string & string) {

    if(string.is_common()) return out << string.original();
    else return out << string.original() << string.separator << string.modified();

}

std::string operator+(const std::string & str, const versioned_string & v_str) {


    return str + std::string(v_str);

}

std::string operator+(const char * c_str, const versioned_string & v_str) {


    return c_str + std::string(v_str);

}
