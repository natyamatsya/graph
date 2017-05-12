// (C) Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0 (See accompanying file
// LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

//[tiernan_print_cycles
#include <iostream>

#include <boost/graph/directed_graph.hpp>
#include <boost/graph/tiernan_all_cycles.hpp>

#include "helper.hpp"

// The cycle_printer is a visitor that will print the path that comprises
// the cycle. Note that the back() vertex of the path is not the same as
// the front(). It is implicit in the listing of vertices that the back()
// vertex is connected to the front().
template <typename OutputStream>
struct cycle_printer
{
    cycle_printer(OutputStream& stream)
        : os(stream)
    { }

    template <typename Path, typename Graph>
    void cycle(const Path& p, const Graph& g)
    {
        // Get the property map containing the vertex indices
        // so we can print them.
        auto indices = boost::get(boost::vertex_index, g);

        // Iterate over path printing each vertex that forms the cycle.
        for(const auto& vertex : p) {
            os << boost::get(indices, vertex) << " ";
        }
        os << std::endl;
    }
    OutputStream& os;
};

// Declare the graph type and its vertex and edge types.
using Graph = boost::directed_graph<>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
using Edge = boost::graph_traits<Graph>::edge_descriptor;

int
main(int argc, char *argv[])
{
    // Create the graph and read it from standard input.
    Graph g;
    read_graph(g, std::cin);

    // Instantiate the visitor for printing cycles
    cycle_printer<std::ostream> vis(std::cout);

    // Use the Tiernan algorithm to visit all cycles, printing them
    // as they are found.
    tiernan_all_cycles(g, vis);

    return 0;
}
//]
