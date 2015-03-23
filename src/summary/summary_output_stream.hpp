#ifndef INCLUDED_SUMMARY_OUTPUT_STREAM_HPP
#define INCLUDED_SUMMARY_OUTPUT_STREAM_HPP

#include <versioned_string.hpp>

#include <iostream>
#include <iomanip>

class summary_output_stream {

private:

	std::ostream & out;

	size_t depth;

    static constexpr const char * const BULLET = "\u2022";

	class iomanip_type {};

protected:

public:

public:

	summary_output_stream(std::ostream & out) : out(out), depth(0) {}

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

    summary_output_stream & pad() {

        for(size_t i = 0; i < depth; ++i)
            out << '\t';

        return *this;

    }

    summary_output_stream & begin_line() {

        return pad() << BULLET << ' ';

    }

	summary_output_stream & operator<<(const versioned_string & v_str) {

		out << v_str;

		return *this;

	}

	summary_output_stream & operator<<(const std::string & str) {

		out << str;

		return *this;

	}

	summary_output_stream & operator<<(const char * c_str) {

		out << c_str;

		return *this;

	}

	summary_output_stream & operator<<(char character) {

		out << character;

		return *this;

	}

	summary_output_stream & operator<<(size_t number) {

		out << number;

		return *this;

	}

	summary_output_stream & operator<<(const iomanip_type & type) {

		return *this;

	}

};

#endif