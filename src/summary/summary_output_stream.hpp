#ifndef INCLUDED_SUMMARY_OUTPUT_STREAM_HPP
#define INCLUDED_SUMMARY_OUTPUT_STREAM_HPP

#include <versioned_string.hpp>

#include <iostream>
#include <sstream>

class summary_output_stream {

private:

	std::ostream & out;

    size_t max_width;

	size_t depth;

	int number_characters_output;

    static constexpr const char * const BULLET = "\u2022";


	class iomanip_type {};

protected:

private:

	std::ostream & output(const std::string & str) {

		// in case too much correct wrapping avoid outputting one word on line
		const size_t number_start_characters = depth * 8 + 2;
		if((depth * 8) > (max_width / 2)) return out << str;

		size_t non_count_characters = 0;
		size_t start_pos = 0, pos = 0;
		for(; pos < str.size(); ++pos) {

			while(str[pos] == '\x1b') {

				while(str[pos] != 'm')
					++pos, ++non_count_characters;

				++pos, ++non_count_characters;

				if(pos >= str.size()) goto end_loop;

			}

			if(isspace(str[pos])) {

				if(number_characters_output <= number_start_characters || (number_characters_output + ((pos - non_count_characters) - start_pos)) < max_width) {

					out << str.substr(start_pos, (pos - start_pos) + 1);
					if(str[pos] == '\n') number_characters_output = 0;
					else                 number_characters_output += ((pos - non_count_characters) - start_pos) + 1;

				} else {

					out << '\n';
					number_characters_output = 0;
					pad();
					out << "  ";
					number_characters_output += 2;

					while(start_pos < str.size() && str[start_pos] == ' ')
						++start_pos;

					if(start_pos <= pos) {

						out << str.substr(start_pos, (pos - start_pos) + 1);
						number_characters_output = ((pos - non_count_characters) - start_pos) + 1;

					}

				}

				non_count_characters = 0;
				start_pos = pos + 1;

			}

		}

end_loop:

		if(pos - start_pos) {

			if(number_characters_output <= number_start_characters || (number_characters_output + ((pos - non_count_characters) - start_pos)) < max_width) {

				out << str.substr(start_pos, pos - start_pos);
				if(str[pos] == '\n') number_characters_output = 0;
				else                 number_characters_output += (pos - non_count_characters) - start_pos;

			} else {

				out << '\n';
				number_characters_output = 0;
				pad();
				out << "  ";

				out << str.substr(start_pos, pos - start_pos);
				number_characters_output = (pos - non_count_characters) - start_pos;

			}

			non_count_characters = 0;

		}

		return out;

	}

public:

	summary_output_stream(std::ostream & out, size_t max_width = 100) : out(out), max_width(max_width), depth(0), number_characters_output(0) {}

	std::ostream & ostream() {

		return out;

	}

	void increment_depth() {

		++depth;

	}

	void decrement_depth() {

		--depth;

	}

    summary_output_stream & pad() {

        for(size_t i = 0; i < depth; ++i)
            out << '\t';

        number_characters_output = depth * 8;

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

};

#endif