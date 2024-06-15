// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file attribute.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCML_NODE_HPP
#define INCLUDED_SRCML_NODE_HPP
#include <srcml.h>
#include <namespace.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <optional>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

namespace srcML {

    class attribute {

    public:

        attribute(xmlAttrPtr attribute);
        attribute(const std::string & name = std::string(),
                        std::shared_ptr<srcML::name_space> ns = srcML::name_space::SRC_NAMESPACE,
                        std::optional<std::string> value = std::optional<std::string>());

        void merge(const attribute & that);
        std::string full_name() const;
        void set_value(const std::optional<std::string> & input);
        const std::string & get_name() const;
        std::shared_ptr<srcML::name_space> get_ns() const;
        const std::optional<std::string> & get_value() const;

        friend std::ostream & operator<<(std::ostream & out, const attribute & that);
        bool operator==(const attribute & that) const;
        bool operator!=(const attribute & that) const;

    private:
	
        std::string name;
        std::optional<std::string> value;
        std::shared_ptr<srcML::name_space> ns;

    };

    std::ostream & operator<<(std::ostream & out, const attribute & that);
}

#endif
