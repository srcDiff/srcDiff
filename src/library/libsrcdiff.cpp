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

#include <deltor.hpp>
#include <input_stream.hpp>
#include <output_stream.hpp>
#include <operation.hpp>
#include <reader.hpp>

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
    srcdiff::input_stream<file_input> input_original(in, original_path, options.archive, language_string);
    srcdiff::input_stream<file_input> input_modified(in, modified_path, options.archive, language_string);

    srcdiff::input_stream_manager manager(options.flags);
    manager.append_stream(input_original);
    manager.append_stream(input_modified);

    srcdiff::deltor deltor(options.methods, options.unit_filename);
    deltor.create(options.archive, manager, language_string, unit_filename, unit_version);

    return SRCDIFF_STATUS_OK;
}

struct srcdiff_config* srcdiff_config_create() {
  srcdiff_config* config = new srcdiff_config();

  config->options = srcdiff::OPTION_STRING_SPLITTING;
  config->method  = 0;

  config->manager = std::make_unique<srcdiff::input_stream_manager>(config->options);
  config->deltor = std::make_unique<srcdiff::deltor>(config->method, std::optional<std::string>());
  return config;
}

void srcdiff_config_free(struct srcdiff_config* config) {
  delete config;
}

class srcml_unit_input : public srcdiff::input_stream_base {
public:
  srcml_unit_input(srcml_unit* unit) 
    : unit(unit){
  }

  virtual void operator()(srcml_converter& converter, srcml_nodes& nodes) const {
    nodes.clear();
    if(!unit) return;

    nodes = input_nodes(converter);
  }

  virtual srcml_nodes input_nodes(srcml_converter& converter) const {
    converter.convert(unit);
    return converter.create_nodes();
  }

private:
  srcml_unit* unit;
};

struct srcdiff_config_deleter { 
public:
  srcdiff_config_deleter(bool free = false) : free(free) {}
  void operator()(srcdiff_config* config) { 
    if(free) {
      srcdiff_config_free(config); 
    }
  }
private:
  bool free;
};

std::optional<std::string> srcdiff_merge_attributes(const char* original_attr, const char* modified_attr) {
    if(!original_attr && !modified_attr) return std::optional<std::string>();

    std::string merged_attr;
    if(original_attr) {
            merged_attr = original_attr;
    }
    merged_attr += '|';
    if(modified_attr) {
        merged_attr += modified_attr;
    }
    return merged_attr;
}

struct srcml_unit* srcdiff_create_delta(struct srcdiff_config* configuration,
                                        struct srcml_unit    * original_unit, 
                                        struct srcml_unit    * modified_unit) {
  if(!original_unit && !modified_unit) return nullptr;

  std::unique_ptr<srcdiff_config, srcdiff_config_deleter> config = std::unique_ptr<srcdiff_config, srcdiff_config_deleter>(configuration, srcdiff_config_deleter());
  if(!config) {
    config = std::unique_ptr<srcdiff_config, srcdiff_config_deleter>(srcdiff_config_create(), srcdiff_config_deleter(true));
  }

  srcml_unit_input original_input(original_unit);
  config->manager->append_stream(original_input);

  srcml_unit_input modified_input(modified_unit);
  config->manager->append_stream(modified_input);

  std::optional<std::string> unit_filename = srcdiff_merge_attributes(srcml_unit_get_filename(original_unit), srcml_unit_get_filename(modified_unit));
  std::optional<std::string> unit_version = srcdiff_merge_attributes(srcml_unit_get_version(original_unit), srcml_unit_get_version(modified_unit));

  srcml_archive* archive = original_unit? srcml_unit_get_archive(original_unit) : srcml_unit_get_archive(modified_unit);
  const char* language = original_unit? srcml_unit_get_language(original_unit) : srcml_unit_get_language(modified_unit);
  return config->deltor->create(archive, *config->manager, language, unit_filename, unit_version);
}

struct srcml_unit* srcdiff_read_unit_original(struct srcml_unit* unit) {
    return srcdiff_read_unit_revision(unit, SRCDIFF_ORIGINAL);
}

struct srcml_unit* srcdiff_read_unit_modified(struct srcml_unit* unit) {
    return srcdiff_read_unit_revision(unit, SRCDIFF_MODIFIED);
}

struct srcml_unit* srcdiff_read_unit_revision(struct srcml_unit* unit, size_t revision_number) {
    if(!unit) return nullptr;

    std::string srcdiff = srcml_unit_get_srcml(unit);
    if(srcdiff.empty()) return nullptr;

    srcml_unit* revision_unit = srcml_unit_create(srcml_unit_get_archive(unit));
    srcdiff::reader reader(revision_unit, revision_number == SRCDIFF_ORIGINAL? srcdiff::operation::DELETE : srcdiff::operation::INSERT);

    srcSAXController controller(srcdiff, "UTF-8");
    controller.parse(&reader);

    return revision_unit;
}
