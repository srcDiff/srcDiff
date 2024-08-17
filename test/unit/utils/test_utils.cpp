// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_utils.cpp
 *
 * @copyright Copyright (C) 2021-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <test_utils.hpp>

#include <construct_factory.hpp>

#include <fstream>
#include <string>
#include <sstream>
#include <srcml_converter.hpp>
#include <iostream>
#include <cstdio>
#include <cassert>

ssize_t str_read(void * context, void * buffer, size_t len) {

    std::string * ctx = static_cast<std::string *>(context);
    size_t num_read = ctx->copy((char*)buffer, len, 0);
    ctx->erase(0, num_read);
    return num_read;
}

int str_close(void * context) {
    return 0;
}


std::string read_from_file(std::string filename){

    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    return str;
}

std::shared_ptr<srcml_nodes> create_nodes(const std::string & code, const std::string & language) {

    //create srcml archive pointer and get code string      
    srcml_archive * archive = srcml_archive_create();
    srcml_archive_enable_solitary_unit(archive);
    srcml_archive_disable_hash(archive);
    srcml_archive_register_namespace(archive, "diff", "http://www.srcML.org/srcDiff");

    //Create burst_config object
    const srcml_converter::srcml_burst_config burst_config = {
            std::optional<std::string>(),
            "",
            std::optional<std::string>(),
            std::optional<std::string>()
        };

    //create srcml_nodes
    srcml_converter contNodes(archive, true, 0);
    std::string source = code;
    contNodes.convert(language, (void*)&source, &str_read, &str_close, burst_config);
    srcml_nodes testNode = contNodes.create_nodes();

    return std::make_shared<srcml_nodes>(testNode);
}

std::shared_ptr<const construct> create_test_construct_inner(std::shared_ptr<construct> parent, size_t pos ) {

    for (std::shared_ptr<const construct> const_child : parent->children()) {
        std::shared_ptr<construct> child = std::const_pointer_cast<construct>(const_child);
        if (child->start_position() == pos) {
            return child;
        }
        if (child->start_position() < pos && pos < child->end_position()) {
            std::shared_ptr<const construct> found_child = create_test_construct_inner(child, pos);
            assert(found_child);
            return found_child;
        }
    }
    assert(false);
    return std::shared_ptr<const construct>();
}

construct_test_data create_test_construct(const std::string & code, const std::string & construct_name, const std::string & language) {

    std::shared_ptr<srcml_nodes> nodes = create_nodes(code, language);

    for (size_t i = 0; i < nodes->size(); ++i) {
        if ((*nodes)[i]->get_name() == construct_name) {
            std::shared_ptr<unit> root_unit = std::make_shared<unit>(*nodes, std::shared_ptr<srcdiff_output>());
            std::shared_ptr<const construct> found_construct = create_test_construct_inner(root_unit, i);
            return {nodes, found_construct};
        }
    }
  return {nodes, std::shared_ptr<construct>() };
}
