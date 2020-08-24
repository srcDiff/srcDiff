#ifndef src_to_nodes
#define src_to_nodes


struct p2test{
        node_set nsone;
        node_set nstwo;
        srcdiff_text_measure txtmeasure;
};


int str_read(void *, char *, unsigned long);
int str_close(void *);
std::string read_from_file(std::string);
srcml_nodes create_nodes(const std::string &, const std::string &);
srcml_nodes create_nodes_file(const std::string &, const std::string &);
p2test ret_node_set(const std::string &,const std::string &, const std::string &);
p2test ret_node_set_code(const std::string &,const std::string &, const std::string &);

#endif
