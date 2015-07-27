#include <srcml_converter.hpp>

class no_file_exception {};

template<class T>
srcdiff_input<T>::srcdiff_input(srcml_archive * archive, const boost::optional<std::string> & input_path, const char * language_string, const OPTION_TYPE & options, const T & input)
	: archive(archive), input_path(input_path), language_string(language_string), options(options), input(input) {}

template<class T>
srcdiff_input<T>::~srcdiff_input() {}

template<class T>
void srcdiff_input<T>::operator()(int stream_source, srcml_nodes & nodes, int & is_input, const boost::optional<std::string> & output_path) const {

  is_input = 0;
  try {

    nodes = input_nodes(stream_source, output_path);
    is_input = 1;

  } catch(no_file_exception) {}
  catch(...) {

    is_input = -2;

  }

}

template<class T>
srcml_nodes srcdiff_input<T>::input_nodes(int stream_source, const boost::optional<std::string> & output_path) const {

  if(!input_path || input_path->empty()) throw no_file_exception();

  srcml_converter converter(archive, stream_source);

  typename T::input_context * context = input.open(input_path->c_str());

  converter.convert(language_string, (void *)context, T::read, T::close, output_path);

  return converter.create_nodes();

}
