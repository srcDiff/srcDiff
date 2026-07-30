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



    private:
    
        std::string uri;
        std::optional<std::string> prefix;

    };
 
class name_spaces {
public:
        static std::string SRC_URI;
        static std::string DIFF_URI;

        static std::shared_ptr<name_space> SRC_NAMESPACE;
        static name_spaces namespace_registry;

        typedef std::unordered_map<std::string, std::shared_ptr<name_space>>::const_iterator namespaces_citr;

        name_spaces();
        void init(const srcml_archive* archive);

        std::shared_ptr<name_space> get_src_ns();
        std::shared_ptr<name_space> get_diff_ns();

        void update(const std::string& uri, const std::string& prefix);

        std::shared_ptr<name_space> get_namespace(xmlNsPtr ns);

    private:
        std::unordered_map<std::string, std::shared_ptr<name_space>> namespaces;

    };

}
#endif
