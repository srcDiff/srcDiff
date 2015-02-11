#ifndef INCLUDED_COUNTS_T_HPP
#define INCLUDED_COUNTS_T_HPP

#include <iostream>

struct counts_t {

    int whitespace;
    int comment;
    int syntax;
    int total;

    counts_t() : whitespace(0), comment(0), syntax(0), total(0) {}

    counts_t(int whitespace, int comment, int syntax, int total) : whitespace(whitespace), comment(comment), syntax(syntax), total(total) {}

    void inc_whitespace() {

        ++whitespace;

    }

     void inc_comment() {

        ++comment;

    }

    void inc_syntax() {

        ++syntax;

    }

    void inc_total() {

        ++total;

    }

    bool operator==(const counts_t & other_count) const {


        return whitespace == other_count.whitespace && comment == other_count.comment && syntax == other_count.syntax && total == other_count.total;

    }

    bool operator!=(const counts_t & other_count) const {


        return !(*this == other_count);

    }

    friend std::ostream & operator<<(std::ostream & out, const counts_t & count) {


        return out << "Whitespace: " << count.whitespace << "\tComment: " << count.comment << "\tSyntax: " << count.syntax << "\tTotal: " << count.total;

    }

};

#endif
