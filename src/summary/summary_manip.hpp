#ifndef INCLUDED_SUMMARY_MANIP_TYPE_HPP
#define INCLUDED_SUMMARY_MANIP_TYPE_HPP

#include <summary_output_stream.hpp>

#include <iostream>
#include <iomanip>

class summary_manip {

private:

	enum manipulation { SETW, LEFT, RIGHT, BOLD, NORMAL } manip;
	int setw_n;

	static constexpr const char * const BOLD_TEXT = "\x1b[1m";
	static constexpr const char * const NORMAL_TEXT = "\x1b[0m";

	static constexpr const char * const BOLD_TEXT_HTML = "<strong>";
	static constexpr const char * const NORMAL_TEXT_HTML = "</strong>";

	static bool is_html;

protected:

public:

	summary_manip() = delete;

	summary_manip(manipulation manip, int setw_n = 0) : manip(manip), setw_n(setw_n) {}

	static bool get_is_html() {
		return is_html;
	}

	static void set_is_html(bool is_html) {
		summary_manip::is_html = is_html;
	}

	static summary_manip setw(int n) {

		return summary_manip(SETW, n);

	}

	static summary_manip left() {

		return summary_manip(LEFT);

	}

	static summary_manip right() {

		return summary_manip(RIGHT);

	}

	static summary_manip bold() {

		return summary_manip(BOLD);

	}

	static summary_manip normal() {

		return summary_manip(NORMAL);
		
	}

	friend summary_output_stream & operator<<(summary_output_stream & out, const summary_manip & type) {

		if(type.manip == SETW)        out.ostream() << std::setw(type.setw_n);
		else if(type.manip == LEFT)   out.ostream() << std::left;
		else if(type.manip == RIGHT)  out.ostream() << std::right;
		else if(type.manip == BOLD)   out.ostream() << (is_html ? BOLD_TEXT_HTML : BOLD_TEXT);
		else if(type.manip == NORMAL) out.ostream() << (is_html ? NORMAL_TEXT_HTML : NORMAL_TEXT);

		return out;

	}

};

using manip = summary_manip;

#endif
