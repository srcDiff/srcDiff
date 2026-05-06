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

  // temp forward declaration of srcDiff types
  struct srcdiff_archive;
  struct srcdiff_unit;

  // srcDiff convenience function - original and modified to/from srcDiff archive
  int srcDiff(const char * original_filename, const char * modified_filename, const char * srcdiff_filename);

  LIBSRCDIFF_EXPORT struct srcdiff_archive* srcdiff_archive_create();

  // srcdiff archive api
  LIBSRCDIFF_EXPORT struct srcdiff_archive* srcdiff_archive_clone(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_error_number(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_error_string(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_write_unit(struct srcdiff_archive* archive, struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT int srcdiff_archive_write_string(struct srcdiff_archive* archive, const char* s, int len);
  LIBSRCDIFF_EXPORT void srcdiff_archive_close(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT void srcdiff_archive_free(struct srcdiff_archive* archive);

  LIBSRCDIFF_EXPORT int srcdiff_archive_write_open_filename(struct srcdiff_archive* archive, const char* srcml_filename);
  LIBSRCDIFF_EXPORT int srcdiff_archive_write_open_memory(struct srcdiff_archive* archive, char** buffer, size_t * size);
  LIBSRCDIFF_EXPORT int srcdiff_archive_write_open_FILE(struct srcdiff_archive* archive, FILE* srcml_file);
  LIBSRCDIFF_EXPORT int srcdiff_archive_write_open_fd(struct srcdiff_archive* archive, int srcml_fd);
  LIBSRCDIFF_EXPORT int srcdiff_archive_write_open_io(struct srcdiff_archive* archive, void * context, int (*write_callback)(void * context, const char* buffer, int len), int (*close_callback)(void * context));

  LIBSRCDIFF_EXPORT int srcdiff_archive_read_open_filename(struct srcdiff_archive* archive, const char* srcml_filename);
  LIBSRCDIFF_EXPORT int srcdiff_archive_read_open_memory(struct srcdiff_archive* archive, const char* buffer, size_t buffer_size);
  LIBSRCDIFF_EXPORT int srcdiff_archive_read_open_FILE(struct srcdiff_archive* archive, FILE* srcml_file);
  LIBSRCDIFF_EXPORT int srcdiff_archive_read_open_fd(struct srcdiff_archive* archive, int srcml_fd);
  LIBSRCDIFF_EXPORT int srcdiff_archive_read_open_io(struct srcdiff_archive* archive, void * context, int (*read_callback)(void * context, char* buffer, int len), int (*close_callback)(void * context));

  // add get_archive
  // Do not mirror/implement.

  // need a utility function to split attributes into: check if diff, original/modified

  LIBSRCDIFF_EXPORT struct srcdiff_unit* srcdiff_archive_read_unit_header(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT struct srcdiff_unit* srcdiff_archive_read_unit(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_skip_unit(struct srcdiff_archive* archive);

// transformation library. Not needed, but maybe a register ext func in future

  // srcdiff unit api
  LIBSRCDIFF_EXPORT struct srcdiff_unit* srcdiff_unit_create(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT struct srcdiff_unit* srcdiff_unit_clone(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT int srcdiff_unit_error_number(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_error_string(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT void srcdiff_unit_free(struct srcdiff_unit* unit);

  // a srcdiff_unit get original/modified srcml_unit
  // convience get original/modified and general get_version
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_srcml(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_srcml_outer(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_srcml_inner(struct srcdiff_unit* unit);

  // parse be rename a difference/delta
  // original/modified file (other srcML IO), original/modified srcml_unit
  // do you always give srcml_unit and run on that
  LIBSRCDIFF_EXPORT int srcdiff_unit_parse_filename(struct srcdiff_unit* unit, const char* src_filename);
  LIBSRCDIFF_EXPORT int srcdiff_unit_parse_memory(struct srcdiff_unit* unit, const char* src_buffer, size_t buffer_size);
  LIBSRCDIFF_EXPORT int srcdiff_unit_parse_FILE(struct srcdiff_unit* unit, FILE* src_file);
  LIBSRCDIFF_EXPORT int srcdiff_unit_parse_fd(struct srcdiff_unit* unit, int src_fd);
  LIBSRCDIFF_EXPORT int srcdiff_unit_parse_io(struct srcdiff_unit* unit, void * context, ssize_t (*read_callback)(void * context, void * buffer, size_t len), int (*close_callback)(void * context));

  // unparse is not needed
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_src(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT ssize_t srcdiff_unit_get_src_size(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT int srcdiff_unit_unparse_filename(struct srcdiff_unit* unit, const char* src_filename);
  LIBSRCDIFF_EXPORT int srcdiff_unit_unparse_memory(struct srcdiff_unit* unit, char** src_buffer, size_t * src_size);
  LIBSRCDIFF_EXPORT int srcdiff_unit_unparse_FILE(struct srcdiff_unit* unit, FILE* file);
  LIBSRCDIFF_EXPORT int srcdiff_unit_unparse_fd(struct srcdiff_unit* unit, int fd);
  LIBSRCDIFF_EXPORT int srcdiff_unit_unparse_io(struct srcdiff_unit* unit, void * context, int (*write_callback)(void * context, const char* buffer, int len), int (*close_callback)(void * context));

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
