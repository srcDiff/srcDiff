// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file libsrcdiff.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff.h>

#include <translator.hpp>
#include <output_stream.hpp>
#include <input_stream.hpp>

#include <string>
#include <optional>
#include <fstream>

class file_input {
public:

    struct input_context {

    std::ifstream in;

    };

    input_context* open(const char * uri) const {

      input_context* context = new input_context;
      context->in.open(uri);

      return context->in ? context : (delete context, nullptr);

    }

    static ssize_t read(void * context, void * buffer, size_t len) {

      input_context* ctx = (input_context *)context;
      ctx->in.read((char *)buffer, len);

      return ctx->in.gcount();
    }

    static int close(void * context) {

      input_context* ctx = (input_context *)context;
      ctx->in.close();
      delete ctx;

      return 1;
    }

};

int srcDiff(const char * original_filename, const char* modified_filename, const char* output_filename){
    /// @todo give actual error codes
    if(original_filename == nullptr && modified_filename == nullptr) return 1;
    if(output_filename == nullptr) return 1;

    const char* path = "";
    if(original_filename)      path = original_filename;
    else if(modified_filename) path = modified_filename;

    const char* language_string = srcml_check_extension(path);

    if(language_string == SRCML_LANGUAGE_NONE) return SRCML_LANGUAGE_NONE;

    const char* unit_version = srcml_get_version();

    std::optional<std::string> original_path = original_filename ? original_filename : std::optional<std::string>();
    std::optional<std::string> modified_path = modified_filename ? modified_filename : std::optional<std::string>();

    std::string unit_filename = original_path ? *original_path : std::string();
    if(modified_path || unit_filename != *modified_path) {
        unit_filename += "|";
        unit_filename += *modified_path;
    }

    srcdiff_options options;
    options.srcdiff_filename = output_filename;

    file_input in;
    srcdiff::input_stream<file_input> input_original(options.archive, original_path, language_string, options.flags, in);
    srcdiff::input_stream<file_input> input_modified(options.archive, modified_path, language_string, options.flags, in);

    srcdiff::translator translator(options.srcdiff_filename, options.flags, options.methods, options.archive,
                                   options.unit_filename,
                                   options.view_options,
                                   options.summary_type_str);

    translator.translate(input_original, input_modified, language_string, unit_filename, unit_version);

    return 0;
}