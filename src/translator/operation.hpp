/**
 * @file operation.hpp
 *
 * srcDiff operations
 *
 * @author Michael John Decker, Ph.D. <mdecke@bgsu.edu>
 */

#ifndef INCLUDED_OPERATION_HPP
#define INCLUDED_OPERATION_HPP


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