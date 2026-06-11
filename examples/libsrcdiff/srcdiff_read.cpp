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

    srcml_archive_read_open_filename(archive, "srcdiff.xml");

    srcml_unit* unit = srcml_archive_read_unit(archive);
    srcml_unit* original_unit = srcdiff_read_unit_revision(unit, SRCDIFF_REVISION_ORIGINAL);

    std::cout << srcml_unit_get_srcml(original_unit) << '\n';

    srcml_unit_free(original_unit);
    srcml_unit_free(unit);
    srcml_archive_free(archive);

    return 0;
}
