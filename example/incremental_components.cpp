//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Copyright 2009 Trustees of Indiana University.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek, Michael Hansen
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <iostream>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/pending/disjoint_sets.hpp>
#include "range_pair.hpp"

/*

  This example shows how to use the disjoint set data structure
  to compute the connected components of an undirected, changing
  graph.

  Sample output:

  An undirected graph:
  0 <--> 1 4 
  1 <--> 0 4 
  2 <--> 5 
  3 <--> 
  4 <--> 1 0 
  5 <--> 2 

  representative[0] = 1
  representative[1] = 1
  representative[2] = 5
  representative[3] = 3
  representative[4] = 1
  representative[5] = 5

  component 0 contains: 4 1 0 
  component 1 contains: 3 
  component 2 contains: 5 2 

 */

int main(int argc, char* argv[]) 
{
  using Graph = boost::adjacency_list <boost::vecS, boost::vecS, boost::undirectedS>;
  using Vertex = boost::graph_traits<Graph>::vertex_descriptor;
  using VertexIndex = boost::graph_traits<Graph>::vertices_size_type;

  const int VERTEX_COUNT = 6;
  Graph graph(VERTEX_COUNT);

  std::vector<VertexIndex> rank(boost::num_vertices(graph));
  std::vector<Vertex> parent(boost::num_vertices(graph));

  using Rank = VertexIndex*;
  using Parent = Vertex*;

  boost::disjoint_sets<Rank, Parent> ds(&rank[0], &parent[0]);

  initialize_incremental_components(graph, ds);
  incremental_components(graph, ds);

  auto [edge, flag] = boost::add_edge(0, 1, graph);
  ds.union_set(0,1);

  std::tie(edge, flag) = boost::add_edge(1, 4, graph);
  ds.union_set(1,4);

  std::tie(edge, flag) = boost::add_edge(4, 0, graph);
  ds.union_set(4,0);

  std::tie(edge, flag) = boost::add_edge(2, 5, graph);
  ds.union_set(2,5);
    
  std::cout << "An undirected graph:" << std::endl;
  boost::print_graph(graph, boost::get(boost::vertex_index, graph));
  std::cout << std::endl;
    
  for ( auto current_vertex : make_range_pair(vertices(graph)) ) {
    std::cout << "representative[" << current_vertex << "] = " <<
      ds.find_set(current_vertex) << std::endl;
  }

  std::cout << std::endl;

  using Components = boost::component_index<VertexIndex>;

  // NOTE: Because we're using vecS for the graph type, we're
  // effectively using identity_property_map for a vertex index map.
  // If we were to use boost::listS instead, the index map would need to be
  // explicitly passed to the component_index constructor.
  Components components(parent.begin(), parent.end());

  // Iterate through the component indices
  for (auto current_index : components) {
    std::cout << "component " << current_index << " contains: ";

    // Iterate through the child vertex indices for [current_index]
    for (auto child_index : make_range_pair(components[current_index])) {
      std::cout << child_index << " ";
    }

    std::cout << std::endl;
  }

  return (0);
}

