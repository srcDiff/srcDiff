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

namespace srcML {

    class name_space {

    public:

        name_space(const std::string & uri = std::string(),
                        const std::optional<std::string> & prefix = std::optional<std::string>());
        name_space(const name_space & ns);
        name_space(xmlNsPtr ns);

        std::string get_uri() const;
        std::optional<std::string> get_prefix() const;

        void set_uri(std::string input);
        void set_prefix(std::optional<std::string> input);

        static std::shared_ptr<name_space> get_namespace(xmlNsPtr ns);

        static std::shared_ptr<name_space> SRC_NAMESPACE;
        static std::shared_ptr<name_space> CPP_NAMESPACE;
        static std::shared_ptr<name_space> DIFF_NAMESPACE;
        static std::unordered_map<std::string, std::shared_ptr<name_space>> namespaces;

    private:
    
        std::string uri;
        std::optional<std::string> prefix;

    };
  
}
#endif