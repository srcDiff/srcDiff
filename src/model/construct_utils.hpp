/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CONSTRUCT_UTILS_HPP
#define INCLUDED_CONSTRUCT_UTILS_HPP

class construct;

#include <functional>
#include <string>
#include <memory>

#include <srcml_nodes.hpp>

namespace std { 

  template<>
  struct hash<construct> {
    size_t operator()(const construct & element) const;
  };

  template<>
  struct hash<std::shared_ptr<const construct>> {
    size_t operator()(const std::shared_ptr<const construct> & element) const;
  };


  template<>
  struct equal_to<std::shared_ptr<const construct>>{
    bool operator()(std::shared_ptr<const construct> self, std::shared_ptr<const construct> that) const;
  };

}

#endif
