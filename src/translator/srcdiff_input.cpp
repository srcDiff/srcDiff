#include <srcdiff_input.hpp>

srcdiff_input::srcdiff_input(srcml_archive * archive) {}

srcdiff_input~srcdiff_input() {}

std::vector<xNodePtr> srcdiff_input::input_nodes(const char * input_path, int stream_source) {

  srcml_translator translator(archive, stream_source);

  translator.translate(input_path, options);

  return translator.create_nodes();



#if 0
  /*

    Input for file two

  */


  int is_new = 0;
  create_nodes_args args_new = { path_two, archive
                                , srcml_archive_check_extension(archive, path_one ? path_one : path_two)
                                , unit_filename
                                , unit_directory
                                , unit_version
                                , mutex
                                , output.get_rbuf_new().nodes
                                , is_new
                                , output.get_rbuf_new().stream_source
                                , options };


  pthread_t thread_new;
  if(pthread_create(&thread_new, NULL, create_nodes_from_srcML_thread, (void *)&args_new)) {

    is_new = -2;

  }

  if(isoption(options, OPTION_THREAD) && is_old != -2 && pthread_join(thread_old, NULL)) {

    is_old = -2;

  }

  if(is_new != -2 && pthread_join(thread_new, NULL)) {

    is_new = -2;

  }

  node_sets set_old(output.get_rbuf_old().nodes);
  node_sets set_new(output.get_rbuf_new().nodes);

  if(is_old && is_old > -1)
    set_old = node_sets(output.get_rbuf_old().nodes, 0, output.get_rbuf_old().nodes.size());


  if(is_new && is_new > -1)
    set_new = node_sets(output.get_rbuf_new().nodes, 0, output.get_rbuf_new().nodes.size());
#endif
}