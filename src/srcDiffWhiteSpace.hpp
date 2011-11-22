#ifndef INCLUDED_SRCDIFF_WHITESPACE_HPP
#define INCLUDED_SRCDIFF_WHITESPACE_HPP

#include "srcDiffTypes.hpp"

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
                                  , writer_state & wstate);

void output_white_space_pure_statement_end(reader_state & rbuf_old
                                           , reader_state & rbuf_new
                                           , int operation
                                           , writer_state & wstate);


void output_white_space_match_most(reader_state & rbuf_old
                                   , reader_state & rbuf_new
                                   , int operation
                                   , writer_state & wstate);

void output_white_space_prefix(reader_state & rbuf_old
                               , reader_state & rbuf_new
                               , writer_state & wstate);

void output_white_space_suffix(reader_state & rbuf_old
                               , reader_state & rbuf_new
                               , writer_state & wstate);

void advance_white_space_suffix(reader_state & rbuf_old
                                , int & end_old
                                , reader_state & rbuf_new
                                , int & end_new){

#endif
