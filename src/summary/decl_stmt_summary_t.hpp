#ifndef INCLUDED_DECL_STMT_SUMMARY_T_HPP
#define INCLUDED_DECL_STMT_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class decl_stmt_summary_t : public summary_t {

    private:

    	bool type_modified;
    	bool name_modified;
    	bool init_modified;

    public:

        decl_stmt_summary_t(srcdiff_type operation,
				        	bool type_modified,	bool name_modified,	bool init_modified)
            : summary_t(DECL_STMT, operation),
              type_modified(type_modified), name_modified(name_modified), init_modified(init_modified) {}

};

#endif
