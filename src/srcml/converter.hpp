// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file converter.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCML_CONVERTER_HPP
#define INCLUDED_SRCML_CONVERTER_HPP

#include <nodes.hpp>

#include <srcml.h>
#include <vector>
#include <map>
#include <mutex>
#include <functional>
#include <memory>

namespace srcML {

class converter {

protected:

  bool split_strings;

  char* output_buffer;
  size_t output_size;
  bool free_buffer;

  static std::mutex mutex;

  static std::map<std::string, std::shared_ptr<srcML::node>> start_tags;
  static std::map<std::string, std::shared_ptr<srcML::node>> end_tags;

private:
    nodes collect_nodes(xmlTextReaderPtr reader) const;
    static std::shared_ptr<srcML::node> get_current_node(xmlTextReaderPtr reader);

public:
    converter(bool split_strings);
    ~converter();

    void set_output_buffer(const std::string& str); 
    void convert(srcml_archive* archive, const std::string& language, void* context, const std::function<ssize_t(void*, void*, size_t)>& read, const std::function<int(void*)>& close); 
    void convert(srcml_unit* unit);
    nodes create_nodes() const;
};


}

#endif
