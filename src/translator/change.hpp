// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file change.hpp
 *
 * @copyright Copyright (C) 2025-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CHANGE_HPP
#define INCLUDED_CHANGE_HPP

#include <construct.hpp>
#include <operation.hpp>

#include <memory>
#include <list>

namespace srcdiff {

struct change {
    const construct::construct_list_view original;
    const construct::construct_list_view modified;

    int similarity;
    enum operation operation;

    change(const construct::construct_list_view original, const construct::construct_list_view modified,
           int similarity, enum operation operation)
        : original(original), modified(modified), similarity(similarity), operation(operation) {}
};

}

#endif