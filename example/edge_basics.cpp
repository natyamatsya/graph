//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <boost/config.hpp>
#include <iostream>
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>

/*
  Edge Basics

  This example demonstrates the GGCL Edge interface

  There is not much to the Edge interface. Basically just two
  functions to access the source and target vertex:
  
  boost::source(e)
  boost::target(e)

  and one associated type for the vertex type:

  edge_traits<Edge>::vertex_type

  Sample output:

  (0,1) (0,2) (0,3) (0,4) (2,0) (2,4) (3,0) (3,1) 

 */



template <class Graph>
struct exercise_edge {
  exercise_edge(Graph& g) : G(g) {}

  using Edge = typename boost::graph_traits<Graph>::edge_descriptor;
  using Vertex = typename boost::graph_traits<Graph>::vertex_descriptor;
  void operator()(Edge e) const
  {
    //begin
    // Get the associated vertex type out of the edge using the
    // edge_traits class
    // Use the boost::source() and boost::target() functions to access the vertices
    // that belong to Edge e
    auto src = boost::source(e, G);
    auto targ = boost::target(e, G);

    // print out the vertex id's just because 
    std::cout << "(" << src << "," << targ << ") ";
    //end
  }

  Graph& G;
};


int
main()
{
  using MyGraph = boost::adjacency_list<>;

  using Pair = std::pair<int,int>;
  Pair edge_array[] = { Pair(0,1), Pair(0,2), Pair(0,3), Pair(0,4), 
                         Pair(2,0), Pair(3,0), Pair(2,4), Pair(3,1) };

  // Construct a graph using the edge_array (passing in pointers
  // (iterators) to the beginning and end of the array), and
  // specifying the number of vertices as 5
  MyGraph G(5);
  for (const auto& edge : edge_array)
    boost::add_edge(edge.first, edge.second, G);

  // Use the STL for_each algorithm to "exercise" all of the edges in
  // the graph
  std::for_each(boost::edges(G).first, edges(G).second, exercise_edge<MyGraph>(G));
  std::cout << std::endl;
  return 0;
}
