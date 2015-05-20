#ifndef INCLUDED_SUMMARY_T_HPP
#define INCLUDED_SUMMARY_T_HPP

#include <srcdiff_type.hpp>
#include <namespace_uri.hpp>
#include <srcdiff_macros.hpp>
#include <type_query.hpp>

class summary_t {

    public:

        enum summary_name_t { IDENTIFIER, REPLACEMENT, NONE };

    private:

        summary_name_t type;
        namespace_uri uri;
        srcdiff_type operation;

    public:

        summary_t(summary_name_t type, namespace_uri uri = SRC, srcdiff_type operation = SRCDIFF_COMMON)
            : type(type), uri(uri), operation(operation) {}

        // bool operator<(const summary_t & summary) const {

        //     return id < summary.id;

        // }

        // friend bool operator<(const std::shared_ptr<summary_t> & summary_one, const std::shared_ptr<summary_t> & summary_two) {

        //     return summary_one->id < summary_two->id;

        // }

        // bool operator==(const summary_t & summary) const {

        //     return id == summary.id;

        // }

        // friend bool operator==(const std::shared_ptr<summary_t> & summary_one, const std::shared_ptr<summary_t> & summary_two) {

        //     return summary_one->id == summary_two->id;

        // }

        // bool operator==(int summary_id) const {

        //     return id == summary_id;

        // }

        // friend bool operator==(const std::shared_ptr<summary_t> & summary_one, size_t summary_id) {

        //     return summary_one->id == summary_id;

        // }

};

#endif
