// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_create.cpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 *
 * Show basic libsrcdiff usage to create an archive.
 */

#include <iostream>
#include <srcml.h>
#include <srcdiff.h>

int main(int argc, char * argv[]) {

    srcml_archive* archive = srcml_archive_create();
    if(argc < 4) {
        srcml_archive_enable_solitary_unit(archive);
    }

    srcml_archive_write_open_filename(archive, "srcdiff.xml");
    srcdiff_config* config = srcdiff_config_create();

    for(int i = 2; i < argc; i += 2) {

        // srcml original file
        const int original_file_pos  = i - 1;

        srcml_unit* original_unit = srcml_unit_create(archive);
        srcml_unit_set_filename(original_unit, argv[original_file_pos]);
        srcml_unit_set_language(original_unit, srcml_check_extension(argv[original_file_pos]));
        srcml_unit_parse_filename(original_unit, argv[original_file_pos]);

        const int modified_file_pos  = i;
        srcml_unit* modified_unit = srcml_unit_create(archive);
        srcml_unit_set_filename(modified_unit, argv[modified_file_pos]);
        srcml_unit_set_language(modified_unit, srcml_check_extension(argv[modified_file_pos]));
        srcml_unit_parse_filename(modified_unit, argv[modified_file_pos]);

        srcml_unit* unit = srcdiff_create_delta(original_unit, modified_unit, config);
        srcml_archive_write_unit(archive, unit);

        srcml_unit_free(original_unit);
        srcml_unit_free(modified_unit);
        srcml_unit_free(unit);
    }

    srcdiff_config_free(config);
    srcml_archive_free(archive);

    return 0;
}
