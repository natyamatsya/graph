//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//
//  Sample output:
//
//   0  --(8, 10)--> 1  
//
//   1  --(12, 20)--> 4 --(12, 40)--> 3 
//        <--(8,10)-- 0 <--(16,20)-- 2  
//   2  --(16, 20)--> 1 
//        <--(16,20)-- 5        
//   3  --(12, 40)--> 6 
//        <--(12,40)-- 1        
//   4  --(12, 20)--> 7 
//        <--(12,20)-- 1        
//   5  --(16, 20)--> 2 
//        <--(16,20)-- 6        
//   6  --(16, 20)--> 5 --(8, 10)--> 8  
//        <--(12,20)-- 7        <--(12,40)-- 3  
//   7  --(12, 20)--> 6 
//        <--(12,20)-- 4        
//   8  
//        <--(8,10)-- 6 
//
//
//   0  --(8, 1)--> 1   
//
//   1  --(12, 2)--> 4  --(12, 3)--> 3  
//        <--(8,1)-- 0  <--(16,4)-- 2   
//   2  --(16, 4)--> 1  
//        <--(16,7)-- 5 
//   3  --(12, 5)--> 6  
//        <--(12,3)-- 1 
//   4  --(12, 6)--> 7  
//        <--(12,2)-- 1 
//   5  --(16, 7)--> 2  
//        <--(16,8)-- 6 
//   6  --(16, 8)--> 5  
//        <--(12,10)-- 7        <--(12,5)-- 3   
//   7  --(12, 10)--> 6 
//        <--(12,6)-- 4 
//   8  
//
        
#include <boost/config.hpp>
#include <iostream>

#include <boost/utility.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "range_pair.hpp"


using namespace boost;


enum edge_myflow_t { edge_myflow };
enum edge_mycapacity_t { edge_mycapacity };

namespace boost {
  BOOST_INSTALL_PROPERTY(edge, myflow);
  BOOST_INSTALL_PROPERTY(edge, mycapacity);
}


template <class Graph>
void print_network(const Graph& G)
{
  auto capacity = get(edge_mycapacity, G);
  auto flow = get(edge_myflow, G);

  for (const auto& vertex : make_range_pair(vertices(G))) {
    std::cout << vertex << "\t";

    for (const auto& edge : make_range_pair(out_edges(vertex, G)))
      std::cout << "--(" << capacity[edge] << ", " << flow[edge] << ")--> "
           << target(edge,G) << "\t";

    std::cout << std::endl << "\t";
    for (const auto& edge : make_range_pair(in_edges(vertex, G)))
      std::cout << "<--(" << capacity[edge] << "," << flow[edge] << ")-- "
           << source(edge, G) << "\t";

    std::cout << std::endl;
  }
}


int main(int , char* [])
{
  using Cap = property<edge_mycapacity_t, int>;
  using Flow = property<edge_myflow_t, int, Cap>;
  using Graph = adjacency_list<vecS, vecS, bidirectionalS, 
     no_property, Flow>;

  const int num_vertices = 9;
  Graph G(num_vertices);

  /*          2<----5 
             /       ^
            /         \
           V           \ 
    0 ---->1---->3----->6--->8
           \           ^
            \         /
             V       /
             4----->7
   */

  add_edge(0, 1, Flow(10, Cap(8)), G);

  add_edge(1, 4, Flow(20, Cap(12)), G);
  add_edge(4, 7, Flow(20, Cap(12)), G);
  add_edge(7, 6, Flow(20, Cap(12)), G);

  add_edge(1, 3, Flow(40, Cap(12)), G);
  add_edge(3, 6, Flow(40, Cap(12)), G);

  add_edge(6, 5, Flow(20, Cap(16)), G);
  add_edge(5, 2, Flow(20, Cap(16)), G);
  add_edge(2, 1, Flow(20, Cap(16)), G);

  add_edge(6, 8, Flow(10, Cap(8)), G);

  print_network(G);

  auto flow = get(edge_myflow, G);

  int f = 0;
  for (const auto& vertex : make_range_pair(vertices(G)))
    for (const auto& edge : make_range_pair(out_edges(vertex, G)))
      flow[edge] = ++f;
  std::cout << std::endl << std::endl;

  remove_edge(6, 8, G);

  print_network(G);

          
  return 0;
}
