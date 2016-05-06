#include <srcdiff_edit_correction.hpp>

#include <srcdiff_text_measure.hpp>
#include <srcdiff_match.hpp>

#include <iostream>

srcdiff_edit_correction::srcdiff_edit_correction(const node_sets & sets_original,
                       							 const node_sets & sets_modified,
						                         edit * edit_script) 
	: sets_original(sets_original), sets_modified(sets_modified), edits(edit_script) {}

edit * srcdiff_edit_correction::correct() {

	for(edit * edit_script = edits; edit_script != nullptr; edit_script = edit_script->next) {

		if(edit_script->length != 1) continue;

		if(edit_script->next == nullptr) continue;
		if(edit_script->next->length != 1) continue;

		if(edit_script->operation == edit_script->next->operation) continue;

		int common_length = edit_script->next->offset_sequence_one - edit_script->offset_sequence_one;
		if(edit_script->operation == SES_DELETE) --common_length;
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

		std::size_t original_set_pos = delete_edit->offset_sequence_one;
		std::size_t modified_set_pos = insert_edit->offset_sequence_two;

		const node_set & set_original = sets_original.at(original_set_pos);
		const node_set & set_modified = sets_modified.at(modified_set_pos);

	  	int original_pos = set_original.at(0);
	  	int modified_pos = set_modified.at(0);

		const std::string & original_tag = set_original.nodes().at(original_pos)->name;
	    const std::string & modified_tag = set_modified.nodes().at(modified_pos)->name;

		const std::string & original_uri = set_original.nodes().at(original_pos)->ns->href;
		const std::string & modified_uri = set_modified.nodes().at(modified_pos)->ns->href;

		if(!(original_tag == modified_tag && original_uri == modified_uri)
			&& !srcdiff_match::is_interchangeable_match(original_tag, original_uri, modified_tag, modified_uri))
			continue;

		srcdiff_text_measure measure(set_original, set_modified);
		measure.compute();

		if(measure.min_length() >= 0.9 * measure.similarity()
			&& 3 * common_set_text.size() < measure.similarity()) {

			++delete_edit->length;
			++insert_edit->length;

			if(edit_script->operation == SES_DELETE) {

				--insert_edit->offset_sequence_two;

			} else {

				--delete_edit->offset_sequence_one;
				delete_edit->offset_sequence_two -= 2;
				insert_edit->offset_sequence_one += 2;
				
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

		}

		// choose smaller move
	}

	return edits;

}
