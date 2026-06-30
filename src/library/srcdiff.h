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

/**@{ @name Status */
/** Return status indicating no errors */
#define SRCDIFF_STATUS_OK    0
/** Return status indicating general errors occurred */
#define SRCDIFF_STATUS_ERROR 1
/**@}*/

/**
 * @struct srcml_unit
 *
 * The srcML markup of one source-code file.
 * See srcML.org
 */
struct srcml_unit;

/**
 * @struct srcdiff_config
 *
 * Configuration object for controlling srcDiff delta creation.
 */
struct srcdiff_config;

/** @defgroup convenience Convenience functions
  @{
 */

/**
 * Create the srcDiff delta in the srcDiff format
 * @details @todo
 * @param [in] original_filename The name of the original source-code file
 * @param [in] modified_filename The name of the modified source-code file
 * @param [in] srcdiff_filename  The name of the srcDiff file
 * @return SRCDIFF_STATUS_OK on success
 * @return Status error on failure
 */
LIBSRCDIFF_EXPORT int srcDiff(const char* original_filename, 
                              const char* modified_filename, 
                              const char* srcdiff_filename);
/**@}*/

/** @defgroup create Create
    @{
*/
/**
 * Create a new srcdiff_config.
 * @note The config must be freed using srcdiff_config_free()
 * @returns Created srcdiff_config on success
 * @return NULL on failure
 */
LIBSRCDIFF_EXPORT struct srcdiff_config* srcdiff_config_create();

/**
 * Free an allocated config
 * @param config The srcdiff config to free
 */
LIBSRCDIFF_EXPORT void srcdiff_config_free  (struct srcdiff_config* config);

/**
 * Create the srcDiff delta of the original and modifid srcml_unit in the srcDiff format.
 * @param config A srcdiff_config to direct delta creation, Null uses default options 
 * @param original_unit A srcml_unit of the original source-code
 * @param modified_unit A srcml_unit of the modified source-code
 * @returns srcml_unit containing the srcDiff delta
 * @return NULL on failure
 */
LIBSRCDIFF_EXPORT struct srcml_unit* srcdiff_create_delta(struct srcdiff_config* config,
                                                          struct srcml_unit    * original_unit, 
                                                          struct srcml_unit    * modified_unit);
/**@}*/

/** @defgroup read Read
    @{
*/
/** Constant for original srcDiff revision number */
#define SRCDIFF_ORIGINAL 0
/** Constant for modified srcDiff revision number */
#define SRCDIFF_MODIFIED 1

/**
 * Read the original srcML from a unit containing srcDiff.
 * @param unit A srcml_unit containing srcDiff
 * @returns srcml_unit containing the original srcML
 * @return NULL on failure
 */
LIBSRCDIFF_EXPORT struct srcml_unit* srcdiff_read_unit_original(struct srcml_unit* unit);

/**
 * Read the modified srcML from a unit containing srcDiff.
 * @param unit A srcml_unit containing srcDiff
 * @returns srcml_unit containing the modified srcML
 * @return NULL on failure
 */
LIBSRCDIFF_EXPORT struct srcml_unit* srcdiff_read_unit_modified(struct srcml_unit* unit);

/**
 * Read a srcML revision from a unit containing srcDiff.
 * @param unit            A srcml_unit containing srcDiff
 * @param revision_number The revision_number to read from unit
 * @returns srcml_unit containing the specified srcML revision
 * @return NULL on failure
 */
LIBSRCDIFF_EXPORT struct srcml_unit* srcdiff_read_unit_revision(struct srcml_unit* unit,
                                                                size_t revision_number);
/**@}*/

#ifndef __cplusplus
}
#endif

#endif
