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
#include <constants.hpp>

#include <client_options.hpp>

#include <string>
#include <optional>
#include <filesystem>
#include <memory>

static bool srcdiff_parse_file(srcml_archive* archive, const char* path, const char* language, srcml_unit*& unit) {
    unit = nullptr;
    if(!path) return true;

    std::error_code error;
    if(!std::filesystem::is_regular_file(path, error)) return false;

    unit = srcml_unit_create(archive);
    srcml_unit_set_language(unit, language);
    srcml_unit_set_filename(unit, path);
    if(srcml_unit_parse_filename(unit, path) != SRCML_STATUS_OK) {
        srcml_unit_free(unit);
        unit = nullptr;
        return false;
    }

    return true;
}

int srcDiff(const char* original_filename, const char* modified_filename, const char* output_filename){
    if(original_filename == nullptr && modified_filename == nullptr) return SRCDIFF_STATUS_ERROR;
    if(output_filename == nullptr) return SRCDIFF_STATUS_ERROR;

    const char* path = original_filename ? original_filename : modified_filename;
    const char* language = srcml_check_extension(path);
    if(language == SRCML_LANGUAGE_NONE) return SRCDIFF_STATUS_ERROR;

    srcml_archive* archive = srcml_archive_create();
    srcml_archive_enable_solitary_unit(archive);
    srcml_archive_disable_hash(archive);
    srcml_archive_register_namespace(archive,
        srcdiff::SRCDIFF_DEFAULT_NAMESPACE_PREFIX.c_str(),
        srcdiff::SRCDIFF_DEFAULT_NAMESPACE_HREF.c_str()
    );

    srcml_unit* original_unit = nullptr;
    srcml_unit* modified_unit = nullptr;
    srcml_unit* delta = nullptr;
    int status = SRCDIFF_STATUS_ERROR;

    if(srcdiff_parse_file(archive, original_filename, language, original_unit)
       && srcdiff_parse_file(archive, modified_filename, language, modified_unit)
       && (delta = srcdiff_create_delta(nullptr, original_unit, modified_unit))
       && srcml_archive_write_open_filename(archive, output_filename) == SRCML_STATUS_OK) {

        if(srcml_archive_write_unit(archive, delta) == SRCML_STATUS_OK) status = SRCDIFF_STATUS_OK;
        srcml_archive_close(archive);
    }

    if(delta)         srcml_unit_free(delta);
    if(modified_unit) srcml_unit_free(modified_unit);
    if(original_unit) srcml_unit_free(original_unit);
    srcml_archive_free(archive);

    return status;
}

struct srcdiff_config* srcdiff_config_create() {
  srcdiff_config* config = new srcdiff_config();

  config->options = srcdiff::OPTION_STRING_SPLITTING;

  config->manager = std::make_unique<srcdiff::input_stream_manager>(config->options);
  config->deltor  = std::make_unique<srcdiff::deltor>(srcdiff::output_options());
  return config;
}

void srcdiff_config_free(struct srcdiff_config* config) {
  delete config;
}

class srcml_unit_input : public srcdiff::abstract_input_stream {
public:
  srcml_unit_input(srcml_unit* unit) 
    : unit(unit){
  }

  virtual void operator()(srcML::converter& converter, srcML::nodes& nodes) const {
    nodes.clear();
    if(!unit) return;

    converter.convert(unit);
    nodes = converter.create_nodes();
  }

private:
  srcml_unit* unit;
};

class empty_unit_input : public srcdiff::abstract_input_stream {
public:
  empty_unit_input(const char* language)
    : language(language ? language : "") {
  }

  virtual void operator()(srcML::converter& converter [[maybe_unused]], srcML::nodes& nodes) const {
    nodes.clear();
    std::shared_ptr<srcML::node> start_unit = std::make_shared<srcML::node>(srcML::node_type::START, "unit");
    start_unit->emplace_attribute("language", srcML::attribute("language", srcML::name_spaces::SRC_NAMESPACE, language));
    nodes.push_back(start_unit);
    nodes.push_back(std::make_shared<srcML::node>(srcML::node_type::END, "unit"));
  }

private:
  std::string language;
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

struct srcml_unit* srcdiff_create_delta(struct srcdiff_config* configuration,
                                        struct srcml_unit    * original_unit, 
                                        struct srcml_unit    * modified_unit) {
  if(!original_unit && !modified_unit) return nullptr;

  if(original_unit && !srcml_unit_get_srcml(original_unit)) return nullptr;
  if(modified_unit && !srcml_unit_get_srcml(modified_unit)) return nullptr;

  std::unique_ptr<srcdiff_config, srcdiff_config_deleter> config = std::unique_ptr<srcdiff_config, srcdiff_config_deleter>(configuration, srcdiff_config_deleter());
  if(!config) {
    config = std::unique_ptr<srcdiff_config, srcdiff_config_deleter>(srcdiff_config_create(), srcdiff_config_deleter(true));
  }

  const char* language = original_unit? srcml_unit_get_language(original_unit) : srcml_unit_get_language(modified_unit);

  std::shared_ptr<srcdiff::abstract_input_stream> original_input = original_unit
    ? std::shared_ptr<srcdiff::abstract_input_stream>(std::make_shared<srcml_unit_input>(original_unit))
    : std::make_shared<empty_unit_input>(language);
  config->manager->append_original_stream(original_input);

  std::shared_ptr<srcdiff::abstract_input_stream> modified_input = modified_unit
    ? std::shared_ptr<srcdiff::abstract_input_stream>(std::make_shared<srcml_unit_input>(modified_unit))
    : std::make_shared<empty_unit_input>(language);
  config->manager->append_modified_stream(modified_input);

  srcml_archive* archive = original_unit? srcml_unit_get_archive(original_unit) : srcml_unit_get_archive(modified_unit);
  return config->deltor->create(archive, *config->manager);
}

struct srcml_unit* srcdiff_read_unit_original(struct srcml_unit* unit) {
    return srcdiff_read_unit_revision(unit, SRCDIFF_ORIGINAL);
}

struct srcml_unit* srcdiff_read_unit_modified(struct srcml_unit* unit) {
    return srcdiff_read_unit_revision(unit, SRCDIFF_MODIFIED);
}

struct srcml_unit* srcdiff_read_unit_revision(struct srcml_unit* unit, size_t revision_number) {
    if(!unit) return nullptr;
    if(revision_number != SRCDIFF_ORIGINAL && revision_number != SRCDIFF_MODIFIED) return nullptr;

    const char* srcml = srcml_unit_get_srcml(unit);
    if(!srcml || !*srcml) return nullptr;

    std::string srcdiff = srcml;

    srcml_unit* revision_unit = srcml_unit_create(srcml_unit_get_archive(unit));
    srcdiff::reader reader(revision_unit, revision_number == SRCDIFF_ORIGINAL? srcdiff::operation::DELETE : srcdiff::operation::INSERT);

    srcSAXController controller(srcdiff, "UTF-8");
    controller.parse(&reader);

    return revision_unit;
}
