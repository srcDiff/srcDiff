/**
 * @file operation.hpp
 *
 * srcDiff operations
 *
 * @author Michael John Decker, Ph.D. <mdecke@bgsu.edu>
 */


namespace srcdiff {

enum operation {
    DELETE,
    INSERT,
    COMMON,
    NEST,
    MOVE
};

}


#endif