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

int main(int argc, char * argv[]) {

    srcml_archive* archive = srcml_archive_create();
    srcdiff_config* config = srcdiff_config_create();

    for(int i = 2; i < argc; i += 2) {

        // srcml original file
        const int orginal_file_pos  = i - 1;
        srcml_unit* original_unit = srcml_unit_create(archive);
        srcml_unit_set_filename(unit, argv[orginal_file_pos]);
        srcml_unit_parse_filename(unit, argv[orginal_file_pos]);

        const int modified_file_pos  = i;
        srcml_unit* modified_unit = srcml_unit_create(archive);
        srcml_unit_set_filename(unit, argv[modified_file_pos]);
        srcml_unit_parse_filename(unit, argv[modified_file_pos]);


        // unit is self-contained (namespace on unit)
        srcml_unit* unit = srcdiff_create_delta(original_unit, modified_unit, config);

        srcml_unit_free(original_unit);
        srcml_unit_free(modified_unit);
        srcml_unit_free(unit);
    }

    srcdiff_config_free(config);
    srcml_archive_free(archive);

    return 0;
}
