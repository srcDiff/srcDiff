/**
 * @file srcml_namespace.cpp
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

#include <srcml.h>
#include <srcml_namespace.hpp>
#include <memory>
#include <string>
#include <iostream>
#include <optional>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

class srcml_attribute {

public:

    srcml_attribute(xmlAttrPtr attribute);
    srcml_attribute(const std::string & name = std::string(),
                    std::shared_ptr<srcml_namespace> ns = SRC_NAMESPACE,
                    std::optional<std::string> value = std::optional<std::string>());

    std::string full_name() const;
    void set_name(std::string input);
    void set_value(std::optional<std::string> input);
    const std::string & get_name() const;
    std::shared_ptr<srcml_namespace> get_ns() const;
    std::optional<std::string> get_value() const;


    friend std::ostream & operator<<(std::ostream & out, const srcml_attribute & that);
    bool operator==(const srcml_attribute & that) const;
    bool operator!=(const srcml_attribute & that) const;

private:
	
    std::string name;
    std::optional<std::string> value;
    std::shared_ptr<srcml_namespace> ns;

  };