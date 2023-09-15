/**
 * @file srcml_namespace.hpp
 *
 * @copyright Copyright (C) 2023-2023 srcML, LLC. (www.srcML.org)
 *
 * srcDiff is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * srcDiff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCLUDED_SRCML_NAMESPACE_HPP
#define INCLUDED_SRCML_NAMESPACE_HPP

#include <srcml.h>

#include <string>
#include <optional>
#include <memory>
#include <map>
#include <unordered_map>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

class srcml_namespace {

public:

  	srcml_namespace(const std::string & uri = std::string(),
                    const std::optional<std::string> & prefix = std::optional<std::string>());
  	srcml_namespace(const srcml_namespace & ns);
  	srcml_namespace(xmlNsPtr ns);

  	std::string get_uri() const;
  	std::optional<std::string> get_prefix() const;

  	void set_uri(std::string input);
  	void set_prefix(std::optional<std::string> input);

    static std::shared_ptr<srcml_namespace> get_namespace(xmlNsPtr ns);

private:
    
    std::string uri;
    std::optional<std::string> prefix;

};

static std::shared_ptr<srcml_namespace> SRC_NAMESPACE = std::make_shared<srcml_namespace>("http://www.srcML.org/srcML/src");
static std::shared_ptr<srcml_namespace> CPP_NAMESPACE = std::make_shared<srcml_namespace>("http://www.srcML.org/srcML/cpp", "cpp");
static std::shared_ptr<srcml_namespace> DIFF_NAMESPACE = std::make_shared<srcml_namespace>("http://www.srcML.org/srcDiff", "diff");
static std::unordered_map<std::string, std::shared_ptr<srcml_namespace>> namespaces = {};
  
#endif