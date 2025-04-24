// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file move_detector.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_MOVE_DETECTOR_HPP
#define INCLUDED_MOVE_DETECTOR_HPP

#include <output_stream.hpp>

#include <construct.hpp>
#include <operation.hpp>

#include <shortest_edit_script.h>

namespace srcdiff {

class move_detector : public output_stream {

protected:

    std::size_t & position;
    enum operation operation;

private:

public:

    move_detector(const output_stream& out, std::size_t& position, enum operation operation);

    static bool is_move(std::shared_ptr<const construct> set);

    static void mark_moves(const construct::construct_list_view original,
                           const construct::construct_list_view modified,
                           const ses::edit_list& edits);

    virtual void output();


};

}

#endif
