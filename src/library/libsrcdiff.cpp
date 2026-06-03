// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file libsrcdiff.cpp
 *
 * @copyright Copyright (C) 2024-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff.h>
#include <srcdiff_types.hpp>

#include <delta.hpp>
#include <output_stream.hpp>
#include <input_stream.hpp>
#include <client_options.hpp>

#include <string>
#include <optional>
#include <fstream>
#include <memory>

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

    srcdiff::client_options options;
    options.output_filename = output_filename;

    file_input in;
    srcdiff::input_stream<file_input> input_original(in, original_path, language_string);
    srcdiff::input_stream<file_input> input_modified(in, modified_path, language_string);

    srcdiff::input_stream_manager manager(options.archive, options.flags);
    manager.append_stream(input_original);
    manager.append_stream(input_modified);

    srcdiff::delta delta(options.archive, options.methods, options.unit_filename);
    delta.create(manager, language_string, unit_filename, unit_version);

    return 0;
}

struct srcdiff_config* srcdiff_config_create(struct srcml_archive* archive) {
  srcdiff_config* config = new srcdiff_config();
  config->deltor = std::make_unique<srcdiff::delta>(archive, config->method, std::optional<std::string>());
  return config;
}

void srcdiff_config_free(struct srcdiff_config* config) {
  delete config;
}

class srcml_unit_input {
public:
  srcml_unit_input(srcml_unit* unit) : unit(unit) {
  }

  srcml_unit*    open(const char* uri) const { return unit; }
  static ssize_t read(void* context, void* buffer, size_t len) {}
  static int     close(void* context) {}
private:
  srcml_unit* unit;
};

struct srcml_unit* srcdiff_create_delta(struct srcdiff_unit  * original_unit, 
                                        struct srcdiff_unit  * modified_unit,
                                        struct srcdiff_config* config) {
  if(!original_unit) return nullptr;
  if(!modified_unit) return nullptr;
  if(!config)        return nullptr;

  // config->deltor->create();

  return nullptr;
}
