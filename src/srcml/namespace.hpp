// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file namespace.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
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
        static std::shared_ptr<name_space> POS_NAMESPACE;
        static std::shared_ptr<name_space> DIFF_NAMESPACE;
        static std::unordered_map<std::string, std::shared_ptr<name_space>> namespaces;

    private:
    
        std::string uri;
        std::optional<std::string> prefix;

    };
  
}
#endif
