// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file monokai_theme.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <monokai_theme.hpp>

monokai_theme::monokai_theme(const std::string & highlight_level, bool is_html)
    : theme_t(highlight_level, is_html) {

    if(is_html) {

        background_color  = "rgb(39 , 40, 34)";
        text_color        = "rgb(248 , 248, 242)";
        line_number_color = "grey";

        common_color = "<span style=\"background-color: " + background_color + ";\">";
        delete_color = "<span style=\"color:grey; text-decoration: line-through;\"><span style=\"color: " + text_color + "; background-color: rgba(255, 187 ,187, 0.6); font-weight: bold;\">";
        insert_color = "<span style=\"background-color: rgba(187 , 255, 187, 0.7); font-weight: bold;\">";

        keyword_color = "<span style=\"color: rgb(249, 38, 114);\">";
        storage_color = "<span style=\"color: rgb(166, 226, 46);\">";
        type_color    = "<span style=\"color: rgb(102, 217, 239);\">";

        comment_color = "<span style=\"color: rgb(157, 152, 132);\">";
        number_color  = "<span style=\"color: rgb(174, 129, 255);\">";
        string_color  = "<span style=\"color: rgb(230, 219, 116);\">";

        function_name_color = "<span style=\"color: rgb(166, 226, 46);\">";
        class_name_color    = "<span style=\"color: rgb(166, 226, 46);\">";
        call_name_color     = "<span style=\"color: rgb(102, 217, 239);\">";

    }

}
