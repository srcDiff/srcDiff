/*
  Options.hpp

  Copyright (C) 2005-2010  SDML (www.sdml.info)

  This file is part of the srcML translator.

  The srcML translator is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcML translator is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcML translator; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Options for translation output.
*/

#ifndef INCLUDED_METHOD_HPP
#define INCLUDED_METHOD_HPP

#ifdef __GNUC__
  typedef unsigned long long OPTION_TYPE;
#else
  typedef unsigned __int64 OPTION_TYPE;
#endif

#ifdef __GNUC__
#define ull(a) a##ULL
#else
#define ull(a) a##i64
#endif

inline bool ismethod(OPTION_TYPE options, OPTION_TYPE flag) {
  return (flag & options) > 0;
}

const char * COLLECT_METHOD = "collect";
const char * RAW_METHOD = "raw";


// @srcdiff
const OPTION_TYPE OPTION_RAW       = ull(1) << 0;

#endif
