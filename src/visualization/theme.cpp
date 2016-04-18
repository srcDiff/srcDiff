/**
 *  @file monokai.hpp
 *
 *  Specifies color scheme for syntax highlighting.
 *
 *  @author Michael John Decker <mdecker6@kent.edu>
 */

#include <theme.hpp>

theme_t::theme_t(bool is_html) {}

std::string theme_t::token2color(const std::string & token, const std::vector<std::string> & srcdiff_elements) const {

	std::vector<std::string>::size_type parent_pos = srcdiff_elements.size() - 1;
	while( parent_pos > 0
		&& (   srcdiff_elements.at(parent_pos) == "diff:common"
			|| srcdiff_elements.at(parent_pos) == "diff:delete"
			|| srcdiff_elements.at(parent_pos) == "diff:insert"
			|| srcdiff_elements.at(parent_pos) == "diff:ws"))
		--parent_pos;

	const std::string & parent = srcdiff_elements.at(parent_pos);

    if(parent == "comment")        return comment_color;
    if(parent == "literal")        return number_color;
    if(parent == "literal_string") return string_color;

    try {

        return keywords.color(token);

    } catch(const std::out_of_range & e) {}

    return "";

}
