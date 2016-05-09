#include <srcdiff_edit_correction.hpp>

#include <srcdiff_text_measure.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_compare.hpp>

#include <iostream>

srcdiff_edit_correction::srcdiff_edit_correction(const node_sets & sets_original,
                       							 const node_sets & sets_modified,
						                         edit * edit_script) 
	: sets_original(sets_original), sets_modified(sets_modified), edits(edit_script) {}

static void split_change(edit * delete_edit, edit * insert_edit, int original_pos, int modified_pos) {

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
	else
		right_delete = delete_edit;

	if(modified_pos != 0)
		left_insert = insert_edit;
	else
		right_insert = insert_edit;

	if(original_pos != 0 && original_pos != (original_length - 1)) {

		right_delete = (struct edit *)malloc(sizeof(struct edit));
		if(right_delete == nullptr) {

			free(common_edit);
			throw std::bad_alloc();

		}

	}

	if(modified_pos != 0 && modified_pos != (modified_length - 1)) {

		right_insert = (struct edit *)malloc(sizeof(struct edit));
		if(right_insert == nullptr) {

			free(common_edit);
			if(original_pos != 0 && original_pos != (original_length - 1))
				free(right_delete);
			throw std::bad_alloc();

		}

	}

	edit * common_edit = (struct edit *)malloc(sizeof(struct edit));
	if(common_edit == nullptr)
		throw std::bad_alloc();
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

		int offset_one = left_delete ? left_delete->offset_sequence_one + left_delete->length : modified_sequence_one_offset;
		left_insert->offset_sequence_one = offset_one;
		left_insert->offset_sequence_two = modified_sequence_two_offset;
		left_insert->length = modified_pos;
		left_insert->previous = left_delete ? left_delete : original_previous;
		left_insert->next = common_edit;

	}

	if(right_delete) {

		right_delete->operation = SES_DELETE;
		right_delete->offset_sequence_one = original_pos + 1;
		right_delete->offset_sequence_two = modified_pos + 1;
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
			right_insert->offset_sequence_one = original_pos + 1;

		right_insert->offset_sequence_two = modified_pos + 1;
		right_insert->length = modified_length - modified_pos - 1;
		right_insert->previous = right_delete ? right_delete : common_edit;
		right_insert->next = modified_next;

	}

}

edit * srcdiff_edit_correction::correct() {

	for(edit * edit_script = edits; edit_script != nullptr; edit_script = edit_script->next) {

		if(edit_script->length > 3) continue;

		if(edit_script->next == nullptr) continue;
		if(edit_script->next->length > 3) continue;

		if(edit_script->operation == edit_script->next->operation) continue;

		int common_length = edit_script->next->offset_sequence_one - edit_script->offset_sequence_one;
		if(edit_script->operation == SES_DELETE) common_length -= edit_script->length;
		if(common_length != 1) continue;

		// move mistaken as common
		edit * delete_edit = nullptr;
		edit * insert_edit = nullptr;
		if(edit_script->operation == SES_DELETE) {

			delete_edit = edit_script;
			insert_edit = edit_script->next;

		} else {

			delete_edit = edit_script->next;
			insert_edit = edit_script;

		}

		std::size_t common_pos = edit_script->operation == SES_DELETE ? 
			edit_script->offset_sequence_one + edit_script->length
			: edit_script->next->offset_sequence_one - 1;

		const node_set & common_set = sets_original.at(common_pos);
		node_set common_set_text(sets_original.nodes());
		srcdiff_text_measure::collect_text_node_set(common_set, common_set_text);

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

					++delete_edit->length;
					++insert_edit->length;

					if(edit_script->operation == SES_DELETE) {

						--insert_edit->offset_sequence_two;

					} else {

						--delete_edit->offset_sequence_one;
						delete_edit->offset_sequence_two -= insert_edit->length;
						insert_edit->offset_sequence_one += delete_edit->length;
						
					}

					if(edit_script->operation == SES_INSERT) {

						if(edit_script->previous)
							edit_script->previous->next = delete_edit;
			
						delete_edit->previous = edit_script->previous;

						edit * after = delete_edit->next;

						delete_edit->next = insert_edit;
						insert_edit->previous = delete_edit;
						insert_edit->next = after;
						if(after)
							after->previous = insert_edit;

						if(insert_edit == edits)
							edits = delete_edit;

					}

					edit_script = insert_edit;				

					std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << original_set_pos << '\n';
					std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << modified_set_pos << "\n\n";

					std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << delete_edit->offset_sequence_one << '\n';
					std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << delete_edit->offset_sequence_two << '\n';
					std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << delete_edit->length << "\n\n";
					std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << insert_edit->offset_sequence_one << '\n';
					std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << insert_edit->offset_sequence_two << '\n';
					std::cerr << "HERE: " << __FILE__ << ' ' << __FUNCTION__ << ' ' << __LINE__ << ' ' << insert_edit->length << "\n\n";

					split_change(delete_edit, insert_edit, i, j);

					goto end_move_check;

				}

			}

		}

end_move_check:
	(void)0;
		/** @todo choose smaller move */

	}

	return edits;

}
