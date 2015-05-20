#ifndef INCLUDED_SUMMARY_T_HPP
#define INCLUDED_SUMMARY_T_HPP

#include <counts_t.hpp>
#include <versioned_string.hpp>
#include <srcdiff_type.hpp>
#include <summary_type.hpp>
#include <namespace_uri.hpp>
#include <srcdiff_macros.hpp>
#include <identifier_diff.hpp>
#include <summary_output_stream.hpp>
#include <type_query.hpp>

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <memory>

class summary_t {

    private:

    public:

        size_t id;
        versioned_string type_name;
        namespace_uri uri;
        srcdiff_type operation;

    public:

        summary_t(std::string type_name = "", namespace_uri uri = SRC, srcdiff_type operation = SRCDIFF_COMMON)
            : id(0), type_name(type_name), uri(uri), operation(operation) {}

        bool operator<(const summary_t & summary) const {

            return id < summary.id;

        }

        friend bool operator<(const std::shared_ptr<summary_t> & summary_one, const std::shared_ptr<summary_t> & summary_two) {

            return summary_one->id < summary_two->id;

        }

        bool operator==(const summary_t & summary) const {

            return id == summary.id;

        }

        friend bool operator==(const std::shared_ptr<summary_t> & summary_one, const std::shared_ptr<summary_t> & summary_two) {

            return summary_one->id == summary_two->id;

        }

        bool operator==(int summary_id) const {

            return id == summary_id;

        }

        friend bool operator==(const std::shared_ptr<summary_t> & summary_one, size_t summary_id) {

            return summary_one->id == summary_id;

        }

};

#endif
