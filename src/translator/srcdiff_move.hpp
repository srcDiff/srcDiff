/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_SRCDIFF_MOVE_HPP
#define INCLUDED_SRCDIFF_MOVE_HPP

#include <srcdiff_output.hpp>

#include <construct.hpp>
#include <shortest_edit_script.h>

class srcdiff_move : public srcdiff_output {

protected:

	std::size_t & position;
	int operation;

private:

public:

	srcdiff_move(const srcdiff_output & out, std::size_t & position, int operation);

	static bool is_move(std::shared_ptr<const construct> set);

	static void mark_moves(const construct::construct_list_view original,
                           const construct::construct_list_view modified,
                           edit_t * edit_script);

	virtual void output();


};


#endif
