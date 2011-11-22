#ifndef INCLUDED_SRCDIFF_WHITESPACE_HPP
#define INCLUDED_SRCDIFF_WHITESPACE_HPP

struct reader_state;
struct writer_state;

void output_white_space_all(reader_state & rbuf_old
                            , reader_state & rbuf_new
                            , writer_state & wstate);

void output_white_space_prefix(reader_state & rbuf_old
                               , reader_state & rbuf_new
                               , writer_state & wstate);

void output_white_space_most(reader_state & rbuf_old
                             , reader_state & rbuf_new
                             , writer_state & wstate);

void output_white_space_nested(reader_state & rbuf_old
                               , reader_state & rbuf_new
                               , int operation, writer_state & wstate);

void output_white_space_statement(reader_state & rbuf_old
                                  , reader_state & rbuf_new
                                  , writer_state & wstate) {

void output_white_space_pure_statement_end(reader_state & rbuf_old
                                           , reader_state & rbuf_new
                                           , int operation
                                           , writer_state & wstate);


void output_white_space_match_most(reader_state & rbuf_old
                                   , reader_state & rbuf_new
                                   , int operation
                                   , writer_state & wstate);
#endif
