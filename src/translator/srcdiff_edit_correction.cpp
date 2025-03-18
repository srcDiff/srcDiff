// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_correction.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_edit_correction.hpp>

#include <list>
#include <algorithm>

#include <iostream>

srcdiff_edit_correction::srcdiff_edit_correction(const construct::construct_list_view sets_original,
                                                 const construct::construct_list_view sets_modified,
                                                 srcdiff_shortest_edit_script & ses) 
    : sets_original(sets_original),
      sets_modified(sets_modified),
      ses(ses) {}

void srcdiff_edit_correction::split_change(edit_t * delete_edit, edit_t * insert_edit,
                                           int original_pos, int modified_pos,
                                           edit_t *& start_edits,
                                           edit_t *& last_edits) {

    int original_sequence_one_offset = delete_edit->offset_sequence_one;
    int original_sequence_two_offset = delete_edit->offset_sequence_two;
    int original_length = delete_edit->length;
    edit_t * original_previous = delete_edit->previous;

    int modified_sequence_two_offset = insert_edit->offset_sequence_two;
    int modified_length = insert_edit->length;

    edit_t * modified_next = insert_edit->next;

    edit_t * left_delete = nullptr, * right_delete = nullptr,
           * left_insert = nullptr, * right_insert = nullptr;

    if(original_pos != 0) {
        left_delete = delete_edit;
    }
    else if(original_length != 1) {
        right_delete = delete_edit;
    }

    if(modified_pos != 0) {
        left_insert = insert_edit;
    }
    else if(modified_length != 1) {
        right_insert = insert_edit;
    }

    if(original_pos != 0 && original_pos != (original_length - 1)) {

        right_delete = (struct edit_t *)malloc(sizeof(struct edit_t));
        if(right_delete == nullptr)
            throw std::bad_alloc();

    }

    if(modified_pos != 0 && modified_pos != (modified_length - 1)) {

        right_insert = (struct edit_t *)malloc(sizeof(struct edit_t));
        if(right_insert == nullptr) {
            if(original_pos != 0 && original_pos != (original_length - 1)) {
                free(right_delete);
            }
            throw std::bad_alloc();

        }

    }

    edit_t * common_edit = (struct edit_t *)malloc(sizeof(struct edit_t));
    if(common_edit == nullptr) {

        if(original_pos != 0 && original_pos != (original_length - 1)) {
            free(right_delete);
        }
        if(modified_pos != 0 && modified_pos != (modified_length - 1)) {
            free(right_insert);
        }
        throw std::bad_alloc();

    }

    common_edit->operation = SES_COMMON;
    common_edit->offset_sequence_one = original_sequence_one_offset + original_pos;
    common_edit->offset_sequence_two = modified_sequence_two_offset + modified_pos;
    common_edit->length = 1;

    if(left_insert) {
        common_edit->previous = left_insert;
    }
    else if(left_delete) {
        common_edit->previous = left_delete;
    }
    else {
        common_edit->previous = original_previous;
    }

    if(right_delete) {
        common_edit->next = right_delete;
    }
    else if(right_insert) {
        common_edit->next = right_insert;
    }
    else {
        common_edit->next = modified_next;
    }

    if(original_previous) {

        if(left_delete) {
            original_previous->next = left_delete;
        }
        else if(left_insert) {
            original_previous->next = left_insert;
        }
        else {
            original_previous->next = common_edit;
        }

    }

    if(modified_next) {

        if(right_insert) {
            modified_next->previous = right_insert;
        }
        else if(right_delete) {
            modified_next->previous = right_delete;
        }
        else {
            modified_next->previous = common_edit;
        }

    }

    if(left_delete) {

        left_delete->operation = SES_DELETE;
        left_delete->offset_sequence_one = original_sequence_one_offset;
        left_delete->offset_sequence_two = original_sequence_two_offset;
        left_delete->length = original_pos;
        left_delete->previous = original_previous;
        left_delete->next = left_insert ? left_insert : common_edit;

    }

    if(left_insert) {

        left_insert->operation = SES_INSERT;

        int offset_one = left_delete ? left_delete->offset_sequence_one + left_delete->length : original_sequence_one_offset;
        left_insert->offset_sequence_one = offset_one;
        left_insert->offset_sequence_two = modified_sequence_two_offset;
        left_insert->length = modified_pos;
        left_insert->previous = left_delete ? left_delete : original_previous;
        left_insert->next = common_edit;

    }

    if(right_delete) {

        right_delete->operation = SES_DELETE;
        right_delete->offset_sequence_one = common_edit->offset_sequence_one + 1;
        right_delete->offset_sequence_two = common_edit->offset_sequence_two + 1;
        right_delete->length = original_length - original_pos - 1;
        right_delete->previous = common_edit;
        right_delete->next = right_insert ? right_insert : modified_next;

    }

    if(right_insert) {

        right_insert->operation = SES_INSERT;

        if(right_delete) {
            right_insert->offset_sequence_one = right_delete->offset_sequence_one + right_delete->length;
        }
        else {
            right_insert->offset_sequence_one = common_edit->offset_sequence_one + 1;
        }

        right_insert->offset_sequence_two = common_edit->offset_sequence_two + 1;
        right_insert->length = modified_length - modified_pos - 1;
        right_insert->previous = right_delete ? right_delete : common_edit;
        right_insert->next = modified_next;

    }

    edit_t * start_edit = nullptr;
    if(left_delete) {
        start_edit = left_delete;
    }
    else if(left_insert) {
        start_edit = left_insert;
    }
    else {
        start_edit = common_edit;
    }

    if(delete_edit == ses.script()) {
       ses.script(start_edit);
    }

    start_edits = start_edit;

    edit_t * last_edit = nullptr;
    if(right_insert) {
        last_edit = right_insert;
    }
    else if(right_delete) {
        last_edit = right_delete;
    }
    else {
        last_edit = common_edit;
    }

    last_edits = last_edit;

    if(original_length == 1 && delete_edit) {
        free(delete_edit);
    }

    if(modified_length == 1 && insert_edit) {
        free(insert_edit);
    }

}

edit_t * srcdiff_edit_correction::correct_common_inner(edit_t * change_edit) {

    edit_t * delete_edit = change_edit;
    edit_t * insert_edit = change_edit->next;

    for(std::size_t i = 0; i < delete_edit->length; ++i) {

        for(std::size_t j = 0; j < insert_edit->length; ++j) {

            std::size_t original_set_pos = delete_edit->offset_sequence_one + i;
            std::size_t modified_set_pos = insert_edit->offset_sequence_two + j;

            std::shared_ptr<const construct> set_original = sets_original[original_set_pos];
            std::shared_ptr<const construct> set_modified = sets_modified[modified_set_pos];

            if(*set_original != *set_modified) {
                continue;
            }

            edit_t * start_edits = nullptr;
            edit_t * last_edits = nullptr;
            split_change(delete_edit,
                         insert_edit,
                         i,
                         j,
                         start_edits, 
                         last_edits);

            return last_edits;

        }

    }

    return change_edit->next;

}

edit_t * srcdiff_edit_correction::correct_common(edit_t * start_edit) {

    edit_t * current = start_edit;

    if(is_change(current)) {
        current = correct_common_inner(current)->next;
    }
    else {
        current = current->next;
    }

    current = current->next;

    if(is_change(current)) {
        current = correct_common_inner(current);
    }
    else if(current->next) {
        current = current->next;
    }

    return current;

}

std::shared_ptr<srcdiff::text_measurer> srcdiff_edit_correction::edit2measure(int original_offset, int modified_offset) {

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
        return std::shared_ptr<srcdiff::text_measurer>();

    std::shared_ptr<srcdiff::text_measurer> measure = std::make_shared<srcdiff::text_measurer>(*set_original, *set_modified);
    measure->compute();

    return measure;

}

void srcdiff_edit_correction::correct() {

    // wrongly matched common correction
    for(edit_t * edit_script = ses.script(); edit_script != nullptr; edit_script = edit_script->next) {

        // save pointer to before edits
        edit_t * before = edit_script->previous;

        // save pointer to starting edit
        edit_t * start_edit = edit_script;

        std::list<edit_t *> free_edit_list;
        free_edit_list.push_back(edit_script);

        // guard checks for first edit
        if(edit_script->operation == SES_COMMON) continue;
        if(edit_script->length > 3) continue;

        // adjust if change
        bool is_change_before = is_change(edit_script);
        if(is_change_before) {

            edit_script = edit_script->next;
            free_edit_list.push_back(edit_script);

        }

        if(is_change_before && edit_script->length > 3) continue;

        // guard checks for next edit
        if(edit_script->next == nullptr) continue;
        if(edit_script->next->operation == SES_COMMON) continue;
        if(edit_script->next->length > 3) continue;

        free_edit_list.push_back(edit_script->next);

        bool is_change_after = is_change(edit_script->next);

        if(is_change_after && edit_script->next->next->length > 3) continue;

        if(is_change_after) {
            free_edit_list.push_back(edit_script->next->next);
        }

        if(    !is_change_before
            && !is_change_after
            && edit_script->operation == edit_script->next->operation) continue;

        int start_offset = start_edit->offset_sequence_one;
        if(start_edit->operation == SES_DELETE) start_offset += start_edit->length;
        int common_length = edit_script->next->offset_sequence_one - start_offset;
        if(common_length != 1) continue;

        edit_t * after = is_change_after ? edit_script->next->next->next : edit_script->next->next;

        edit_t * delete_edit = nullptr;
        edit_t * insert_edit = nullptr;

        if(start_edit->operation == SES_DELETE) {

            delete_edit = copy_edit(start_edit);
            if(!is_change_before && is_change_after) {
                insert_edit = copy_edit(start_edit->next->next);
            }
            else {
                insert_edit = copy_edit(start_edit->next);
            }

        } else {

            delete_edit = copy_edit(start_edit->next);
            insert_edit = copy_edit(start_edit);

        }

        ++delete_edit->length;
        ++insert_edit->length;

        delete_edit->next = insert_edit;
        delete_edit->previous = before;
        insert_edit->previous = delete_edit;
        insert_edit->next = after;
        std::size_t original_offset = 0;
        std::size_t modified_offset = 0;

        if(is_change_before) {

            original_offset = delete_edit->length - 1;
            modified_offset = insert_edit->length - 1;

            if(is_change_after) {

                delete_edit->length += edit_script->next->length;
                insert_edit->length += edit_script->next->next->length;
                insert_edit->offset_sequence_one = delete_edit->offset_sequence_one + delete_edit->length;

            } else {

                (edit_script->next->operation == SES_DELETE ? delete_edit->length : insert_edit->length) += edit_script->next->length;
                insert_edit->offset_sequence_one = delete_edit->offset_sequence_one + delete_edit->length;

            }

        } else {

            if(start_edit->operation == SES_DELETE) {

                original_offset = delete_edit->length - 1;
                modified_offset = 0;

                --insert_edit->offset_sequence_two;

                if(is_change_after) {
                    delete_edit->length += edit_script->next->length;  
                }

            } else {

                original_offset = 0;
                modified_offset = insert_edit->length - 1;

                --delete_edit->offset_sequence_one;
                delete_edit->offset_sequence_two -= insert_edit->length;
                insert_edit->offset_sequence_one += delete_edit->length;

                if(is_change_after) {
                    insert_edit->length += edit_script->next->next->length;
                }

            }

        }

        std::size_t common_pos = delete_edit->offset_sequence_one + original_offset;

        std::shared_ptr<const construct> common_set = sets_original[common_pos];
        std::shared_ptr<construct> common_set_text(std::make_shared<construct>(common_set->nodes()));
        srcdiff::text_measurer::collect_text_element(*common_set, *common_set_text);

        std::vector<std::size_t> original_similarities(delete_edit->length);
        std::vector<std::size_t> modified_similarities(insert_edit->length);

        for(std::size_t i = 0; i < delete_edit->length; ++i) {

            if(i == original_offset) continue;

            for(std::size_t j = 0; j < insert_edit->length; ++j) {

                if(j == modified_offset) continue;

                std::shared_ptr<srcdiff::text_measurer> measure 
                    = srcdiff_edit_correction::edit2measure(delete_edit->offset_sequence_one + i,
                                                            insert_edit->offset_sequence_two + j);               
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

                std::size_t original_set_pos = delete_edit->offset_sequence_one + i;
                std::size_t modified_set_pos = insert_edit->offset_sequence_two + j;

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

                    if(before) {
                        before->next = delete_edit;
                    }
                    if(after) {
                        after->previous = insert_edit;
                    }

                    for(edit_t * edit : free_edit_list) {
                        free(edit);
                    }

                    if(start_edit == ses.script()) {
                        ses.script(delete_edit);
                    }

                    edit_t * last_edits = nullptr;
                    split_change(delete_edit,
                                 insert_edit,
                                 i,
                                 j,
                                 edit_script,
                                 last_edits);
                    edit_script = correct_common(edit_script);

                    goto end_move_check;

                }

            }

        }

        free(delete_edit);
        free(insert_edit);

end_move_check:
        (void)0;

        /** @todo choose smaller move */

    }

}
