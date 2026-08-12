// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_stream.hpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_INPUT_STREAM_MANAGER_HPP
#define INCLUDED_INPUT_STREAM_MANAGER_HPP

#include <input_stream.hpp>
#include <nodes.hpp>

#include <srcml.h>

#include <optional>
#include <functional>
#include <thread>
#include <memory>

namespace srcdiff {

class input_stream_manager {
public:

    input_stream_manager(bool should_split_strings) 
        : converter(should_split_strings), original_streams(), modified_streams() {
    }
    ~input_stream_manager() {}

    void append_original_stream(std::shared_ptr<abstract_input_stream> stream) {
        original_streams.push_back(stream);
    }

    void append_modified_stream(std::shared_ptr<abstract_input_stream> stream) {
        modified_streams.push_back(stream);
    }

    std::pair<srcML::nodes, srcML::nodes> consume_streams() {
        /// @todo handle better
        if(original_streams.empty() || modified_streams.empty()) return std::pair<srcML::nodes, srcML::nodes>();

        std::pair<srcML::nodes, srcML::nodes> nodes;
        if(original_streams.front()) {
            std::thread thread_original(std::ref(*original_streams.front().get()), std::ref(converter), std::ref(nodes.first));
            thread_original.join();
        }
        original_streams.pop_front();

        if(modified_streams.front()) {
            std::thread thread_modified(std::ref(*modified_streams.front().get()), std::ref(converter), std::ref(nodes.second));
            thread_modified.join();
        }
        modified_streams.pop_front();

        return nodes;
    }

protected:
    srcML::converter converter;
    std::list<std::shared_ptr<abstract_input_stream>> original_streams;
    std::list<std::shared_ptr<abstract_input_stream>> modified_streams;
};

}

#endif
