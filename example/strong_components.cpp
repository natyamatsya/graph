//=======================================================================
// Copyright 1997-2001 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_utility.hpp>
/*
  Sample output:
  A directed graph:
  a --> b f h 
  b --> c a 
  c --> d b 
  d --> e 
  e --> d 
  f --> g 
  g --> f d 
  h --> i 
  i --> h j e c 
  j --> 

  Total number of components: 4
  Vertex a is in component 3
  Vertex b is in component 3
  Vertex c is in component 3
  Vertex d is in component 0
  Vertex e is in component 0
  Vertex f is in component 1
  Vertex g is in component 1
  Vertex h is in component 3
  Vertex i is in component 3
  Vertex j is in component 2
 */

int main(int, char*[])
{
  const char* name = "abcdefghij";

  boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> G;
  boost::dynamic_properties dp;
  boost::read_graphviz("scc.dot", G, dp);

  std::cout << "A directed graph:" << std::endl;
  boost::print_graph(G, name);
  std::cout << std::endl;

  using Vertex = boost::graph_traits<boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>>::vertex_descriptor;
    
  std::vector<int> component(boost::num_vertices(G)), discover_time(boost::num_vertices(G));
  std::vector<boost::default_color_type> color(boost::num_vertices(G));
  std::vector<Vertex> root(boost::num_vertices(G));
  auto num = strong_components(G, boost::make_iterator_property_map(component.begin(), boost::get(boost::vertex_index, G)), 
                              root_map(boost::make_iterator_property_map(root.begin(), boost::get(boost::vertex_index, G))).
                              color_map(boost::make_iterator_property_map(color.begin(), boost::get(boost::vertex_index, G))).
                              discover_time_map(boost::make_iterator_property_map(discover_time.begin(), boost::get(boost::vertex_index, G))));
    
  std::cout << "Total number of components: " << num << std::endl;
  std::vector<int>::size_type i;
  for (i = 0; i != component.size(); ++i)
    std::cout << "Vertex " << name[i]
         <<" is in component " << component[i] << std::endl;
    
  return 0;
}
