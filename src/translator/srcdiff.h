// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff.h
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_H
#define INCLUDED_SRCDIFF_H

#ifndef __cplusplus
extern "C" {
#endif

  int srcdiff(const char * original_filename, const char * modified_filename, const char * output_filename);

#ifndef __cplusplus
}
#endif

#endif
