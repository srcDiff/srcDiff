// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_correction.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <edit_corrector.hpp>

#include <list>
#include <vector>
#include <algorithm>
#include <optional>

#include <iostream>

namespace srcdiff {

edit_corrector::edit_corrector(const construct::construct_list_view sets_original,
                               const construct::construct_list_view sets_modified,
                               ses::edit_list& edits) 
    : sets_original(sets_original),
      sets_modified(sets_modified),
      edits(edits) {}

void edit_corrector::split_change(struct ses::edit subject_edits,
                                  int original_pos, int modified_pos,
                                  ses::edit_iterator& start_edits,
                                  ses::edit_iterator& last_edits) {

    int sequence_one_offset = subject_edits.original_offset;
    int sequence_two_offset = subject_edits.modified_offset;
    int original_length = subject_edits.original_length;
    int modified_length = subject_edits.modified_length;

    std::optional<struct ses::edit> left_edit;
    if(original_pos || modified_pos) {
        left_edit  = ses::edit(ses::CHANGE);
    }

    bool is_delete = original_length - original_pos - 1;
    bool is_insert = modified_length - modified_pos - 1;

    std::optional<struct ses::edit> right_edit;
    if(is_delete || is_insert) {
        right_edit = ses::edit(ses::CHANGE);
    }

    struct ses::edit common_edit(ses::COMMON,
                                 sequence_one_offset + original_pos,
                                 1,
                                 sequence_two_offset + modified_pos,
                                 1
    );

    if(left_edit) {

        left_edit->operation = original_pos && modified_pos ? ses::CHANGE : (original_pos ? ses::DELETE : ses::INSERT);
        left_edit->original_offset = sequence_one_offset;
        left_edit->original_length = original_pos;

        left_edit->modified_offset = sequence_two_offset;
        left_edit->modified_length = modified_pos;
    }

    if(right_edit) {

        right_edit->operation = is_delete && is_insert ? ses::CHANGE : (is_delete ? ses::DELETE : ses::INSERT);
        right_edit->original_offset = common_edit.original_offset + 1;
        right_edit->original_length = original_length - original_pos - 1;

        right_edit->modified_offset = common_edit.modified_offset + 1;
        right_edit->modified_length = modified_length - modified_pos - 1;
    }

    std::vector<ses::edit_iterator> inserted_edits;
    if(left_edit) {
        inserted_edits.push_back(edits.insert(start_edits, *left_edit));
    }
    
    inserted_edits.push_back(edits.insert(start_edits, common_edit));

    if(right_edit) {
        inserted_edits.push_back(edits.insert(start_edits, *right_edit));
    }

    start_edits = inserted_edits.front();
    last_edits  = inserted_edits.back();

}

ses::edit_iterator edit_corrector::correct_common_inner(ses::edit_iterator change_edit) {

    for(std::size_t i = 0; i < change_edit->original_length; ++i) {

        for(std::size_t j = 0; j < change_edit->modified_length; ++j) {

            std::size_t original_set_pos = change_edit->original_offset + i;
            std::size_t modified_set_pos = change_edit->modified_offset + j;

            std::shared_ptr<const construct> set_original = sets_original[original_set_pos];
            std::shared_ptr<const construct> set_modified = sets_modified[modified_set_pos];

            if(*set_original != *set_modified) {
                continue;
            }

            ses::edit_iterator start_edits = change_edit;
            ses::edit_iterator last_edits;
            split_change(*change_edit,
                         i,
                         j,
                         change_edit, 
                         last_edits);
            edits.erase(start_edits);
            return last_edits;

        }

    }

    return change_edit;

}

ses::edit_iterator edit_corrector::correct_common(ses::edit_iterator start_edit) {

    ses::edit_iterator current = start_edit;

    if(current->operation == ses::CHANGE) {
        current = ++correct_common_inner(current);
    }
    else {
        ++current;
    }

    ++current;

    if(current->operation == ses::CHANGE) {
        current = correct_common_inner(current);
    }
    else if(current != edits.end()) {
        ++current;
    }

    return current;

}

std::shared_ptr<text_measurer> edit_corrector::edit2measure(int original_offset, int modified_offset) {

    std::size_t original_set_pos = original_offset;
    std::size_t modified_set_pos = modified_offset;

    std::shared_ptr<const construct> set_original = sets_original[original_set_pos];
    std::shared_ptr<const construct> set_modified = sets_modified[modified_set_pos];

    const std::string & original_tag = set_original->term(0)->get_name();
    const std::string & modified_tag = set_modified->term(0)->get_name();

    const std::string & original_uri = set_original->term(0)->get_namespace()->get_uri();
    const std::string & modified_uri = set_modified->term(0)->get_namespace()->get_uri();

    if(!(original_tag == modified_tag && original_uri == modified_uri)
        && !set_original->is_tag_convertable(*set_modified))
        return std::shared_ptr<text_measurer>();

    std::shared_ptr<text_measurer> measure = std::make_shared<text_measurer>(*set_original, *set_modified);
    measure->compute();

    return measure;

}

void edit_corrector::correct() {

    // wrongly matched common correction
    for(ses::edit_iterator edit = edits.begin(); edit != edits.end(); ++edit) {

        bool is_change_before = edit->operation == ses::CHANGE;

        // guard checks for first edit
        if(edit->operation == ses::COMMON) continue;
        if(edit->original_length > 3 || edit->modified_length > 3) continue;

        // guard checks for next edit
        ses::edit_iterator next = std::next(edit);
        if(next == edits.end()) continue;
        if(next->operation == ses::COMMON) continue;
        if(next->original_length > 3 || next->modified_length > 3) continue;

        bool is_change_after = next->operation == ses::CHANGE;

        if(    !is_change_before
            && !is_change_after
            && edit->operation == next->operation) continue;

        // start edit should be edit_script here (check later)
        int start_offset = edit->original_offset;
        if(    edit->operation == ses::DELETE
            || edit->operation == ses::CHANGE) {
            start_offset += edit->original_length;
        }

        int common_length = next->original_offset - start_offset;
        if(common_length != 1) continue;

        struct ses::edit subject_edits(*edit);
        subject_edits.operation = ses::CHANGE;

        if(edit->operation == ses::DELETE) {
            subject_edits.modified_offset = next->modified_offset;
            subject_edits.modified_length = 0;
        } else if(edit->operation == ses::INSERT) {
            subject_edits.original_offset = next->original_offset;
            subject_edits.original_length = 0;
        }

        ++subject_edits.original_length;
        ++subject_edits.modified_length;

        std::size_t original_offset = 0;
        std::size_t modified_offset = 0;

        if(is_change_before) {

            original_offset = subject_edits.original_length - 1;
            modified_offset = subject_edits.modified_length - 1;

            subject_edits.original_length += next->original_length;
            subject_edits.modified_length += next->modified_length;

        } else {

            if(edit->operation == ses::DELETE) {

                original_offset = subject_edits.original_length - 1;
                modified_offset = 0;

                --subject_edits.modified_offset;

                subject_edits.original_length += next->original_length;  
                subject_edits.modified_length += next->modified_length;  

            } else {

                original_offset = 0;
                modified_offset = subject_edits.modified_length - 1;

                --subject_edits.original_offset;

                subject_edits.original_length += next->original_length;  
                subject_edits.modified_length += next->modified_length;
            }

        }

        std::size_t common_pos = subject_edits.original_offset + original_offset;

        std::shared_ptr<const construct> common_set = sets_original[common_pos];
        std::shared_ptr<construct> common_set_text(std::make_shared<construct>(common_set->nodes()));
        text_measurer::collect_text_element(*common_set, *common_set_text);

        std::vector<std::size_t> original_similarities(subject_edits.original_length);
        std::vector<std::size_t> modified_similarities(subject_edits.modified_length);

        for(std::size_t i = 0; i < subject_edits.original_length; ++i) {

            if(i == original_offset) continue;

            for(std::size_t j = 0; j < subject_edits.modified_length; ++j) {

                if(j == modified_offset) continue;

                std::shared_ptr<text_measurer> measure 
                    = edit_corrector::edit2measure(subject_edits.original_offset + i,
                                                   subject_edits.modified_offset + j);               
                if(!measure) continue;

                // does not need broken if either of these are true
                if(i < original_offset && j < modified_offset) {
                    original_similarities[i] = std::max(original_similarities[i], std::size_t(measure->similarity()));
                    modified_similarities[j] = std::max(modified_similarities[j], std::size_t(measure->similarity()));
                    continue;
                }
                if(i > original_offset && j > modified_offset) {
                    original_similarities[i] = std::max(original_similarities[i], std::size_t(measure->similarity()));
                    modified_similarities[j] = std::max(modified_similarities[j], std::size_t(measure->similarity()));
                    continue;
                }

                std::size_t original_set_pos = subject_edits.original_offset + i;
                std::size_t modified_set_pos = subject_edits.modified_offset + j;

                std::shared_ptr<const construct> set_original = sets_original[original_set_pos];
                std::shared_ptr<const construct> set_modified = sets_modified[modified_set_pos];

                if(set_original->size() >= 3 * set_modified->size()) {
                    continue;
                }

                if(set_modified->size() >= 3 * set_original->size()) {
                    continue;
                }

                if(set_original->size() < 3 * common_set->size()) {
                    continue;
                }

                if(set_modified->size() < 3 * common_set->size()) {
                    continue;
                }

                bool is_similar = measure->max_length() >= 8 
                    ? (10 * measure->similarity() >= 9 * measure->max_length() 
                        && std::size_t(measure->similarity()) > original_similarities[i]
                        && std::size_t(measure->similarity()) > modified_similarities[j])
                    : (2  * measure->similarity() >= measure->max_length()
                       && measure->similarity() == measure->min_length()
                       && 2 * measure->min_length() >= measure->max_length());

                if(is_similar
                    && 3 * common_set_text->size() <= std::size_t(measure->similarity())) {

                    ses::edit_iterator start_edits = edit;
                    ses::edit_iterator last_edits;
                    split_change(subject_edits,
                                 i,
                                 j,
                                 edit,
                                 last_edits);

                    edits.erase(std::next(start_edits));
                    edits.erase(start_edits);

                    edit = correct_common(edit);

                    goto end_move_check;

                }

            }

        }

end_move_check:
        (void)0;

        /** @todo choose smaller move */

    }

}

}
