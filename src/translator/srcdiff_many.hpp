#ifndef INCLUDED_SRCDIFFMANY_HPP
#define INCLUDED_SRCDIFFMANY_HPP

#include <srcDiffTypes.hpp>
#include <shortest_edit_script.h>
#include <xmlrw.hpp>
#include <vector>

#include <srcdiff_diff.hpp>

class srcdiff_many : public srcdiff_diff {

protected:
  edit * edit_script;

private:

  typedef std::vector<IntPairs> Moves;

  void output_unmatched(int start_old, int end_old, int start_new, int end_new);

  Moves determine_operations();
  
public:
  srcdiff_many(const srcdiff_diff & diff, edit * edit_script);

  virtual void output();

};

#endif
