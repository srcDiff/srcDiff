#ifndef INCLUDED_CLIENT_OPTIONS_HPP
#define INCLUDED_CLIENT_OPTIONS_HPP

#include <srcdiff_uri.hpp>
#include <methods.hpp>
#include <srcml.h>

#include <boost/optional.hpp>

struct srcdiff_options
{

  srcml_archive * archive;

  int language;

  boost::optional<std::string> srcdiff_filename;
  boost::optional<std::string> files_from_name;
  boost::optional<std::string> css_url;
  boost::optional<std::string> svn_url;

  OPTION_TYPE flags;
  METHOD_TYPE methods;

  int revision_one;
  int revision_two;

  int number_context_lines;
  
};

srcdiff_options process_command_line(int argc, char* argv[]);

int option_error_status(int optopt);

void output_help(const char* name);

void output_version(const char* name);

#endif
