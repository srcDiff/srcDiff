#ifndef INCLUDED_SRCDIFF_SYNTAX_MEASURE_HPP
#define INCLUDED_SRCDIFF_SYNTAX_MEASURE_HPP

#include <srcdiff_measure.hpp>

class srcdiff_syntax_measure : public srcdiff_measure{

protected:

private:

public:

	srcdiff_syntax_measure(const srcml_nodes & nodes_original, const srcml_nodes & nodes_modified, const node_set & set_original, const node_set & set_modified);

	virtual void compute();

};





#endif
