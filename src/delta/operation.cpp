// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file operation.cpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <operation.hpp>

#include <unordered_map>

namespace srcdiff {

operation string_to_operation(const std::string& str) {
	static std::unordered_map<std::string, operation> operation_map
		= {
			{ "common", operation::COMMON },
			{ "delete", operation::DELETE },
			{ "insert", operation::INSERT },
		  };
	std::unordered_map<std::string, operation>::iterator op_itr = operation_map.find(str);
	return op_itr != operation_map.end() ? op_itr->second : operation::NONE;
}

}
