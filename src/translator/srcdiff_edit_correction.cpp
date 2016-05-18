#include <srcdiff_edit_correction.hpp>

#include <srcdiff_text_measure.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_compare.hpp>

#include <iostream>

srcdiff_edit_correction::srcdiff_edit_correction(const node_sets & sets_original,
                                                 const node_sets & sets_modified,
                                                 class shortest_edit_script & ses) 
    : sets_original(sets_original),
      sets_modified(sets_modified),
      ses(ses) {}

void srcdiff_edit_correction::split_change(edit * delete_edit, edit * insert_edit,
                                           int original_pos, int modified_pos,
                                           edit *& start_edits,
                                           edit *& last_edits) {

    int original_sequence_one_offset = delete_edit->offset_sequence_one;
    int original_sequence_two_offset = delete_edit->offset_sequence_two;
    int original_length = delete_edit->length;
    edit * original_previous = delete_edit->previous;
    edit * original_next = delete_edit->next;

    int modified_sequence_one_offset = insert_edit->offset_sequence_one;
    int modified_sequence_two_offset = insert_edit->offset_sequence_two;
    int modified_length = insert_edit->length;
    edit * modified_previous = insert_edit->previous;
    edit * modified_next = insert_edit->next;

    edit * left_delete = nullptr, * right_delete = nullptr,
         * left_insert = nullptr, * right_insert = nullptr;

    if(original_pos != 0)
        left_delete = delete_edit;
    else if(original_length != 1)
        right_delete = delete_edit;

    if(modified_pos != 0)
        left_insert = insert_edit;
    else if(modified_length != 1)
        right_insert = insert_edit;

    if(original_pos != 0 && original_pos != (original_length - 1)) {

        right_delete = (struct edit *)malloc(sizeof(struct edit));
        if(right_delete == nullptr)
            throw std::bad_alloc();

    }

    if(modified_pos != 0 && modified_pos != (modified_length - 1)) {

        right_insert = (struct edit *)malloc(sizeof(struct edit));
        if(right_insert == nullptr) {
            if(original_pos != 0 && original_pos != (original_length - 1))
                free(right_delete);
            throw std::bad_alloc();

        }

    }

    edit * common_edit = (struct edit *)malloc(sizeof(struct edit));
    if(common_edit == nullptr) {

        if(original_pos != 0 && original_pos != (original_length - 1))
            free(right_delete);
        if(modified_pos != 0 && modified_pos != (modified_length - 1))
            free(right_insert);
        throw std::bad_alloc();

    }

    common_edit->operation = SES_COMMON;
    common_edit->offset_sequence_one = original_sequence_one_offset + original_pos;
    common_edit->offset_sequence_two = modified_sequence_two_offset + modified_pos;
    common_edit->length = 1;

    if(left_insert)
        common_edit->previous = left_insert;
    else if(left_delete)
        common_edit->previous = left_delete;
    else
        common_edit->previous = original_previous;

    if(right_delete)
        common_edit->next = right_delete;
    else if(right_insert)
        common_edit->next = right_insert;
    else
        common_edit->next = modified_next;

    if(original_previous) {

        if(left_delete)
            original_previous->next = left_delete;
        else if(left_insert)
            original_previous->next = left_insert;
        else
            original_previous->next = common_edit;

    }

    if(modified_next) {

        if(right_insert)
            modified_next->previous = right_insert;
        else if(right_delete)
            modified_next->previous = right_delete;
        else
            modified_next->previous = common_edit;

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

        int offset_one = 0;
        if(right_delete)
            right_insert->offset_sequence_one = right_delete->offset_sequence_one + right_delete->length;
        else
            right_insert->offset_sequence_one = common_edit->offset_sequence_one + 1;

        right_insert->offset_sequence_two = common_edit->offset_sequence_two + 1;
        right_insert->length = modified_length - modified_pos - 1;
        right_insert->previous = right_delete ? right_delete : common_edit;
        right_insert->next = modified_next;

    }

    edit * start_edit = nullptr;
    if(left_delete)
        start_edit = left_delete;
    else if(left_insert)
        start_edit = left_insert;
    else
        start_edit = common_edit;

    if(delete_edit == ses.get_script())
       ses.set_script(start_edit);

    start_edits = start_edit;

    edit * last_edit = nullptr;
    if(right_insert)
        last_edit = right_insert;
    else if(right_delete)
        last_edit = right_delete;
    else
        last_edit = common_edit;

    last_edits = last_edit;

    if(original_length == 1 && delete_edit)
        free(delete_edit);

    if(modified_length == 1 && insert_edit)
        free(insert_edit);

}

edit * srcdiff_edit_correction::correct_common_inner(edit * change_edit) {

    edit * delete_edit = change_edit;
    edit * insert_edit = change_edit->next;

    diff_nodes diff = { sets_original.nodes(), sets_modified.nodes() };

    for(int i = 0; i < delete_edit->length; ++i) {

        for(int j = 0; j < insert_edit->length; ++j) {

            std::size_t original_set_pos = delete_edit->offset_sequence_one + i;
            std::size_t modified_set_pos = insert_edit->offset_sequence_two + j;

            const node_set & set_original = sets_original.at(original_set_pos);
            const node_set & set_modified = sets_modified.at(modified_set_pos);

            int original_pos = set_original.at(0);
            int modified_pos = set_modified.at(0);

            const std::string & original_tag = set_original.nodes().at(original_pos)->name;
            const std::string & modified_tag = set_modified.nodes().at(modified_pos)->name;

            const std::string & original_uri = set_original.nodes().at(original_pos)->ns.href;
            const std::string & modified_uri = set_modified.nodes().at(modified_pos)->ns.href;

            if(srcdiff_compare::node_set_syntax_compare(&set_original, &set_modified, &diff) != 0)
                continue;

            edit * start_edits = nullptr;
            edit * last_edits = nullptr;
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

edit * srcdiff_edit_correction::correct_common(edit * start_edit) {

    edit * current = start_edit;

    if(is_change(current))
        current = correct_common_inner(current)->next;
    else
        current = current->next;

    current = current->next;

    if(is_change(current))
        current = correct_common_inner(current);
    else if(current->next)
        current = current->next;

    return current;

}

void srcdiff_edit_correction::correct() {

    // wrongly matched common correction
    for(edit * edit_script = ses.get_script(); edit_script != nullptr; edit_script = edit_script->next) {

        /**
            @todo extend to work if change on either side.
            Maybe form combined delete edit and combined insert edit with common
            search this for one to break skipping the common in each.
            switch out edits if accept.

        */

        // save pointer to before edits
        edit * before = edit_script->previous;

        // save pointer to starting edit
        edit * start_edit = edit_script;

        // guard checks for first edit
        if(edit_script->operation == SES_COMMON) continue;
        if(edit_script->length > 3) continue;

        // adjust if change
        bool is_change_before = is_change(edit_script);
        if(is_change_before) edit_script = edit_script->next;

        // temp clause to maintain behaviour while adjusting code
        if(is_change_before) continue;

        // guard checks for next edit
        if(edit_script->next == nullptr) continue;
        if(edit_script->next->operation == SES_COMMON) continue;
        if(edit_script->next->length > 3) continue;

        // save pointer to after common
        bool is_change_after = is_change(edit_script->next);

        // temp clause to maintain behaviour while adjusting code
        if(is_change_after) continue;

        if(    !is_change_before
            && !is_change_after
            && edit_script->operation == edit_script->next->operation) continue;


        int start_offset = start_edit->offset_sequence_one;
        if(start_edit->operation == SES_DELETE) start_offset += start_edit->length;
        int common_length = edit_script->next->offset_sequence_one - start_offset;
        if(common_length != 1) continue;

        edit * after = is_change_after ? edit_script->next->next->next : edit_script->next->next;

        edit * delete_edit = nullptr;
        edit * insert_edit = nullptr;

        if(start_edit->operation == SES_DELETE) {

            delete_edit = copy_edit(start_edit);
            insert_edit = copy_edit(start_edit->next);

        } else {

            delete_edit = copy_edit(start_edit->next);
            insert_edit = copy_edit(start_edit);

        }

        ++delete_edit->length;
        ++insert_edit->length;

        std::size_t original_offset = 0;
        std::size_t modified_offset = 0;

        if(is_change_before) {

            original_offset = delete_edit->length - 1;
            modified_offset = insert_edit->length - 1;

            if(is_change_after) {

                delete_edit->length += edit_script->next->length;
                insert_edit->length += edit_script->next->next->length;

            } else {

                (edit_script->next->operation == SES_DELETE ? delete_edit : insert_edit) += edit_script->next->length;

            }

        } else if(is_change_after) {

            if(start_edit->operation == SES_DELETE) {

                original_offset = delete_edit->length - 1;
                modified_offset = 0;

                delete_edit->length += edit_script->next->length;               
                --insert_edit->offset_sequence_two;

            } else {

                original_offset = 0;
                modified_offset = insert_edit->length - 1;

                --delete_edit->offset_sequence_one;
                delete_edit->offset_sequence_two -= insert_edit->length;
                insert_edit->offset_sequence_one += delete_edit->length;
                insert_edit->length += edit_script->next->next->length;

                delete_edit->previous = before;

                delete_edit->next = insert_edit;
                insert_edit->previous = delete_edit;
                insert_edit->next = after;
            }

        } else {

            if(start_edit->operation == SES_DELETE) {

                original_offset = delete_edit->length - 1;
                modified_offset = 0;

                --insert_edit->offset_sequence_two;

            } else {

                original_offset = 0;
                modified_offset = insert_edit->length - 1;

                --delete_edit->offset_sequence_one;
                delete_edit->offset_sequence_two -= insert_edit->length;
                insert_edit->offset_sequence_one += delete_edit->length;

            }

            if(start_edit->operation == SES_INSERT) {

                delete_edit->previous = before;

                delete_edit->next = insert_edit;
                insert_edit->previous = delete_edit;
                insert_edit->next = after;

            }

        }

        std::size_t common_pos = delete_edit->offset_sequence_one + original_offset;

        const node_set & common_set = sets_original.at(common_pos);
        node_set common_set_text(sets_original.nodes());
        srcdiff_text_measure::collect_text_node_set(common_set, common_set_text);

        for(int i = 0; i < delete_edit->length; ++i) {

            if(i == original_offset) continue;

            for(int j = 0; j < insert_edit->length; ++j) {

                if(j == modified_offset) continue;

                std::size_t original_set_pos = delete_edit->offset_sequence_one + i;
                std::size_t modified_set_pos = insert_edit->offset_sequence_two + j;

                const node_set & set_original = sets_original.at(original_set_pos);
                const node_set & set_modified = sets_modified.at(modified_set_pos);

                int original_pos = set_original.at(0);
                int modified_pos = set_modified.at(0);

                const std::string & original_tag = set_original.nodes().at(original_pos)->name;
                const std::string & modified_tag = set_modified.nodes().at(modified_pos)->name;

                const std::string & original_uri = set_original.nodes().at(original_pos)->ns.href;
                const std::string & modified_uri = set_modified.nodes().at(modified_pos)->ns.href;

                if(!(original_tag == modified_tag && original_uri == modified_uri)
                    && !srcdiff_match::is_interchangeable_match(original_tag, original_uri, modified_tag, modified_uri))
                    continue;

                if(set_original.size() >= 3 * set_modified.size())
                    continue;

                if(set_modified.size() >= 3 * set_original.size())
                    continue;

                if(set_original.size() < 3 * common_set.size())
                    continue;

                if(set_modified.size() < 3 * common_set.size())
                    continue;

                srcdiff_text_measure measure(set_original, set_modified);
                measure.compute();

                if(measure.similarity() >= 0.9 * measure.min_length()
                    && 3 * common_set_text.size() < measure.similarity()) {

                    if(before)
                        before->next = delete_edit;
                    if(after)
                        after->previous = insert_edit;

                    free(edit_script);
                    free(edit_script->next);

                    if(edit_script == ses.get_script() || edit_script->next == ses.get_script())
                        ses.set_script(delete_edit);

                    edit * last_edits = nullptr;
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

end_move_check:
    (void)0;
        /** @todo choose smaller move */

    }

}
