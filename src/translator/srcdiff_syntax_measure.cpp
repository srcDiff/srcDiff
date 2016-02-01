#include <srcdiff_syntax_measure.hpp>

#include <srcdiff_compare.hpp>
#include <srcdiff_constants.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_diff.hpp>

#include <algorithm>

srcdiff_syntax_measure::srcdiff_syntax_measure(const node_set & set_original, const node_set & set_modified) 
  : srcdiff_measure(set_original, set_modified) {}


static bool is_significant(int & node_pos, const srcml_nodes & nodes, const void * context) {

  const std::shared_ptr<srcml_node> & node = nodes[node_pos];

  if(node->name == "argument_list") {

    int pos = node_pos + 1;
    while(pos < nodes.size() && (nodes[pos]->type == XML_READER_TYPE_ELEMENT || nodes[pos]->name != "argument_list")) {

      if(!nodes[pos]->is_text()) return true;

      ++pos;

    }

    node_pos = pos;

    return false;

  }

  return !node->is_text() && node->name != "operator"
      && node->name != "literal" && node->name != "modifier";

}

void srcdiff_syntax_measure::compute() {

  if(computed) return;

  computed = true;
  
  diff_nodes dnodes = { set_original.nodes(), set_modified.nodes() };

  if((xmlReaderTypes)set_original.nodes().at(set_original.at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)set_modified.nodes().at(set_modified.at(0))->type != XML_READER_TYPE_ELEMENT
     || (srcdiff_compare::node_compare(set_original.nodes().at(set_original.at(0)), set_modified.nodes().at(set_modified.at(0))) != 0
        && !srcdiff_match::is_interchangeable_match(set_original.nodes().at(set_original.at(0))->name, set_original.nodes().at(set_original.at(0))->ns->href,
                                                    set_modified.nodes().at(set_modified.at(0))->name, set_modified.nodes().at(set_modified.at(0))->ns->href)
        && (set_original.nodes().at(set_original.at(0))->name != "block" || set_modified.nodes().at(set_modified.at(0))->name != "block"))) {

    a_similarity = 0;
    a_difference = MAX_INT;

    return;

  }

  class shortest_edit_script ses(srcdiff_compare::node_set_syntax_compare, srcdiff_compare::node_set_index, &dnodes);

  // collect subset of nodes
  node_sets next_node_sets_original = set_original.size() > 1 ? node_sets(set_original.nodes(), set_original.at(1), set_original.back(), is_significant) : node_sets(set_original.nodes());
  node_sets next_node_sets_modified = set_modified.size() > 1 ? node_sets(set_modified.nodes(), set_modified.at(1), set_modified.back(), is_significant) : node_sets(set_modified.nodes());
  original_len = next_node_sets_original.size();
  modified_len = next_node_sets_modified.size();
  int distance = ses.compute((const void *)&next_node_sets_original, original_len, (const void *)&next_node_sets_modified, modified_len);

  process_edit_script(ses.get_script());

}
