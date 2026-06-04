// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcml_converter.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCML_CONVERTER_HPP
#define INCLUDED_SRCML_CONVERTER_HPP

#include <srcml_nodes.hpp>

#include <srcml.h>
#include <vector>
#include <map>
#include <mutex>
#include <functional>
#include <memory>

class srcml_converter {

protected:

  bool split_strings;

  char* output_buffer;
  size_t output_size;
  bool free_buffer;

  static std::mutex mutex;

  static std::map<std::string, std::shared_ptr<srcML::node>> start_tags;
  static std::map<std::string, std::shared_ptr<srcML::node>> end_tags;

private:

    srcml_nodes collect_nodes(xmlTextReaderPtr reader) const;
    static std::shared_ptr<srcML::node> get_current_node(xmlTextReaderPtr reader);

public:
    srcml_converter(bool split_strings);
    ~srcml_converter();

    void set_output_buffer(const std::string& str); 
    void convert(srcml_archive* archive, const std::string& language, void* context, const std::function<ssize_t(void*, void*, size_t)>& read, const std::function<int(void*)>& close); 
    void convert(srcml_unit* unit);
    srcml_nodes create_nodes() const;
};

#endif
