// (C) Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0 (See accompanying file
// LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

//[inclusive_mean_geodesic_example
#include <iostream>
#include <iomanip>

#include <boost/graph/directed_graph.hpp>
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/geodesic_distance.hpp>
#include "helper.hpp"

using namespace boost;

// This template structure defines the function that we will apply
// to compute both the per-vertex mean geodesic distances and the
// graph's mean geodesic distance.
template <typename Graph,
          typename DistanceType,
          typename ResultType,
          typename Divides = std::divides<ResultType>>
struct inclusive_average
{
    using distance_type = DistanceType;
    using result_type = ResultType;

    result_type operator ()(distance_type d, const Graph& g)
    {
        if(d == numeric_values<distance_type>::infinity()) {
            return numeric_values<result_type>::infinity();
        }
        else {
            return div(result_type(d), result_type(num_vertices(g)));
        }
    }
    Divides div;
};

// The Page type stores the name of each vertex in the graph and
// represents web pages that can be navigated to.
struct WebPage
{
    std::string name;
};

// The Link type stores an associated probability of traveling
// from one page to another.
struct Link
{
    float probability;
};

// Declare the graph type and its vertex and edge types.
using Graph = directed_graph<WebPage, Link>;
using Vertex = graph_traits<Graph>::vertex_descriptor;
using Edge = graph_traits<Graph>::edge_descriptor;

// The name map provides an abstract accessor for the names of
// each vertex. This is used during graph creation.
using NameMap = property_map<Graph, std::string WebPage::*>::type;

// Declare a matrix type and its corresponding property map that
// will contain the distances between each pair of vertices.
using DistanceProperty = exterior_vertex_property<Graph, float>;
using DistanceMatrix = DistanceProperty::matrix_type;
using DistanceMatrixMap = DistanceProperty::matrix_map_type;

// Declare the weight map as an accessor into the bundled
// edge property.
using WeightMap = property_map<Graph, float Link::*>::type;

// Declare a container and its corresponding property map that
// will contain the resulting mean geodesic distances of each
// vertex in the graph.
using GeodesicProperty = exterior_vertex_property<Graph, float>;
using GeodesicContainer = GeodesicProperty::container_type;
using GeodesicMap = GeodesicProperty::map_type;

static float exclusive_geodesics(const Graph&, DistanceMatrixMap, GeodesicMap);
static float inclusive_geodesics(const Graph&, DistanceMatrixMap, GeodesicMap);

int
main(int argc, char *argv[])
{
    // Create the graph, a name map that providse abstract access
    // to the web page names, and the weight map as an accessor to
    // the edge weights (or probabilities).
    Graph g;
    NameMap nm(get(&WebPage::name, g));
    WeightMap wm(get(&Link::probability, g));

    // Read the weighted graph from standard input.
    read_weighted_graph(g, nm, wm, std::cin);

    // Compute the distances between all pairs of vertices using
    // the Floyd-Warshall algorithm. The weight map was created
    // above so it could be populated when the graph was read in.
    DistanceMatrix distances(num_vertices(g));
    DistanceMatrixMap dm(distances, g);
    floyd_warshall_all_pairs_shortest_paths(g, dm, weight_map(wm));

    // Create the containers and the respective property maps that
    // will contain the mean geodesics averaged both including
    // self-loop distances and excluding them.
    GeodesicContainer exclude(num_vertices(g));
    GeodesicContainer include(num_vertices(g));
    GeodesicMap exmap(exclude, g);
    GeodesicMap inmap(include, g);

    float ex = exclusive_geodesics(g, dm, exmap);
    float in = inclusive_geodesics(g, dm, inmap);

    // Print the mean geodesic distance of each vertex and finally,
    // the graph itself.
    std::cout << std::setw(12) << std::setiosflags(std::ios::left) << "vertex";
    std::cout << std::setw(12) << std::setiosflags(std::ios::left) << "excluding";
    std::cout << std::setw(12) << std::setiosflags(std::ios::left) << "including" << std::endl;
    graph_traits<Graph>::vertex_iterator i, end;
    for(boost::tie(i, end) = vertices(g); i != end; ++i) {
          std::cout << std::setw(12) << std::setiosflags(std::ios::left)
             << g[*i].name
             << std::setw(12) << get(exmap, *i)
             << std::setw(12) << get(inmap, *i) << std::endl;
    }
    std::cout << "small world (excluding self-loops): " << ex << std::endl;
    std::cout << "small world (including self-loops): " << in << std::endl;

    return 0;
}

float
exclusive_geodesics(const Graph& g, DistanceMatrixMap dm, GeodesicMap gm)
{
    // Compute the mean geodesic distances, which excludes distances
    // of self-loops by default. Return the measure for the entire graph.
    return all_mean_geodesics(g, dm, gm);
}


float
inclusive_geodesics(const Graph &g, DistanceMatrixMap dm, GeodesicMap gm)
{
    // Create a new measure object for computing the mean geodesic
    // distance of all vertices. This measure will actually be used
    // for both averages.
    inclusive_average<Graph, float, float> m;

    // Compute the mean geodesic distance using the inclusive average
    // to account for self-loop distances. Return the measure for the
    // entire graph.
    return all_mean_geodesics(g, dm, gm, m);
}
//]
