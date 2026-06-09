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

#include <srcSAXController.hpp>

#include <libxml/parser.h>

#include <vector>
#include <algorithm>
#include <cstring>

class srcdiff_reader : public srcSAXHandler {
public:

    srcdiff_reader() : srcSAXHandler() {
    }

    virtual void startUnit(const char * localname, const char * prefix, const char * URI,
                           int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                           const struct srcsax_attribute * attributes) {
    }

    virtual void startElement(const char * localname, const char * prefix, const char * URI,
                                int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                                const struct srcsax_attribute * attributes) {
    }

    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {
    }

    virtual void endElement(const char * localname, const char * prefix, const char * URI) {
    }

    virtual void charactersUnit(const char * ch, int len) {
    }

};

#endif
