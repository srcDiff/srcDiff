/**
 * @file srcdiff_single.hpp
 *
 * @copyright Copyright (C) 2014-2023 srcML, LLC. (www.srcML.org)
 *
 * srcDiff is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * srcDiff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef INCLUDED_SRCDIFF_SINGLE_HPP
#define INCLUDED_SRCDIFF_SINGLE_HPP

#include <construct.hpp>
#include <srcdiff_output.hpp>

class srcdiff_single {

protected:

	std::shared_ptr<srcdiff_output> out;

	std::shared_ptr<const construct> original_construct;
	std::shared_ptr<const construct> modified_construct;

private:

	void output_recursive_same();
	void output_recursive_interchangeable();

public:

	srcdiff_single(std::shared_ptr<srcdiff_output> out, std::shared_ptr<const construct> original_construct,  std::shared_ptr<const construct> modified_construct);

	virtual void output();

};

#endif
