#include <test_utils.hpp>

#include <fstream>
#include <string>
#include <sstream>
#include <node_set.hpp>
#include <srcml_converter.hpp>
#include <iostream>
#include <cstdio>


ssize_t str_read(void * context, void * buffer, size_t len) {

	std::string * ctx = static_cast<std::string *>(context);
	size_t num_read = ctx->copy((char*)buffer, len, 0);
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

std::shared_ptr<srcml_nodes> create_nodes(const std::string & code, const std::string & language) {

    //create srcml archive pointer and get code string      
	srcml_archive * archive = srcml_archive_create();
	srcml_archive_enable_solitary_unit(archive);
	srcml_archive_disable_hash(archive);
	srcml_archive_register_namespace(archive, "diff", "http://www.srcML.org/srcDiff");

    //Create burst_config object
	const srcml_converter::srcml_burst_config burst_config = {
	       	std::optional<std::string>(),
	       	"",
	       	std::optional<std::string>(),
	       	std::optional<std::string>()
       	};

    //create srcml_nodes
	srcml_converter contNodes(archive, true, 0);
	std::string source = code;
	contNodes.convert(language, (void*)&source, &str_read, &str_close, burst_config);
	srcml_nodes testNode = contNodes.create_nodes();

	return std::make_shared<srcml_nodes>(testNode);
}

node_set_data create_node_set(const std::string & code, const std::string & language) {

	std::shared_ptr<srcml_nodes> nodes = create_nodes(code, language);
	int pos = 0;

	node_set_data set = { nodes, node_set(*nodes, pos) };
	return set;
}
