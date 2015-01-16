#ifndef INCLUDED_SRCDIFF_MATCH_HPP
#define INCLUDED_SRCDIFF_MATCH_HPP

#include <node_sets.hpp>

struct offset_pair {

  int old_offset;
  int old_length;
  int new_offset;
  int new_length;
  int similarity;
  offset_pair * next;

};

class srcdiff_match {

protected:

  const srcml_nodes & nodes_original;
  const srcml_nodes & nodes_modified;
  const node_sets & node_sets_original;
  const node_sets & node_sets_modified;

private:

public:

  srcdiff_match(const srcml_nodes & nodes_original, const srcml_nodes & nodes_modified, const node_sets & node_sets_original, const node_sets & node_sets_modified);
  offset_pair * match_differences();

  static bool reject_match(int similarity, int difference, int text_original_length, int text_modified_length,
    const srcml_nodes & nodes_original, const node_set & set_original, const srcml_nodes & nodes_modified, const node_set & set_modified);
  static bool reject_similarity(int similarity, int difference, int text_original_length, int text_modified_length,
    const srcml_nodes & nodes_original, const node_set & set_original, const srcml_nodes & nodes_modified, const node_set & set_modified);


  static const char * find_attribute(const std::shared_ptr<srcml_node> & node, const char * attr_name);

  static bool is_interchangeable_match(const boost::optional<std::string> & old_tag, const boost::optional<std::string> & new_tag);

};



#endif
