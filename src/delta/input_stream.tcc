 // SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_stream.tcc
 *
 * @copyright Copyright (C) 2015-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

template<class T>
input_stream<T>::input_stream(const T& input, const std::optional<std::string>& path,
                              srcml_archive* archive,
                              const char* language, const std::optional<std::string>& filename)
	: input(input), input_stream_base(path), archive(archive), language(language), filename(filename) {
  }

template<class T>
input_stream<T>::~input_stream() {}

template<class T>
void input_stream<T>::operator()(srcML::converter& converter, srcML::nodes& nodes) const {
 if(!path || path->empty()) return;

  try {
    nodes = input_nodes(converter);
  } catch(...) {
    /// @todo handle this more gracefully...
    fprintf(stderr, "Error with files\n");
    exit(1);
  }

}

template<class T>
srcML::nodes input_stream<T>::input_nodes(srcML::converter& converter) const {

  typename T::input_context* context = input.open(path->c_str());
  converter.convert(archive, language, filename? filename->c_str() : std::filesystem::path(*path).filename(), (void*)context, T::read, T::close);
  return converter.create_nodes();

}
