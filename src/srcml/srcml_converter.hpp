// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcml_converter.hpp
 *
 * @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCML_CONVERTER_HPP
#define INCLUDED_SRCML_CONVERTER_HPP

#include <srcml_nodes.hpp>
#include <srcdiff_options.hpp>

#include <srcml.h>
#include <vector>
#include <map>
#include <mutex>
#include <functional>
#include <memory>

class srcml_converter {

protected:

  srcml_archive * archive;

  bool split_strings;

  int stream_source;

  char * output_buffer;
  size_t output_size;

  static std::mutex mutex;

  static std::map<std::string, std::shared_ptr<srcML::node>> start_tags;
  static std::map<std::string, std::shared_ptr<srcML::node>> end_tags;

private:

	srcml_nodes collect_nodes(xmlTextReaderPtr reader) const;

	static std::shared_ptr<srcML::node> get_current_node(xmlTextReaderPtr reader, bool is_archive);

public:

  struct srcml_burst_config {

    const std::optional<std::string> & output_path;
    const std::string & language;
    const std::optional<std::string> & unit_filename;
    const std::optional<std::string> & unit_version;

  };

 	srcml_converter(srcml_archive * archive, bool split_strings, int stream_source);
 	~srcml_converter();

	void convert(const std::string & language, void * context, const std::function<ssize_t(void *, void *, size_t)> & read, const std::function<int(void *)> & close, const srcml_burst_config & burst_config); 
	srcml_nodes create_nodes() const;

};

#endif
