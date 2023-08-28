/**
 * @file call.cpp
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

#include <call.hpp>

void call::collect_names() {
    if(names.size()) return;

    std::shared_ptr<const construct> call_name = find_child("name");
    for(std::shared_ptr<const construct> child : call_name.children()) {
        if(child.is_text() && !child->is_white_space()) {
            names.push_back(*child->content);
        } else if(child.root_term_name("name")) {
            names.push_back(child->to_string());
        }
    }
}

bool call::is_matchable_impl(const construct & modified) const {
    collect_names();
    const construct & modified_call = static_cast<const construct &>(modified);
    modified_call.collect_names();

    srcdiff_shortest_edit_script ses;
    ses.compute_edit_script(names, modified_call.names);

    edit_t * edits = ses.script();

    int similarity = 0;

    int delete_similarity = 0;
    int insert_similarity = 0;
    for(; edits; edits = edits->next) {

    switch(edits->operation) {

      case SES_DELETE :

        delete_similarity += edits->length;
        break;

      case SES_INSERT :

        insert_similarity += edits->length;
        break;

      }

    }

    delete_similarity = name_list_original.size() - delete_similarity;
    insert_similarity = name_list_modified.size() - insert_similarity;

    similarity = std::min(delete_similarity, insert_similarity);

    if(similarity < 0) similarity = 0;

    return false;
}