// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_types.hpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 *
 * Defines the types for libsrcdiff.
 */

#include <input_stream_manager.hpp>
#include <deltor.hpp>

#include <memory>

struct srcdiff_config {
    srcdiff_config() {}
    ~srcdiff_config() {}

    std::unique_ptr<srcdiff::input_stream_manager> manager;
    std::unique_ptr<srcdiff::deltor> deltor;
    METHOD_TYPE method;
    srcdiff::OPTION_TYPE options;
};
