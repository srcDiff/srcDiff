// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source_manager.cpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <input_source_manager.hpp>

namespace srcdiff {

input_source_manager::input_source_manager()
	: input_sources() {
}

input_source_manager::~input_source_manager() {
}

void input_source_manager::append_source(std::unique_ptr<input_source> input) {
    input_sources.push_back(std::move(input));
}

input_source_manager::operator bool(){
	return !input_sources.empty();
}

void input_source_manager::consume() {
	assert(bool(*this));
	input_sources.front()->consume();
	input_sources.pop_front();
}

}
