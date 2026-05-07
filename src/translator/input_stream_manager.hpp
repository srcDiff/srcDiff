// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_stream.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
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
#include <thread>
#include <memory>

namespace srcdiff {

class input_stream_manager {
public:

    input_stream_manager(srcml_archive* archive, const OPTION_TYPE& options) 
        : archive(archive), options(options) {
    }

    ~input_stream_manager() {}

    template<typename T, typename U>
    void input_streams(const input_stream<T>& original_stream,
                       srcml_nodes& original_nodes, 
                       const input_stream<U>& modified_stream,
                       srcml_nodes& modified_nodes) {

      std::thread thread_original(std::ref(original_stream), std::ref(original_nodes), std::ref(archive), std::ref(options));
      thread_original.join();

      std::thread thread_modified(std::ref(modified_stream), std::ref(modified_nodes), std::ref(archive), std::ref(options));
      thread_modified.join();
    }

protected:
    srcml_archive* archive;
    const OPTION_TYPE& options;
};

}

#endif
