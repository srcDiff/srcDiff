// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file construct.txx
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

template <class nest_rule_checker, class convert_rule_checker>
void construct::set_rule_checkers() {
  nest_checker = std::make_shared<nest_rule_checker>(*this);
  convert_checker = std::make_shared<convert_rule_checker>(*this);
}
