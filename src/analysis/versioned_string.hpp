#ifndef INCLUDED_VERSIONED_STRING_HPP
#define INCLUDED_VERSIONED_STRING_HPP

#include <srcdiff_type.hpp>

#include <string>

#include <boost/optional.hpp>

class versioned_string {

	private:

		boost::optional<std::string> string_original;
		boost::optional<std::string> string_modified;

	protected:

	public:

		versioned_string() {}

		versioned_string(std::string string) : string_original(string), string_modified(string) {}

		versioned_string(std::string string_original, std::string string_modified) : string_original(string_original), string_modified(string_modified) {}

		bool is_common() const {

			return string_original == string_modified;

		}

		bool has_original() const {

			return bool(string_original);

		}

		bool has_modified() const {

			return bool(string_modified);

		}

		std::string original() const {

			if(!has_original()) return "";

			return *string_original;

		}

		std::string modified() const {

			if(!has_modified()) return "";

			return *string_modified;

		}

		void set_original(const std::string & string_original) {

			this->string_original = string_original;

		}

		void set_modified(const std::string & string_modified) {

			this->string_modified = string_modified;

		}

		void append(const char * characters, size_t len, enum srcdiff_type version) {

			if(len == 0) return;

			if(version != SRCDIFF_INSERT) {

				if(!bool(string_original)) string_original = std::string(characters, len);
				else string_original->append(characters, len);

			}

			if(version != SRCDIFF_DELETE) {

				if(!bool(string_modified)) string_modified = std::string(characters, len);
				else string_modified->append(characters, len);

			}

		}

		void clear() {

			string_original = boost::optional<std::string>();
			string_modified = boost::optional<std::string>();

		}


        friend std::ostream & operator<<(std::ostream & out, const versioned_string & string) {

        	if(string.is_common()) return out << string.original();
        	else return out << string.original() << '|' << string.modified();

       	}

       	operator std::string() const {

       		if(is_common()) return original();

       		return original() + '|' + modified();

       	}

       	bool operator==(const std::string & str) const {

       		return std::string(*this) == str;

		}

		bool operator!=(const std::string & str) const {

       		return std::string(*this) != str;

		}

		std::string operator+(const char * c_str) const {


			return std::string(*this) + c_str;

		}

		friend std::string operator+(const std::string & str, const versioned_string & v_str) {


			return str + std::string(v_str);

		}

		friend std::string operator+(const char * c_str, const versioned_string & v_str) {


			return c_str + std::string(v_str);

		}
};



#endif
