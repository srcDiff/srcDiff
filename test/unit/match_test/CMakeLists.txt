##
#  CMakeLists.txt
#
#  Copyright (C) 2014  SDML (www.sdml.info)
#
#  This file is part of the srcDiff translator.
#
#  The srcDiff translator is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  The srcDiff translator is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the srcDiff translator; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# add tests
add_unit_test(test_is_interchangeable_match.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_extract_name.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_get_name.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_skip_tag.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_top_level_name_seek.cpp $<TARGET_OBJECTS:common>)
add_unit_test(test_get_decl_name.cpp $<TARGET_OBJECTS:common>)
