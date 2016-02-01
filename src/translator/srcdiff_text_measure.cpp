#include <srcdiff_text_measure.hpp>


#include <srcdiff_compare.hpp>
#include <srcdiff_constants.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_diff.hpp>

#include <algorithm>
#include <cassert>

const int SIZE_THRESHOLD = 1000;

srcdiff_text_measure::srcdiff_text_measure(const node_set & set_original, const node_set & set_modified) 
  : srcdiff_measure(set_original, set_modified),
    set_original_text(set_original.nodes()),
    set_modified_text(set_modified.nodes()) {}

void srcdiff_text_measure::collect_text() {

  unsigned int olength = set_original.size();
  unsigned int nlength = set_modified.size();

  for(unsigned int i = 0; i < olength; ++i)
    if(set_original.nodes().at(set_original.at(i))->is_text() && !set_original.nodes().at(set_original.at(i))->is_white_space())
      set_original_text.push_back(set_original.at(i));

  for(unsigned int i = 0; i < nlength; ++i)
    if(set_modified.nodes().at(set_modified.at(i))->is_text() && !set_modified.nodes().at(set_modified.at(i))->is_white_space())
      set_modified_text.push_back(set_modified.at(i));

  original_len = set_original_text.size();
  modified_len = set_modified_text.size();

}

void srcdiff_text_measure::collect_important_text() {

  unsigned int olength = set_original.size();
  unsigned int nlength = set_modified.size();

  for(unsigned int i = 0; i < olength; ++i) {

    if(set_original.nodes().at(set_original.at(i))->name == "operator"
      || set_original.nodes().at(set_original.at(i))->name == "modifier") {

      if(set_original.nodes().at(set_original.at(i))->extra & 0x1) continue;

      if(set_original.nodes().at(set_original.at(i))->parent && (*set_original.nodes().at(set_original.at(i))->parent)->name != "name") continue;

      if((set_original.at(i) + 1) < set_original.nodes().size() && set_original.nodes().at(set_original.at(i) + 1)->is_text()
        && (*set_original.nodes().at(set_original.at(i) + 1)->content == "::")) continue;

      while(set_original.nodes().at(set_original.at(i))->type != XML_READER_TYPE_END_ELEMENT)
        ++i;

    }

    if(set_original.nodes().at(set_original.at(i))->is_text() && !set_original.nodes().at(set_original.at(i))->is_white_space()
      && set_original.nodes().at(set_original.at(i))->content
      && ((set_original.nodes().at(set_original.at(i))->parent &&
          (*set_original.nodes().at(set_original.at(i))->parent)->name == "operator")
      ||   (*set_original.nodes().at(set_original.at(i))->content != "("
        && *set_original.nodes().at(set_original.at(i))->content != ")"
        && *set_original.nodes().at(set_original.at(i))->content != "{"
        && *set_original.nodes().at(set_original.at(i))->content != "}"
        && *set_original.nodes().at(set_original.at(i))->content != ";"
        && *set_original.nodes().at(set_original.at(i))->content != ",")))
      set_original_text.push_back(set_original.at(i));

  }

  for(unsigned int i = 0; i < nlength; ++i) {

    if(set_modified.nodes().at(set_modified.at(i))->name == "operator"
      || set_modified.nodes().at(set_modified.at(i))->name == "modifier") {

      if(set_modified.nodes().at(set_modified.at(i))->extra & 0x1) continue;

      if(set_modified.nodes().at(set_modified.at(i))->parent && (*set_modified.nodes().at(set_modified.at(i))->parent)->name != "name") continue;

      if((set_modified.at(i) + 1) < set_modified.nodes().size() && set_modified.nodes().at(set_modified.at(i) + 1)->is_text()
        && (*set_modified.nodes().at(set_modified.at(i) + 1)->content == "::")) continue;

      while(set_modified.nodes().at(set_modified.at(i))->type != XML_READER_TYPE_END_ELEMENT)
        ++i;

    }

    if(set_modified.nodes().at(set_modified.at(i))->is_text() && !set_modified.nodes().at(set_modified.at(i))->is_white_space()
      && set_modified.nodes().at(set_modified.at(i))->content
      && ((set_modified.nodes().at(set_modified.at(i))->parent
        && (*set_modified.nodes().at(set_modified.at(i))->parent)->name == "operator")
      ||  (*set_modified.nodes().at(set_modified.at(i))->content != "("
        && *set_modified.nodes().at(set_modified.at(i))->content != ")"
        && *set_modified.nodes().at(set_modified.at(i))->content != "{"
        && *set_modified.nodes().at(set_modified.at(i))->content != "}"
        && *set_modified.nodes().at(set_modified.at(i))->content != ";"
        && *set_modified.nodes().at(set_modified.at(i))->content != ",")))
      set_modified_text.push_back(set_modified.at(i));

  }

  original_len = set_original_text.size();
  modified_len = set_modified_text.size();

}

void srcdiff_text_measure::compute() {

  if(computed) return;

  computed = true;

  diff_nodes dnodes = { set_original.nodes(), set_modified.nodes() };

  if((xmlReaderTypes)set_original.nodes().at(set_original.at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)set_modified.nodes().at(set_modified.at(0))->type != XML_READER_TYPE_ELEMENT
     || (srcdiff_compare::node_compare(set_original.nodes().at(set_original.at(0)), set_modified.nodes().at(set_modified.at(0))) != 0
        && !srcdiff_match::is_interchangeable_match(set_original.nodes().at(set_original.at(0))->name, set_original.nodes().at(set_original.at(0))->ns->href,
                                                    set_modified.nodes().at(set_modified.at(0))->name, set_modified.nodes().at(set_modified.at(0))->ns->href)
        && (set_original.nodes().at(set_original.at(0))->name != "block" || set_modified.nodes().at(set_modified.at(0))->name != "block"))) {

    a_similarity = MAX_INT;
    a_difference = MAX_INT;

    return;

  }

  collect_important_text();

  if(original_len < SIZE_THRESHOLD || modified_len < SIZE_THRESHOLD) {

    class shortest_edit_script ses(srcdiff_compare::node_index_compare, srcdiff_compare::node_index, &dnodes);
    ses.compute((const void *)&set_original_text, original_len, (const void *)&set_modified_text, modified_len);

    process_edit_script(ses.get_script(), a_similarity, a_difference);
    
  } else {

    int original_blocks = original_len / SIZE_THRESHOLD;
    int modified_blocks = modified_len / SIZE_THRESHOLD;
    int num_blocks = std::min(original_blocks, modified_blocks);
    int i = 0;
    for(; i < num_blocks; ++i) {

      class shortest_edit_script ses(srcdiff_compare::node_index_compare, srcdiff_compare::node_array_index, &dnodes);
      int original_size = std::min(SIZE_THRESHOLD, original_len - SIZE_THRESHOLD * i);
      int modified_size = std::min(SIZE_THRESHOLD, modified_len - SIZE_THRESHOLD * i);
      ses.compute((const void *)(set_original_text.data() + SIZE_THRESHOLD * i),
                  original_size,
                  (const void *)(set_modified_text.data() + SIZE_THRESHOLD * i),
                  modified_size);

      int similarity = 0, difference = 0;
      process_edit_script(ses.get_script(), similarity, difference);

      a_similarity += similarity;
      a_difference += difference;

    }

    if(original_blocks > num_blocks)
      a_difference += original_len - num_blocks * SIZE_THRESHOLD;

    if(modified_blocks > num_blocks)
      a_difference += modified_len - num_blocks * SIZE_THRESHOLD;

    // std::sort(set_original_text.begin(), set_original_text.end());
    // std::sort(set_modified_text.begin(), set_modified_text.end());

    // int i = 0, j = 0;
    // while(i < original_len && j < modified_len) {

    //   if(set_original_text.at(i) == set_modified_text.at(j)) {

    //     ++a_similarity;
    //     ++i;
    //     ++j;

    //   } else {

    //     ++a_difference;
    //     if(set_original_text.at(i) < set_modified_text.at(j))
    //       ++i;
    //     else
    //       ++j;

    //   }

    // }

    // if(i < original_len)
    //   a_difference += original_len - i;

    // if(j < modified_len)
    // //   a_difference += modified_len - j;

    // for(int i = 0, j = 1; j < original_len && j < modified_len; ++i, ++j)
    //   if(  set_original_text.at(i) == set_modified_text.at(i)
    //     && set_original_text.at(j) == set_modified_text.at(j))
    //     ++a_similarity;
    //   else
    //     ++a_difference;

  }

}
