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
#include <algorithm>

#include <iostream>

namespace srcdiff {

edit_corrector::edit_corrector(const construct::construct_list_view sets_original,
                               const construct::construct_list_view sets_modified,
                               edit_t*& edits) 
    : sets_original(sets_original),
      sets_modified(sets_modified),
      edits(edits) {}

void edit_corrector::split_change(edit_t * subject_edits,
                                  int original_pos, int modified_pos,
                                  edit_t *& start_edits,
                                  edit_t *& last_edits) {

    int sequence_one_offset = subject_edits->offset_sequence_one;
    int sequence_two_offset = subject_edits->offset_sequence_two;
    int original_length = subject_edits->length;
    int modified_length = subject_edits->length_two;

    edit_t* previous = subject_edits->previous;
    edit_t* next     = subject_edits->next;

    edit_t* left_edit = nullptr;
    if(original_pos || modified_pos) {
        left_edit  = (struct edit_t *)malloc(sizeof(struct edit_t));
        if(!left_edit) throw std::bad_alloc();
    }

    bool is_delete = original_length - original_pos - 1;
    bool is_insert = modified_length - modified_pos - 1;

    edit_t* right_edit = nullptr;
    if(is_delete || is_insert) {
        right_edit = (struct edit_t *)malloc(sizeof(struct edit_t));
        if(!right_edit) throw std::bad_alloc();
    }

    edit_t* common_edit = (struct edit_t *)malloc(sizeof(struct edit_t));
    if(!common_edit) throw std::bad_alloc();

    common_edit->operation = SES_COMMON;
    common_edit->offset_sequence_one = sequence_one_offset + original_pos;
    common_edit->offset_sequence_two = sequence_two_offset + modified_pos;
    common_edit->length = 1;
    common_edit->length_two = 1;

    if(left_edit) {
        common_edit->previous = left_edit;
    } else {
        common_edit->previous = previous;
    }

    if(right_edit) {
        common_edit->next = right_edit;
    } else {
        common_edit->next = next;
    }

    if(previous) {
        if(left_edit) {
            previous->next = left_edit;
        } else {
            previous->next = common_edit;
        }
    }

    if(next) {
        if(right_edit) {
            next->previous = right_edit;
        } else {
            next->previous = common_edit;
        }
    }

    if(left_edit) {

        left_edit->operation = original_pos && modified_pos ? SES_CHANGE : (original_pos ? SES_DELETE : SES_INSERT);
        left_edit->previous = previous;
        left_edit->next = common_edit;
        left_edit->offset_sequence_one = sequence_one_offset;
        left_edit->offset_sequence_two = sequence_two_offset;
        left_edit->length = original_pos;

        if(!original_pos) {
            left_edit->length = modified_pos;
        } else {
            left_edit->length_two = modified_pos;
        }
    }

    if(right_edit) {

        right_edit->operation = is_delete && is_insert ? SES_CHANGE : (is_delete ? SES_DELETE : SES_INSERT);
        right_edit->previous = common_edit;
        right_edit->next = next;
        right_edit->offset_sequence_one = common_edit->offset_sequence_one + 1;
        right_edit->offset_sequence_two = common_edit->offset_sequence_two + 1;
        right_edit->length = original_length - original_pos - 1;

        if(!is_delete) {
            right_edit->length = modified_length - modified_pos - 1;
        } else {
            right_edit->length_two = modified_length - modified_pos - 1;
        }
    }

    // for(edit_t* temp = left_edit; temp != nullptr; temp = temp->next) {
    //     std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << temp->operation << '\n';
    //     std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << temp->offset_sequence_one << '\n';
    //     std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << temp->length << '\n';
    //     std::cerr << sets_original[temp->offset_sequence_one]->to_string() << '\n';
    //     std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << temp->offset_sequence_two << '\n';
    //     std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << temp->length_two << '\n';
    //     std::cerr << sets_modified[temp->offset_sequence_two]->to_string() << '\n';
    // }

    edit_t * start_edit = nullptr;
    if(left_edit) {
        start_edit = left_edit;
    } else {
        start_edit = common_edit;
    }

    if(subject_edits == edits) {
       edits = start_edit;
    }

    start_edits = start_edit;

    edit_t * last_edit = nullptr;
    if(right_edit) {
        last_edit = right_edit;
    } else {
        last_edit = common_edit;
    }

    last_edits = last_edit;

    if(original_length == 1 && modified_length == 1) {
        free(subject_edits);
    }

}

edit_t * edit_corrector::correct_common_inner(edit_t * change_edit) {

    for(std::size_t i = 0; i < change_edit->length; ++i) {

        for(std::size_t j = 0; j < change_edit->length_two; ++j) {

            std::size_t original_set_pos = change_edit->offset_sequence_one + i;
            std::size_t modified_set_pos = change_edit->offset_sequence_two + j;

            std::shared_ptr<const construct> set_original = sets_original[original_set_pos];
            std::shared_ptr<const construct> set_modified = sets_modified[modified_set_pos];

            if(*set_original != *set_modified) {
                continue;
            }

            edit_t * start_edits = nullptr;
            edit_t * last_edits = nullptr;
            split_change(change_edit,
                         i,
                         j,
                         start_edits, 
                         last_edits);

            return last_edits;

        }

    }

    return change_edit;

}

edit_t * edit_corrector::correct_common(edit_t * start_edit) {

    edit_t * current = start_edit;

    if(current->operation == SES_CHANGE) {
        current = correct_common_inner(current)->next;
    }
    else {
        current = current->next;
    }

    current = current->next;

    if(current->operation == SES_CHANGE) {
        current = correct_common_inner(current);
    }
    else if(current->next) {
        current = current->next;
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
    for(edit_t * edit_script = edits; edit_script != nullptr; edit_script = edit_script->next) {

        // save pointer to before edits
        edit_t * before = edit_script->previous;

        // save pointer to starting edit
        edit_t * start_edit = edit_script;

        std::list<edit_t *> free_edit_list;
        free_edit_list.push_back(edit_script);

        bool is_change_before = edit_script->operation == SES_CHANGE;

        // guard checks for first edit
        if(edit_script->operation == SES_COMMON) continue;
        if(edit_script->length > 3) continue;
        if(is_change_before && edit_script->length_two > 3) continue;

        // guard checks for next edit
        edit_t * next = edit_script->next;
        if(next == nullptr) continue;
        if(next->operation == SES_COMMON) continue;
        if(next->length > 3) continue;

        free_edit_list.push_back(next);

        bool is_change_after = next->operation == SES_CHANGE;

        if(is_change_after && next->length > 3) continue;

        if(    !is_change_before
            && !is_change_after
            && edit_script->operation == next->operation) continue;

        // start edit should be edit_script here (check later)
        int start_offset = start_edit->offset_sequence_one;
        if(    start_edit->operation == SES_DELETE
            || start_edit->operation == SES_CHANGE) {
            start_offset += start_edit->length;
        }

        int common_length = next->offset_sequence_one - start_offset;
        if(common_length != 1) continue;

        edit_t * after = next->next;

        edit_t * subject_edits = copy_edit(start_edit);
        subject_edits->operation = SES_CHANGE;

        if(start_edit->operation == SES_DELETE) {
            subject_edits->offset_sequence_two = start_edit->next->offset_sequence_two;
            subject_edits->length_two = start_edit->next->length;
        } else if(start_edit->operation == SES_INSERT) {
            subject_edits->length_two = subject_edits->length;
            subject_edits->offset_sequence_one = start_edit->next->offset_sequence_one;
            subject_edits->length = start_edit->next->length;
        }// change?

        ++subject_edits->length;
        ++subject_edits->length_two;

        subject_edits->next = after;
        subject_edits->previous = before;

        std::size_t original_offset = 0;
        std::size_t modified_offset = 0;

        if(is_change_before) {

            original_offset = subject_edits->length - 1;
            modified_offset = subject_edits->length_two - 1;

            if(is_change_after) {

                subject_edits->length += next->length;
                subject_edits->length_two += next->length_two;

            } else {
                (next->operation == SES_DELETE ? subject_edits->length : subject_edits->length_two) += next->length;
            }

        } else {

            if(start_edit->operation == SES_DELETE) {

                original_offset = subject_edits->length - 1;
                modified_offset = 0;

                --subject_edits->offset_sequence_two;

                if(is_change_after) {
                    subject_edits->length += next->length;  
                }

            } else {

                original_offset = 0;
                modified_offset = subject_edits->length_two - 1;

                --subject_edits->offset_sequence_one;

                if(is_change_after) {
                    subject_edits->length_two += next->length_two;
                }

            }

        }

        std::size_t common_pos = subject_edits->offset_sequence_one + original_offset;

        std::shared_ptr<const construct> common_set = sets_original[common_pos];
        std::shared_ptr<construct> common_set_text(std::make_shared<construct>(common_set->nodes()));
        text_measurer::collect_text_element(*common_set, *common_set_text);

        std::vector<std::size_t> original_similarities(subject_edits->length);
        std::vector<std::size_t> modified_similarities(subject_edits->length_two);

        for(std::size_t i = 0; i < subject_edits->length; ++i) {

            if(i == original_offset) continue;

            for(std::size_t j = 0; j < subject_edits->length_two; ++j) {

                if(j == modified_offset) continue;

                std::shared_ptr<text_measurer> measure 
                    = edit_corrector::edit2measure(subject_edits->offset_sequence_one + i,
                                                   subject_edits->offset_sequence_two + j);               
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

                std::size_t original_set_pos = subject_edits->offset_sequence_one + i;
                std::size_t modified_set_pos = subject_edits->offset_sequence_two + j;

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
                        before->next = subject_edits;
                    }
                    if(after) {
                        after->previous = subject_edits;
                    }

                    for(edit_t * edit : free_edit_list) {
                        free(edit);
                    }

                    if(start_edit == edits) {
                        edits = subject_edits;
                    }

                    edit_t * last_edits = nullptr;
                    split_change(subject_edits,
                                 i,
                                 j,
                                 edit_script,
                                 last_edits);
                    edit_script = correct_common(edit_script);

                    goto end_move_check;

                }

            }

        }

        free(subject_edits);

end_move_check:
        (void)0;

        /** @todo choose smaller move */

    }

}

}
