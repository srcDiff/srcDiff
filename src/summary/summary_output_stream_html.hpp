#ifndef INCLUDED_SUMMARY_OUTPUT_STREAM_HTML_HPP
#define INCLUDED_SUMMARY_OUTPUT_STREAM_HTML_HPP

#include <versioned_string.hpp>

#include <summary_output_stream.hpp>

#include <iostream>
#include <sstream>

class summary_output_stream_html : public summary_output_stream {

public:

	bool start_ul;
	bool end_li;

//	class iomanip_type {};

protected:
	std::ostream & output(const std::string & str) {
		out << str;
		return out;
	}

public:

	summary_output_stream_html(std::ostream & out) 
		: summary_output_stream(out), start_ul(true), end_li(false) {}

	std::ostream & ostream() {

		return out;

	}

	size_t depth() const {

		return depth_;

	}

	void depth(size_t depth_) {

		this->depth_ = depth_;

	}

	void increment_depth() {
		out << "<ul>";
		++depth_;

	}

	void decrement_depth() {
		out << "</ul>";
		--depth_;

	}

    summary_output_stream & pad() {
        return *this;
    }

    summary_output_stream & begin_line() {

    	if(start_ul) {
    		out << "<ul>";
    		start_ul = false;
    	}

    	out << "<li>";
    	end_li = true;

        return *this;

    }

    summary_output_stream & end_line() {
    	out << "</li>";
    	return *this;
    }

    void flush() {

    	out.flush();

    }

};

#endif
