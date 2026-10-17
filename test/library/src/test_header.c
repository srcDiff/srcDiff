// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_header.c
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 *
 * Checks that srcdiff.h can be used from C.
 */

#include <srcdiff.h>
#include <srcml.h>

#include <stdio.h>
#include <string.h>

int main(void) {
    const char* original = "int f() {\n  return 0;\n}\n";
    const char* modified = "int f() {\n  return 1;\n}\n";
    char* buffer = NULL;
    size_t size = 0;
    int failed = 0;

    struct srcml_archive* archive = srcml_archive_create();
    srcml_archive_register_namespace(archive, "diff", "http://www.srcML.org/srcDiff");
    srcml_archive_write_open_memory(archive, &buffer, &size);

    struct srcml_unit* original_unit = srcml_unit_create(archive);
    srcml_unit_set_language(original_unit, "C++");
    srcml_unit_parse_memory(original_unit, original, strlen(original));

    struct srcml_unit* modified_unit = srcml_unit_create(archive);
    srcml_unit_set_language(modified_unit, "C++");
    srcml_unit_parse_memory(modified_unit, modified, strlen(modified));

    struct srcdiff_config* config = srcdiff_config_create();
    if(!config) { puts("srcdiff_config_create returned NULL"); failed = 1; }

    struct srcml_unit* delta = srcdiff_create_delta(config, original_unit, modified_unit);
    if(!delta) { puts("srcdiff_create_delta returned NULL"); failed = 1; }

    if(delta) {
        struct srcml_unit* units[3];
        units[0] = srcdiff_read_unit_original(delta);
        units[1] = srcdiff_read_unit_modified(delta);
        units[2] = srcdiff_read_unit_revision(delta, SRCDIFF_ORIGINAL);
        for(int i = 0; i < 3; ++i) {
            if(!units[i]) { printf("read %d returned NULL\n", i); failed = 1; }
            else srcml_unit_free(units[i]);
        }
        srcml_unit_free(delta);
    }

    int (*convenience)(const char*, const char*, const char*) = srcDiff;
    if(!convenience) failed = 1;

    srcdiff_config_free(config);
    srcml_unit_free(modified_unit);
    srcml_unit_free(original_unit);
    srcml_archive_close(archive);
    srcml_archive_free(archive);
    srcml_memory_free(buffer);

    puts(failed ? "FAILED" : "passed");
    return failed;
}
