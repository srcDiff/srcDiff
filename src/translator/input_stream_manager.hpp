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
#include <client_options.hpp>
#include <srcml_nodes.hpp>

#include <srcml.h>

#include <optional>
#include <functional>
#include <thread>
#include <memory>

namespace srcdiff {

class input_stream_manager {
public:

    input_stream_manager(srcml_archive* archive, const OPTION_TYPE& options) 
        : archive(archive), options(options) {
    }

    ~input_stream_manager() {}

    template<typename T>
    void append_stream(const input_stream<T>& stream) {
        streams.push_back(std::ref(stream));
    }

    std::pair<srcml_nodes, srcml_nodes> consume_streams() {
      /// @todo handle better
      if(streams.size() < 2) return std::pair<srcml_nodes, srcml_nodes>();

      std::pair<srcml_nodes, srcml_nodes> nodes;
      std::thread thread_original(streams.front(), std::ref(nodes.first), std::ref(archive), std::ref(options));
      thread_original.join();
      streams.pop_front();

      std::thread thread_modified(streams.front(), std::ref(nodes.second), std::ref(archive), std::ref(options));
      thread_modified.join();
      streams.pop_front();

      return nodes;
    }

protected:
    srcml_archive* archive;
    const OPTION_TYPE& options;

    std::list<std::reference_wrapper<const input_stream_base>> streams;
};

}

#endif
