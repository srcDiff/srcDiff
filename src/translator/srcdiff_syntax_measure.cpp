#include <srcdiff_syntax_measure.hpp>

#include <srcdiff_compare.hpp>
#include <srcdiff_constants.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_diff.hpp>

#include <algorithm>

srcdiff_syntax_measure::srcdiff_syntax_measure(const construct & set_original, const construct & set_modified) 
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

  return !node->is_text() && (xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT
    && node->name != "operator" && node->name != "literal" && node->name != "modifier";

}

void srcdiff_syntax_measure::compute() {

  if(computed) return;

  computed = true;
  
  diff_nodes dnodes = { set_original.nodes(), set_modified.nodes() };

  if((xmlReaderTypes)set_original.term(0)->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)set_modified.term(0)->type != XML_READER_TYPE_ELEMENT
     || (srcdiff_compare::node_compare(set_original.term(0), set_modified.term(0)) != 0
        && !srcdiff_match::is_interchangeable_match(set_original, set_modified)
        && (set_original.term(0)->name != "block" || set_modified.term(0)->name != "block"))) {

    a_similarity = 0;
    a_original_difference = MAX_INT;
    a_modified_difference = MAX_INT;

    return;

  }

  // collect subset of nodes
  construct_list next_construct_list_original = set_original.size() > 1 ? construct_list(set_original.nodes(), set_original.get_terms().at(1), set_original.end_position(), is_significant) : construct_list(set_original.nodes());
  construct_list next_construct_list_modified = set_modified.size() > 1 ? construct_list(set_modified.nodes(), set_modified.get_terms().at(1), set_modified.end_position(), is_significant) : construct_list(set_modified.nodes());
  original_len = next_construct_list_original.size();
  modified_len = next_construct_list_modified.size();

  shortest_edit_script_t ses(srcdiff_compare::element_syntax_compare, srcdiff_compare::element_array_index, &dnodes);
  ses.compute<construct_list>(next_construct_list_original, next_construct_list_modified, false);
  process_edit_script(ses.script());

}
