 // SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_stream.tcc
 *
 * @copyright Copyright (C) 2015-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

template<class T>
input_stream<T>::input_stream(const T& input, const std::optional<std::string>& input_path,
                              srcml_archive* archive,
                              const char* language_string)
	: input(input), input_path(input_path), archive(archive), language_string(language_string) {
  }

template<class T>
input_stream<T>::~input_stream() {}

template<class T>
void input_stream<T>::operator()(srcml_converter& converter, srcml_nodes& nodes) const {
 if(!input_path || input_path->empty()) return;

  try {
    nodes = input_nodes(converter);
  } catch(...) {
    /// @todo handle this more gracefully...
    fprintf(stderr, "Error with files\n");
    exit(1);
  }

}

template<class T>
srcml_nodes input_stream<T>::input_nodes(srcml_converter& converter) const {

  typename T::input_context* context = input.open(input_path->c_str());

  converter.convert(archive, language_string, (void*)context, T::read, T::close);
  return converter.create_nodes();

}
