#ifndef INCLUDED_SRCDIFF_MATCH_HPP
#define INCLUDED_SRCDIFF_MATCH_HPP

#include <node_sets.hpp>

struct offset_pair {

  int original_offset;
  int original_length;
  int modified_offset;
  int modified_length;
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

  static bool is_interchangeable_match(const std::string & original_tag, const std::string & original_uri,
                                       const std::string & modified_tag, const std::string & modified_uri);

};



#endif
