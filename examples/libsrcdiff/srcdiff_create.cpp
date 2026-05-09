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

    srcdiff_archive* archive = srcdiff_archive_create(/** optional srcml_archive?*/);

    for(int i = 2; i < argc; i += 2) {

        // Could create your own archive as an alternative
        srcml_archive* srcml_arch = srcml_archive_create();
        // configuration
 
        // srcml original file
        const int orginal_file_pos  = i - 1;
        srcml_unit* original_unit = srcml_unit_create(srcdiff_get_srcml_archive(archive)/*srcml_arc*/);
        srcml_unit_set_filename(unit, argv[orginal_file_pos]);
        srcml_unit_parse_filename(unit, argv[orginal_file_pos]);

        const int modified_file_pos  = i;
        srcml_unit* modified_unit = srcml_unit_create(srcdiff_get_srcml_archive(archive)/*srcml_arc*/);
        srcml_unit_set_filename(unit, argv[modified_file_pos]);
        srcml_unit_parse_filename(unit, argv[modified_file_pos]);

        // Do I need a create? Or, does the srcml_unit handle everything I need
        // I can see some minor settings, but not maybe the normal.
        //srcdiff_unit* unit = srcdiff_create_unit(archive);

        // Something besides create/calculate/compute/find/delta?
        srcdiff_unit* unit = srcdiff_create_delta(original_unit, modified_unit);
        srcdiff_unit_create_delta(unit, original_unit, modified_unit);

        srcdiff_archive_write_unit(archive, unit);

        srcml_unit_free(original_unit);
        srcml_unit_free(modified_unit);
        srcdiff_unit_free(unit);
    }

    return 0;
}
