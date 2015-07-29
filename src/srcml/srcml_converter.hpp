#ifndef INCLUDED_SRCML_CONVERTER_HPP
#define INCLUDED_SRCML_CONVERTER_HPP

#include <srcml_nodes.hpp>
#include <srcdiff_options.hpp>

#include <srcml.h>

#include <vector>
#include <map>
#include <mutex>
#include <functional>
#include <memory>

class srcml_converter {

protected:

  srcml_archive * archive;

  int stream_source;

  char * output_buffer;
  size_t output_size;

  static std::mutex mutex;

  static std::map<std::string, std::shared_ptr<srcml_node>> start_tags;
  static std::map<std::string, std::shared_ptr<srcml_node>> end_tags;

private:

	srcml_nodes collect_nodes(xmlTextReaderPtr reader) const;

	static std::shared_ptr<srcml_node> get_current_node(xmlTextReaderPtr reader, const OPTION_TYPE & options);

public:

  struct srcml_burst_config {

    const boost::optional<std::string> & output_path;
    const std::string & language;
    const boost::optional<std::string> & unit_filename;
    const boost::optional<std::string> & unit_version;

  };

 	srcml_converter(srcml_archive * archive, int stream_source);
 	~srcml_converter();

	void convert(const std::string & language, void * context, const std::function<int(void *, char *, size_t)> & read, const std::function<int(void *)> & close, const srcml_burst_config & burst_config); 
	srcml_nodes create_nodes() const;

};

#endif
