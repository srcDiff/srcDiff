#include "srcDiffWhiteSpace.hpp"

void output_white_space_nested(reader_state & rbuf_old
                               , reader_state & rbuf_new
                               , int operation, writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end
  for(; oend < nodes_old.size() && nend < nodes_new.size()
        && is_white_space(nodes_old.at(oend)) && is_white_space(nodes_new.at(nend))
        ; ++oend)
    ;

  if(operation == DELETE)
    for(; oend < nodes_old.size() && is_white_space(nodes_old.at(oend)); ++oend) {
    }
  else
    for(; nend < nodes_new.size() && is_white_space(nodes_new.at(nend)); ++nend) {
    }


  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}


void output_white_space_statement(reader_state & rbuf_old, reader_state & rbuf_new, writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end
  for(; oend < nodes_old.size() && is_white_space(nodes_old.at(oend)) && !is_new_line(nodes_old.at(oend)); ++oend)
    ;

  for(; nend < nodes_new.size() && is_white_space(nodes_new.at(nend)) && !is_new_line(nodes_new.at(nend)); ++nend)
    ;

  if(oend < nodes_old.size() && is_new_line(nodes_old.at(oend)))
    ++oend;

  if(nend < nodes_new.size() && is_new_line(nodes_new.at(nend)))
    ++nend;

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}

void output_white_space_all(reader_state & rbuf_old
                            , reader_state & rbuf_new
                            , writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end
  for(; oend < nodes_old.size() && is_white_space(nodes_old.at(oend)); ++oend)
    ;

  for(; nend < nodes_new.size() && is_white_space(nodes_new.at(nend)); ++nend)
    ;

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}

void output_white_space_pure_statement_end(reader_state & rbuf_old
                                           , reader_state & rbuf_new
                                           , int operation
                                           , writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end
  if(operation == DELETE) {
    if(oend < nodes_old.size() && is_new_line(nodes_old.at(oend)))
      ++oend; 
    }
  else if(operation == INSERT)
    if(nend < nodes_new.size() && is_new_line(nodes_new.at(nend)))
      ++nend;

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}

void output_white_space_match_most(reader_state & rbuf_old
                                   , reader_state & rbuf_new
                                   , int operation
                                   , writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end

  for(; oend < nodes_old.size() && nend < nodes_new.size()
        && is_white_space(nodes_old.at(oend)) && is_white_space(nodes_new.at(nend))
        && node_compare(nodes_old.at(oend), nodes_new.at(nend)) == 0; ++oend, ++nend)
    ;

  if(rbuf_old.last_output < oend && (is_white_space(nodes_old.at(oend - 1)) && !is_new_line(nodes_old.at(oend - 1))))
    --oend;

  if(rbuf_new.last_output < nend && (is_white_space(nodes_new.at(nend - 1)) && !is_new_line(nodes_new.at(nend - 1))))
    --nend;

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}

void output_white_space_most(reader_state & rbuf_old
                             , reader_state & rbuf_new
                             , writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end
  for(; oend < nodes_old.size() && is_white_space(nodes_old.at(oend)); ++oend)
    ;

  for(; nend < nodes_new.size() && is_white_space(nodes_new.at(nend)); ++nend)
    ;

  if(rbuf_old.last_output < oend && (is_white_space(nodes_old.at(oend - 1)) && !is_new_line(nodes_old.at(oend - 1))))
    --oend;

  if(rbuf_new.last_output < nend && (is_white_space(nodes_new.at(nend - 1)) && !is_new_line(nodes_new.at(nend - 1))))
    --nend;

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}
