// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_common.hpp
 *
 * @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_COMMON_HPP
#define INCLUDED_SRCDIFF_COMMON_HPP

#include <srcdiff_output.hpp>

class srcdiff_common : public srcdiff_output {

protected:

	unsigned int end_original;
	unsigned int end_modified;

private:

	static const std::optional<std::string> whitespace;
	virtual void markup_common();

public:

	static void output_common(std::shared_ptr<srcdiff_output> out, int end_original, int end_modified) {
 		srcdiff_common common(*out, end_original, end_modified);
		common.output();
	}

	srcdiff_common(const srcdiff_output & out, unsigned int end_original, unsigned int end_modified);

	virtual void output();

};


#endif
