#ifndef INCLUDED_VERSIONED_STRING_HPP
#define INCLUDED_VERSIONED_STRING_HPP

#include <srcdiff_type.hpp>

#include <string>

#include <boost/optional.hpp>

class versioned_string {

	private:

		boost::optional<std::string> string_original;
		boost::optional<std::string> string_modified;

		bool is_common;

	protected:

	public:

		versioned_string(std::string string, enum srcdiff_type version) : string_original(version != SRCDIFF_INSERT ? string : boost::optional<std::string>()),
																		  string_modified(version != SRCDIFF_DELETE ? string : boost::optional<std::string>()),
																		  is_common(version == SRCDIFF_COMMON) {}

		versioned_string(std::string string_original, std::string string_modified) : string_original(string_original), string_modified(string_modified), is_common(false) {}

		bool is_common() const {

			return is_common;

		}

		bool has_original() const {

			return string_original;

		}

		bool has_modified() const {

			return string_modified;

		}

		std::string get_original() const {

			if(!has_original()) return "";

			return *string_original;

		}

		std::string get_modified() const {

			if(!has_modified()) return "";

			return *string_modified;

		}


};



#endif