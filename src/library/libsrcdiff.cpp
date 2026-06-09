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

#include <client_options.hpp>

#include <string>
#include <optional>
#include <fstream>
#include <memory>

#include <string_view>
using namespace ::std::literals::string_view_literals;

#include <libxml/parserInternals.h>

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

    return 0;
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

struct srcml_unit* srcdiff_create_delta(struct srcdiff_config* configuration,
                                        struct srcml_unit    * original_unit, 
                                        struct srcml_unit    * modified_unit) {
  if(!original_unit) return nullptr;
  if(!modified_unit) return nullptr;

  std::unique_ptr<srcdiff_config, srcdiff_config_deleter> config = std::unique_ptr<srcdiff_config, srcdiff_config_deleter>(configuration, srcdiff_config_deleter());
  if(!config) {
    config = std::unique_ptr<srcdiff_config, srcdiff_config_deleter>(srcdiff_config_create(), srcdiff_config_deleter(true));
  }

  srcml_unit_input original_input(original_unit);
  config->manager->append_stream(original_input);

  srcml_unit_input modified_input(modified_unit);
  config->manager->append_stream(modified_input);

  return config->deltor->create(srcml_unit_get_archive(original_unit), *config->manager, srcml_unit_get_language(original_unit), std::optional<std::string>(), std::optional<std::string>());
}

std::string extract_revision(const char* srcml, int size, int revision, bool text_only) {

    std::string_view DIFF_PREFIX = "diff:"sv;

    std::stack<srcdiff::operation> mode;
    mode.push(srcdiff::COMMON);

    std::string news;
    const char* p = srcml;
    const char* lastp = p;
    while ((p = (const char*) memchr(p, '<', static_cast<size_t>(size - (p - srcml))))) {

        bool inmode = mode.top() == srcdiff::COMMON || (revision == 0 && mode.top() == srcdiff::DELETE) || (revision == 1 && mode.top() == srcdiff::INSERT);

        // output previous non-tag text
        if (inmode) {
            news.append(lastp, static_cast<size_t>(p - lastp));
        }

        auto sp = p;

        // skip to end of tag
        p = (const char*) memchr(p, '>', static_cast<size_t>(size - (p - srcml)));
        ++p;

        if (strncmp(sp + 1, DIFF_PREFIX.data(), DIFF_PREFIX.size()) == 0) {

            const char* tstart = sp + 1 + DIFF_PREFIX.size();

            if (strncmp(tstart, "delete", 6) == 0) {
                mode.push(srcdiff::DELETE);
            } else if (strncmp(tstart, "insert", 6) == 0) {
                mode.push(srcdiff::INSERT);
            } else if (strncmp(tstart, "ws", 2) != 0) {
                mode.push(srcdiff::COMMON);
            }

        }
        else if (*(sp + 1) == '/' && strncmp(sp + 2, DIFF_PREFIX.data(), DIFF_PREFIX.size()) == 0) {
            if(strncmp(sp + 2 + DIFF_PREFIX.size(), "ws", 2) != 0) {
                mode.pop();
            }
        }
        else {
            if (inmode && !text_only) {
                news.append(sp, static_cast<size_t>(p - sp));
            }
        }

        lastp = p;
    }

    bool inmode = mode.top() == srcdiff::COMMON || (revision == 0 && mode.top() == srcdiff::DELETE) || (revision == 1 && mode.top() == srcdiff::INSERT);
    auto remaining_size = (size_t) size - (size_t) (lastp - srcml);
    if (inmode && remaining_size > 0) {
        news.append(lastp, remaining_size);
    }

    return news;
}

struct extract_context {
    std::string src;
    srcdiff::operation operation;
    std::stack<srcdiff::operation> mode;  
};

// Extract source code from srcml
std::string extract_src(std::string_view srcml, srcdiff::operation operation) {

    extract_context context;
    context.operation = operation;
    context.mode.push(srcdiff::COMMON);

    // parse the srcml collecting the (now needed) src
    xmlSAXHandler charactersax;
    memset(&charactersax, 0, sizeof(charactersax));
    charactersax.initialized    = XML_SAX2_MAGIC;

    charactersax.ignorableWhitespace = charactersax.characters = [](void* ctx, const xmlChar* ch, int len) {

        auto ctxt = (xmlParserCtxtPtr) ctx;
        if (ctxt == nullptr)
            return;
        auto context = (extract_context*) ctxt->_private;
        if (context == nullptr)
            return;

        context->src.append((const char*) ch, static_cast<size_t>(len));
    };

    charactersax.startElementNs = [](void* ctx, const xmlChar* localname, const xmlChar* /* prefix */, const xmlChar* URI,
                     int /* nb_namespaces */, const xmlChar** /* namespaces */,
                     int /* nb_attributes */, int /* nb_defaulted */, const xmlChar** attributes) {

        auto ctxt = (xmlParserCtxtPtr) ctx;
        if (ctxt == nullptr)
            return;
        auto context = (extract_context*) ctxt->_private;
        if (context == nullptr)
            return;

        if ("escape"sv == (const char*) localname && "http://www.srcML.org/srcML/src"sv == (const char*) URI) {
            std::string svalue((const char *)attributes[0 * 5 + 3], static_cast<std::size_t>(attributes[0 * 5 + 4] - attributes[0 * 5 + 3]));

            // use strtol() instead of atoi() since strtol() understands hex encoding of '0x0?'
            char value = (char)strtol(svalue.data(), NULL, 0);

            context->src.append(1, value);

        } else if (context->operation && srcML::name_space::DIFF_NAMESPACE->get_uri() == (const char*) URI) {

            if ((const char*) localname == "srcdiff::INSERT"sv)
                context->mode.push(srcdiff::INSERT);
            else if ((const char*) localname == "srcdiff::DELETE"sv)
                context->mode.push(srcdiff::DELETE);
            else
                context->mode.push(srcdiff::COMMON);
        }
    };

    xmlParserCtxtPtr xml_context = xmlCreateMemoryParserCtxt(srcml.data(), (int) srcml.size());
    auto save_private = xml_context->_private;
    xml_context->_private = &context;
    auto save_sax = xml_context->sax;
    xml_context->sax = &charactersax;

    xmlParseDocument(xml_context);

    xml_context->_private = save_private;
    xml_context->sax = save_sax;

    xmlFreeParserCtxt(xml_context);

    return context.src;
}

std::string_view attribute_revision(std::string_view attribute, int revision) {

    auto pos = attribute.find('|');
    if (pos == std::string::npos)
        return attribute;

    if (revision == SRCDIFF_REVISION_ORIGINAL)
        return attribute.substr(0, pos);

    return attribute.substr(pos + 1);
}
