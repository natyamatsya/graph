// (C) Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0 (See accompanying file
// LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)


//[eccentricity_example
#include <iostream>
#include <iomanip>

#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/eccentricity.hpp>
#include "helper.hpp"
#include "range_pair.hpp"

using namespace std;
using namespace boost;

// The Actor type stores the name of each vertex in the graph.
struct Actor
{
    string name;
};

// Declare the graph type and its vertex and edge types.
using Graph = undirected_graph<Actor>;
using Vertex = graph_traits<Graph>::vertex_descriptor;
using Edge = graph_traits<Graph>::edge_descriptor;

// The name map provides an abstract accessor for the names of
// each vertex. This is used during graph creation.
using NameMap = property_map<Graph, string Actor::*>::type;

// Declare a matrix type and its corresponding property map that
// will contain the distances between each pair of vertices.
using DistanceProperty = exterior_vertex_property<Graph, int>;
using DistanceMatrix = DistanceProperty::matrix_type;
using DistanceMatrixMap = DistanceProperty::matrix_map_type;

// Declare the weight map so that each edge returns the same value.
using WeightMap = constant_property_map<Edge, int>;

// Declare a container and its corresponding property map that
// will contain the resulting eccentricities of each vertex in
// the graph.
using EccentricityProperty = boost::exterior_vertex_property<Graph, int>;
using EccentricityContainer = EccentricityProperty::container_type;
using EccentricityMap = EccentricityProperty::map_type;

int
main(int argc, char *argv[])
{
    // Create the graph and a name map that provides access to
    // then actor names.
    Graph g;
    NameMap nm(get(&Actor::name, g));

    // Read the graph from standard input.
    read_graph(g, nm, cin);

    // Compute the distances between all pairs of vertices using
    // the Floyd-Warshall algorithm. Note that the weight map is
    // created so that every edge has a weight of 1.
    DistanceMatrix distances(num_vertices(g));
    DistanceMatrixMap dm(distances, g);
    WeightMap wm(1);
    floyd_warshall_all_pairs_shortest_paths(g, dm, weight_map(wm));

    // Compute the eccentricities for graph - this computation returns
    // both the radius and diameter as well.
    int r, d;
    EccentricityContainer eccs(num_vertices(g));
    EccentricityMap em(eccs, g);
    boost::tie(r, d) = all_eccentricities(g, dm, em);

    // Print the closeness centrality of each vertex.
    for(const auto& vertex : make_range_pair(vertices(g))) {
        cout << setw(12) << setiosflags(ios::left)
                << g[vertex].name << get(em, vertex) << endl;
    }
    cout << "radius: " << r << endl;
    cout << "diamter: " << d << endl;

    return 0;
}
//]
