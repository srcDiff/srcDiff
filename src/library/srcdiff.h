// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff.h
 *
 * @copyright Copyright (C) 2024-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_H
#define INCLUDED_SRCDIFF_H

#include <stddef.h>    /* size_t  */
#include <sys/types.h> /* ssize_t */
#include <stdio.h>     /* FILE*   */

#ifndef __cplusplus
extern "C" {
#endif

/* Function export macro */
#if defined(WIN32) && !defined(__MINGW32__)
#define LIBSRCDIFF_EXPORT __declspec(dllexport)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#define LIBSRCDIFF_EXPORT
#endif

/**
 * @struct srcdiff_config
 *
 * Configuration object for controlling srcDiff delta creation.
 */
struct srcdiff_config;

// srcDiff convenience function - original and modified to/from srcDiff archive
int srcDiff(const char* original_filename, 
            const char* modified_filename, 
            const char* srcdiff_filename);



LIBSRCDIFF_EXPORT struct srcdiff_config* srcdiff_config_create(struct srcml_archive * archive);
LIBSRCDIFF_EXPORT void                   srcdiff_config_free  (struct srcdiff_config* config);
LIBSRCDIFF_EXPORT struct srcml_unit    * srcdiff_create_delta (struct srcml_unit    * original_unit, 
                                                               struct srcml_unit    * modified_unit,
                                                               struct srcdiff_config* config);

// use revision instead of version in naming
/** Constant for original srcDiff revision number */
#define SRCDIFF_REVISION_ORIGINAL 0
/** Constant for modified srcDiff revision number */
#define SRCDIFF_REVISION_MODIFIED 1
/** Constant for an invalid srcDiff revision number */
#define SRCDIFF_REVISION_INVALID  2

// srcML srcDiff functions
// LIBSRCDIFF_EXPORT size_t srcml_get_srcdiff_revision();
// LIBSRCDIFF_EXPORT size_t srcdiff_archive_get_srcdiff_revision(const struct srcdiff_archive* archive);
// LIBSRCDIFF_EXPORT int srcdiff_archive_set_srcdiff_revision(struct srcdiff_archive* archive, size_t revision_number);

#ifndef __cplusplus
}
#endif

#endif
