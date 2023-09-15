#include <srcdiff_syntax_measure.hpp>

#include <srcdiff_shortest_edit_script.hpp>
#include <srcdiff_constants.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_diff.hpp>

#include <algorithm>

srcdiff_syntax_measure::srcdiff_syntax_measure(const construct & set_original, const construct & set_modified) 
  : srcdiff_measure(set_original, set_modified) {}


static bool is_significant(int & node_pos, const srcml_nodes & nodes, const void * context) {

  const std::shared_ptr<srcml_node> & node = nodes[node_pos];

  if(node->get_name() == "argument_list") {

    int pos = node_pos + 1;
    while(pos < nodes.size() && (nodes[pos]->get_type() == srcml_node::srcml_node_type::START || nodes[pos]->get_name() != "argument_list")) {

      if(!nodes[pos]->is_text()) return true;

      ++pos;

    }

    node_pos = pos;

    return false;

  }

  return !node->is_text() && node->get_type() == srcml_node::srcml_node_type::START
    && node->get_name() != "operator" && node->get_name() != "literal" && node->get_name() != "modifier";

}

void srcdiff_syntax_measure::compute() {

  if(computed) return;

  computed = true;
  
  if(set_original.term(0)->get_type() != srcml_node::srcml_node_type::START
     || set_modified.term(0)->get_type() != srcml_node::srcml_node_type::START
     || (*set_original.term(0) != *set_modified.term(0)
        && !set_original.is_tag_convertable(set_modified)
        && (set_original.term(0)->get_name() != "block" || set_modified.term(0)->get_name() != "block"))) {

    a_similarity = 0;
    a_original_difference = MAX_INT;
    a_modified_difference = MAX_INT;

    return;

  }

  // collect subset of nodes
  construct::construct_list next_construct_list_original = set_original.size() > 1 ? construct::get_descendent_constructs(set_original.nodes(), set_original.get_terms().at(1), set_original.end_position(), is_significant) : construct::construct_list();
  construct::construct_list next_construct_list_modified = set_modified.size() > 1 ? construct::get_descendent_constructs(set_modified.nodes(), set_modified.get_terms().at(1), set_modified.end_position(), is_significant) : construct::construct_list();
  original_len = next_construct_list_original.size();
  modified_len = next_construct_list_modified.size();

  srcdiff_shortest_edit_script ses;
  ses.compute_edit_script(next_construct_list_original, next_construct_list_modified);
  process_edit_script(ses.script());

}
