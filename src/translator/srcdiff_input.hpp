#ifndef INCLUDED_SRCDIFF_INPUT_HPP
#define INCLUDED_SRCDIFF_INPUT_HPP

#include <thread>

class srcdiff_input {

protected:

  std::mutext mutex;

private:

public:

	srcdiff_input();
	~srcdiff_input();

};

#endif