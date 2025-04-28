// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file change_list.hpp
 *
 * @copyright Copyright (C) 2025-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CHANGE_LIST_HPP
#define INCLUDED_CHANGE_LIST_HPP


#include <change.hpp>
#include <operation.hpp>
#include <construct.hpp>

#include <memory>
#include <list>

namespace srcdiff {

class change_list : public std::list<change> {
};

}

#endif