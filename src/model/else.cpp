/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <else.hpp>

std::shared_ptr<const construct> else_t::condition() const {
   return std::shared_ptr<const construct>();
}
