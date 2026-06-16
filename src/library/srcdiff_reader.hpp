// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_reader.hpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_READER_HPP
#define INCLUDED_SRCDIFF_READER_HPP

#include <srcSAXHandler.hpp>
#include <srcSAXController.hpp>
#include <libxml/parser.h>

#include <srcml.h>

#include <operation.hpp>

#include <stack>


#include <string_view>
using namespace ::std::literals::string_view_literals;


class srcdiff_reader : public srcSAXHandler {
public:

    srcdiff_reader(srcml_unit* unit, srcdiff::operation operation) : srcSAXHandler(),
        unit(unit), operation(operation), op_mode() {
    }

    virtual void startUnit(const char* localname, const char* prefix, const char* URI,
                           int num_namespaces, const struct srcsax_namespace* namespaces,
                           int num_attributes, const struct srcsax_attribute* attributes) {
        op_mode.push(srcdiff::operation::COMMON);
        register_namespaces(num_namespaces, namespaces);
        update_unit_attributes(num_attributes, attributes);

        srcml_write_start_unit(unit);
        write_attributes(num_attributes, attributes);
    }

    virtual void startElement(const char* localname, const char* prefix, const char* URI,
                                int num_namespaces, const struct srcsax_namespace* namespaces, int num_attributes,
                                const struct srcsax_attribute* attributes) {
        if(URI == srcdiff::SRCDIFF_DEFAULT_NAMESPACE_HREF) {
            if(localname != "ws"sv) {
                op_mode.push(srcdiff::string_to_operation(localname));
            }
            return;
        }

        if(!in_operation()) return;

        srcml_write_start_element(unit, prefix, localname, URI);
        write_attributes(num_attributes, attributes);
    }

    virtual void endUnit(const char* localname, const char* prefix, const char* URI) {
        srcml_write_end_unit(unit);
    }

    virtual void endElement(const char* localname, const char* prefix, const char* URI) {
        if(URI == srcdiff::SRCDIFF_DEFAULT_NAMESPACE_HREF) {
            if(localname != "ws"sv) {
                op_mode.pop();
            }
            return;            
        }

        if(!in_operation()) return;

        srcml_write_end_element(unit);
    }

    virtual void charactersUnit(const char* ch, int len) {
        if(!in_operation()) return; 

        std::string text;
        text.append(ch, len) ;
        srcml_write_string(unit, text.c_str());
    }

    void update_unit_attributes(int num_attributes, const srcsax_attribute* attributes) {

        for(int attr_pos = 0; attr_pos < num_attributes; ++attr_pos) {
            const srcsax_attribute& attr = attributes[attr_pos];

            std::string_view attribute = attr.localname;
            if (attribute == "timestamp"sv)
                srcml_unit_set_timestamp(unit, attr.value);
            else if (attribute == "hash"sv)
                srcml_unit_set_hash(unit, attr.value);
            else if (attribute == "language"sv)
                srcml_unit_set_language(unit, attr.value);
            else if (attribute == "revision"sv)
                ; /** @todo */
            else if (attribute == "filename"sv)
                srcml_unit_set_filename(unit, attr.value);
            else if (attribute == "url"sv)
               ;
            else if (attribute == "version"sv)
                srcml_unit_set_version(unit, attr.value);
            else if (attribute == "tabs"sv || attribute == "options"sv)
                ;
            else {
                // add custom attribute
                srcml_unit_add_attribute(unit, attr.prefix, attr.localname, attr.value);
            }
        }
    }

    void register_namespaces(int num_namespaces, const srcsax_namespace* namespaces) {

        for (int ns_pos = 0; ns_pos < num_namespaces; ++ns_pos) {
            const srcsax_namespace& ns = namespaces[ns_pos];
            srcml_unit_register_namespace(unit, ns.prefix, ns.uri);
        }

    }

    void write_attributes(int num_attributes, const struct srcsax_attribute* attributes) {
        for(int attr_pos = 0; attr_pos < num_attributes; ++attr_pos) {
            const srcsax_attribute& attr = attributes[attr_pos];
            srcml_write_attribute(unit, attr.prefix, attr.localname, 0/*URI?*/, attr.value);
      } 
    }

    bool in_operation() const {
        if(op_mode.top() == srcdiff::operation::COMMON) return true;
        return operation == op_mode.top();
    }

private:
    srcml_unit* unit;
    srcdiff::operation operation;
    std::stack<srcdiff::operation> op_mode;
};

#endif
