#ifndef INCLUDED_SUMMARY_OUTPUT_STREAM_HPP
#define INCLUDED_SUMMARY_OUTPUT_STREAM_HPP

#include <versioned_string.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>

class summary_output_stream {

private:

	std::ostream & out;

    size_t max_width;

	size_t depth;

	int number_characters_output;

	static constexpr const char * const BOLD_TEXT = "\x1b[1m";
	static constexpr const char * const NORMAL_TEXT = "\x1b[0m";
    static constexpr const char * const BULLET = "\u2022";


	class iomanip_type {};

protected:

private:

	std::ostream & output(const std::string & str) {

		// in case too much correct wrapping avoid outputting word on line
		if((depth * 8) > (max_width / 2)) return out << str;

		size_t start_pos = 0, pos = 0;
		for(; pos < str.size(); ++pos) {

			if(isspace(str[pos])) {

				if(number_characters_output == 0 || (number_characters_output + (pos - start_pos)) < max_width) {

					out << str.substr(start_pos, (pos - start_pos) + 1);
					if(str[pos] == '\n') number_characters_output = 0;
					else                 number_characters_output += (pos - start_pos) + 1;

				} else {

					out << '\n';
					number_characters_output = 0;
					pad();
					out << "  ";

					out << str.substr(start_pos, (pos - start_pos) + 1);
					number_characters_output = (pos - start_pos) + 1;

				}

				start_pos = pos + 1;

			}

		}

		if(pos - start_pos) {

			if(number_characters_output == 0 || (number_characters_output + (pos - start_pos)) < max_width) {

				out << str.substr(start_pos, pos - start_pos);
				if(str[pos] == '\n') number_characters_output = 0;
				else                 number_characters_output += pos - start_pos;

			} else {

				out << '\n';
				number_characters_output = 0;
				pad();
				out << "  ";

				out << str.substr(start_pos, pos - start_pos);
				number_characters_output = pos - start_pos;

			}

		}

		return out;

	}

public:

	summary_output_stream(std::ostream & out, size_t max_width = 100) : out(out), max_width(max_width), depth(0), number_characters_output(0) {}

	iomanip_type setw(int n) {

		out << std::setw(n);
		return iomanip_type();


	}

	iomanip_type left() {

		out << std::left;
		return iomanip_type();


	}

	iomanip_type right() {

		out << std::right;
		return iomanip_type();


	}

	void increment_depth() {

		++depth;

	}

	void decrement_depth() {

		--depth;

	}

	iomanip_type bold() {

		out << BOLD_TEXT;

		return iomanip_type();

	}

	iomanip_type normal() {

		out << NORMAL_TEXT;

		return iomanip_type();
		
	}

    summary_output_stream & pad() {

        for(size_t i = 0; i < depth; ++i)
            out << '\t';

        number_characters_output += depth * 8;

        return *this;

    }

    summary_output_stream & begin_line() {

        pad();
        out << BULLET << ' ';

        number_characters_output += 2;

        return *this;

    }

	summary_output_stream & operator<<(const versioned_string & v_str) {

		std::ostringstream ostr_stream;

		ostr_stream << v_str;

		output(ostr_stream.str());

		return *this;

	}

	summary_output_stream & operator<<(const std::string & str) {

		output(str);

		return *this;

	}

	summary_output_stream & operator<<(const char * c_str) {

		output(c_str);

		return *this;

	}

	summary_output_stream & operator<<(char character) {

		std::string str;
		str.append(&character, 1);

		output(str);

		return *this;

	}

	summary_output_stream & operator<<(size_t number) {

		output(std::to_string(number));

		return *this;

	}

	summary_output_stream & operator<<(const iomanip_type & type) {

		return *this;

	}

};

#endif