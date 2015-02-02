#include <srcdiff_input_source.hpp>

void srcdiff_input_source::file(const boost::optional<std::string> & path_one, const void * context_original,
          const boost::optional<std::string> & path_two, const void * context_modified) {

  process_file(path_one, context_original, path_two, context_modified);

}

void srcdiff_input_source::directory(const boost::optional<std::string> & directory_original, const void * context_original,
               const boost::optional<std::string> & directory_modified, const void * context_modified) {

  process_directory(directory_original, context_original, directory_modified, context_modified);

}
