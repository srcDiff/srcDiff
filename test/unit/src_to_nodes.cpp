#include <fstream>
#include <string>
#include <sstream>
#include <node_set.hpp>
#include <srcml.h>
#include <srcml_nodes.hpp>
#include <srcdiff_constants.hpp>
#include <srcml_converter.hpp>

#include <iostream>
#include <cstdio>

struct p2test{
node_set one;
node_set two;
srcdiff_test_measure ameasure;
};




int str_read(void * context, char * buffer, unsigned long len) {
    std::string * ctx = static_cast<std::string *>(context);
    size_t num_read = ctx->copy(buffer, len, 0);
    ctx->erase(0, num_read);
    return num_read;
}

int str_close(void * context) {
    return 0;
}


std::string read_from_file(std::string filename){

    std::ifstream t(filename);
    std::string str((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
    return str;
}

srcml_nodes create_nodes(const std::string & filename, const std::string & language){

    //create srcml archive pointer and get code string      
    srcml_archive * archive = srcml_archive_create();
    srcml_archive_disable_full_archive(archive);
    srcml_archive_disable_hash(archive);
    srcml_archive_enable_option(archive, SRCML_OPTION_XML_DECL);
    srcml_archive_register_namespace(archive, "diff", SRCDIFF_DEFAULT_NAMESPACE_HREF.c_str());

    std::string source = read_from_file(filename);
    //Create burst_config object
    const srcml_converter::srcml_burst_config burst_config = { boost::optional<std::string>(), "", boost::optional<std::string>(), boost::optional<std::string>() };

    //create srcml_nodes
    srcml_converter contNodes(archive, true, 0);
    contNodes.convert(language, (void*)&source, &str_read, &str_close, burst_config);
    srcml_nodes testNode = contNodes.create_nodes();
    return testNode;
}



p2test ret_node_set(const std::string & filename1,const std::string & filename2, const std::string & language){
srcml_nodes test_nodes1 = test_nodes(filename, language);
srcml_nodes test_nodes2 = test_nodes(filename2,language);
node_set anode(test_nodes1);
node_set anode2(test_nodes2);
srcdiff_text_measure ameasure(anode, anode2);

p2test anobject = {
anode,
anode2,
ameasure,
};

return anobject;
}





/*
#include <iostream>

int main(int argc, char * argv[]) {

    srcml_nodes nodes = create_nodes(argv[1], "C++");

    for(const std::shared_ptr<srcml_node> & node : nodes) {
        std::cout << *node << '\n';
    }

    return 0;
}*/
