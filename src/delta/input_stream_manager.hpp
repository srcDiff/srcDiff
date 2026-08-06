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

    input_stream_manager(bool should_split_strings) : converter(should_split_strings) {
    }
    ~input_stream_manager() {}

    void append_stream(std::unique_ptr<abstract_input_stream> stream) {
        streams.push_back(std::move(stream));
    }

    std::pair<srcML::nodes, srcML::nodes> consume_streams() {
      /// @todo handle better
      if(streams.size() < 2) return std::pair<srcML::nodes, srcML::nodes>();

      std::pair<srcML::nodes, srcML::nodes> nodes;
      std::thread thread_original(std::ref(*streams.front().get()), std::ref(converter), std::ref(nodes.first));
      thread_original.join();
      streams.pop_front();

      std::thread thread_modified(std::ref(*streams.front().get()), std::ref(converter), std::ref(nodes.second));
      thread_modified.join();
      streams.pop_front();

      return nodes;
    }

protected:
    srcML::converter converter;
    std::list<std::unique_ptr<abstract_input_stream>> streams;
};

}

#endif
