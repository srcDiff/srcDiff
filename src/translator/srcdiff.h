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

  LIBSRCDIFF_EXPORT int srcdiff_archive_is_solitary_unit(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_enable_solitary_unit(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_disable_solitary_unit(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_has_hash(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_enable_hash(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_disable_hash(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_set_xml_encoding(struct srcdiff_archive* archive, const char* encoding);
  LIBSRCDIFF_EXPORT int srcdiff_archive_set_src_encoding(struct srcdiff_archive* archive, const char* encoding);
  LIBSRCDIFF_EXPORT int srcdiff_archive_set_language(struct srcdiff_archive* archive, const char* language);
  LIBSRCDIFF_EXPORT int srcdiff_archive_set_options(struct srcdiff_archive* archive, size_t option);
  LIBSRCDIFF_EXPORT int srcdiff_archive_enable_option(struct srcdiff_archive* archive, size_t option);
  LIBSRCDIFF_EXPORT int srcdiff_archive_disable_option(struct srcdiff_archive* archive, size_t option);
  LIBSRCDIFF_EXPORT int srcdiff_archive_set_tabstop(struct srcdiff_archive* archive, size_t tabstop);
  LIBSRCDIFF_EXPORT int srcdiff_archive_register_file_extension(struct srcdiff_archive* archive, const char* extension, const char* language);
  LIBSRCDIFF_EXPORT int srcdiff_archive_register_namespace(struct srcdiff_archive* archive, const char* prefix, const char* uri);
  LIBSRCDIFF_EXPORT int srcdiff_archive_add_attribute(struct srcdiff_archive* archive, const char* uri, const char* name, const char* value);
  LIBSRCDIFF_EXPORT size_t srcdiff_archive_get_attribute_size(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_attribute_prefix(const struct srcdiff_archive* archive, size_t pos);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_attribute_name(const struct srcdiff_archive* archive, size_t pos);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_attribute_value(const struct srcdiff_archive* archive, size_t pos);
  LIBSRCDIFF_EXPORT int srcdiff_archive_set_processing_instruction(struct srcdiff_archive* archive, const char* target, const char* data);
  LIBSRCDIFF_EXPORT int srcdiff_archive_set_url(struct srcdiff_archive* archive, const char* url);
  LIBSRCDIFF_EXPORT int srcdiff_archive_set_version(struct srcdiff_archive* archive, const char* version);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_xml_encoding(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_src_encoding(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_revision(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_language(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_url(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_version(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_get_options(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT size_t srcdiff_archive_get_tabstop(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT size_t srcdiff_archive_get_namespace_size(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_namespace_prefix(const struct srcdiff_archive* archive, size_t pos);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_prefix_from_uri(const struct srcdiff_archive* archive, const char* namespace_uri);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_namespace_uri(const struct srcdiff_archive* archive, size_t pos);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_uri_from_prefix(const struct srcdiff_archive* archive, const char* prefix);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_processing_instruction_target(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_get_processing_instruction_data(const struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT const char* srcdiff_archive_check_extension(const struct srcdiff_archive* archive, const char* filename);

  LIBSRCDIFF_EXPORT struct srcdiff_unit* srcdiff_archive_read_unit_header(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT struct srcdiff_unit* srcdiff_archive_read_unit(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT int srcdiff_archive_skip_unit(struct srcdiff_archive* archive);

// transformation library. May not implement.
// LIBSRCDIFF_EXPORT int srcml_append_transform_xpath(struct srcdiff_archive* archive, const char* xpath_string);
// LIBSRCDIFF_EXPORT int srcml_append_transform_xpath_attribute(struct srcdiff_archive* archive, const char* xpath_string,
//                                                             const char* prefix, const char* namespace_uri,
//                                                             const char* attr_name, const char* attr_value);
// LIBSRCDIFF_EXPORT int srcml_append_transform_xpath_element(struct srcdiff_archive* archive, const char* xpath_string,
//                                                             const char* prefix, const char* namespace_uri,
//                                                             const char* element);
// LIBSRCDIFF_EXPORT int srcml_append_transform_xslt_filename(struct srcdiff_archive* archive, const char* xslt_filename);
// LIBSRCDIFF_EXPORT int srcml_append_transform_xslt_memory(struct srcdiff_archive* archive, const char* xslt_buffer, size_t size);
// LIBSRCDIFF_EXPORT int srcml_append_transform_xslt_FILE(struct srcdiff_archive* archive, FILE* xslt_file);
// LIBSRCDIFF_EXPORT int srcml_append_transform_xslt_fd(struct srcdiff_archive* archive, int xslt_fd);
// LIBSRCDIFF_EXPORT int srcml_append_transform_relaxng_filename(struct srcdiff_archive* archive, const char* relaxng_filename);
// LIBSRCDIFF_EXPORT int srcml_append_transform_relaxng_memory(struct srcdiff_archive* archive, const char* relaxng_buffer, size_t size);
// LIBSRCDIFF_EXPORT int srcml_append_transform_relaxng_FILE(struct srcdiff_archive* archive, FILE* relaxng_file);
// LIBSRCDIFF_EXPORT int srcml_append_transform_relaxng_fd(struct srcdiff_archive* archive, int relaxng_fd);
// LIBSRCDIFF_EXPORT int srcml_append_transform_srcql(struct srcdiff_archive* archive, const char* srcql_string);
// LIBSRCDIFF_EXPORT int srcml_append_transform_srcql_attribute(struct srcdiff_archive* archive, const char* srcql_string,
//                                                             const char* prefix, const char* namespace_uri,
//                                                             const char* attr_name, const char* attr_value);
// LIBSRCDIFF_EXPORT int srcml_append_transform_srcql_element(struct srcdiff_archive* archive, const char* srcql_string,
//                                                             const char* prefix, const char* namespace_uri,
//                                                             const char* element);
// LIBSRCDIFF_EXPORT int srcml_append_transform_param(struct srcdiff_archive* archive, const char* param_name, const char* param_value);
// LIBSRCDIFF_EXPORT int srcml_append_transform_stringparam(struct srcdiff_archive* archive, const char* param_name, const char* param_value);
// LIBSRCDIFF_EXPORT int srcdiff_unit_apply_transforms(struct srcdiff_archive* archive, struct srcdiff_unit* unit, struct srcml_transform_result** result);
// LIBSRCDIFF_EXPORT int srcml_transform_get_type(struct srcml_transform_result* result);
// LIBSRCDIFF_EXPORT int srcml_transform_get_unit_size(struct srcml_transform_result* result);
// LIBSRCDIFF_EXPORT struct srcdiff_unit* srcml_transform_get_unit(struct srcml_transform_result* result, int index);
// LIBSRCDIFF_EXPORT const char* srcml_transform_get_string(struct srcml_transform_result* result);
// LIBSRCDIFF_EXPORT double srcml_transform_get_number(struct srcml_transform_result* result);
// LIBSRCDIFF_EXPORT int srcml_transform_get_bool(struct srcml_transform_result* result);
// LIBSRCDIFF_EXPORT int srcml_transform_free(struct srcml_transform_result* result);
// LIBSRCDIFF_EXPORT int srcml_clear_transforms(struct srcdiff_archive* archive);

  // srcdiff unit api
  LIBSRCDIFF_EXPORT struct srcdiff_unit* srcdiff_unit_create(struct srcdiff_archive* archive);
  LIBSRCDIFF_EXPORT struct srcdiff_unit* srcdiff_unit_clone(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT int srcdiff_unit_error_number(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_error_string(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT void srcdiff_unit_free(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT int srcdiff_unit_set_src_encoding(struct srcdiff_unit* unit, const char* encoding);
  LIBSRCDIFF_EXPORT int srcdiff_unit_set_language(struct srcdiff_unit* unit, const char* language);
  LIBSRCDIFF_EXPORT int srcdiff_unit_set_filename(struct srcdiff_unit* unit, const char* filename);
  LIBSRCDIFF_EXPORT int srcdiff_unit_set_version(struct srcdiff_unit* unit, const char* version);
  LIBSRCDIFF_EXPORT int srcdiff_unit_set_timestamp(struct srcdiff_unit* unit, const char* timestamp);
  LIBSRCDIFF_EXPORT int srcdiff_unit_set_eol(struct srcdiff_unit* unit, size_t eol);
  LIBSRCDIFF_EXPORT int srcdiff_unit_register_namespace(struct srcdiff_unit* unit, const char* prefix, const char* uri);
  LIBSRCDIFF_EXPORT int srcdiff_unit_add_attribute(struct srcdiff_unit* unit, const char* uri, const char* name, const char* value);
  LIBSRCDIFF_EXPORT size_t srcdiff_unit_get_attribute_size(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_attribute_prefix(const struct srcdiff_unit* unit, size_t pos);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_attribute_name(const struct srcdiff_unit* unit, size_t pos);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_attribute_value(const struct srcdiff_unit* unit, size_t pos);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_src_encoding(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_revision(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_language(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_filename(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_version(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_timestamp(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_hash(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT int srcdiff_unit_get_loc(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT size_t srcdiff_unit_get_eol(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_srcml(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_srcml_outer(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_srcml_inner(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT size_t srcdiff_unit_get_namespace_size(const struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_namespace_prefix(const struct srcdiff_unit* unit, size_t pos);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_prefix_from_uri(const struct srcdiff_unit* unit, const char* namespace_uri);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_namespace_uri(const struct srcdiff_unit* unit, size_t pos);
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_uri_from_prefix(const struct srcdiff_unit* unit, const char* prefix);
  LIBSRCDIFF_EXPORT int srcdiff_unit_parse_filename(struct srcdiff_unit* unit, const char* src_filename);
  LIBSRCDIFF_EXPORT int srcdiff_unit_parse_memory(struct srcdiff_unit* unit, const char* src_buffer, size_t buffer_size);
  LIBSRCDIFF_EXPORT int srcdiff_unit_parse_FILE(struct srcdiff_unit* unit, FILE* src_file);
  LIBSRCDIFF_EXPORT int srcdiff_unit_parse_fd(struct srcdiff_unit* unit, int src_fd);
  LIBSRCDIFF_EXPORT int srcdiff_unit_parse_io(struct srcdiff_unit* unit, void * context, ssize_t (*read_callback)(void * context, void * buffer, size_t len), int (*close_callback)(void * context));
  LIBSRCDIFF_EXPORT const char* srcdiff_unit_get_src(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT ssize_t srcdiff_unit_get_src_size(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT int srcdiff_unit_unparse_filename(struct srcdiff_unit* unit, const char* src_filename);
  LIBSRCDIFF_EXPORT int srcdiff_unit_unparse_memory(struct srcdiff_unit* unit, char** src_buffer, size_t * src_size);
  LIBSRCDIFF_EXPORT int srcdiff_unit_unparse_FILE(struct srcdiff_unit* unit, FILE* file);
  LIBSRCDIFF_EXPORT int srcdiff_unit_unparse_fd(struct srcdiff_unit* unit, int fd);
  LIBSRCDIFF_EXPORT int srcdiff_unit_unparse_io(struct srcdiff_unit* unit, void * context, int (*write_callback)(void * context, const char* buffer, int len), int (*close_callback)(void * context));

  // write-by api. Don't think it is needed
  LIBSRCDIFF_EXPORT int srcml_write_start_unit(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT int srcml_write_end_unit(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT int srcml_write_start_element(struct srcdiff_unit* unit, const char* prefix, const char* name, const char* uri);
  LIBSRCDIFF_EXPORT int srcml_write_end_element(struct srcdiff_unit* unit);
  LIBSRCDIFF_EXPORT int srcml_write_namespace(struct srcdiff_unit* unit, const char* prefix, const char* uri);
  LIBSRCDIFF_EXPORT int srcml_write_attribute(struct srcdiff_unit* unit, const char* prefix, const char* name, const char* uri, const char* content);
  LIBSRCDIFF_EXPORT int srcml_write_string(struct srcdiff_unit* unit, const char* content);

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
