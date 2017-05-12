//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//
//  Sample output
//
// DFS categorized directed graph
// tree: 0 --> 2
// tree: 2 --> 1
// back: 1 --> 1
// tree: 1 --> 3
// back: 3 --> 1
// tree: 3 --> 4
// back: 4 --> 0
// back: 4 --> 1
// forward or cross: 2 --> 3

// BFS categorized directed graph
// tree: 0 --> 2
// tree: 2 --> 1
// tree: 2 --> 3
// cycle: 1 --> 1
// cycle: 1 --> 3
// cycle: 3 --> 1
// tree: 3 --> 4
// cycle: 4 --> 0
// cycle: 4 --> 1

#include <boost/config.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>

#include <boost/graph/visitors.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/depth_first_search.hpp>


template <class Tag>
struct edge_printer : public boost::base_visitor<edge_printer<Tag>> {
  using event_filter = Tag;
  edge_printer(std::string edge_t) : m_edge_type(edge_t) { }
  template <class Edge, class Graph>
  void operator()(Edge e, Graph& G) {
    std::cout << m_edge_type << ": " << boost::source(e, G) 
              << " --> " <<  boost::target(e, G) << std::endl;
  }
  std::string m_edge_type;
};
template <class Tag>
edge_printer<Tag> print_edge(std::string type, Tag) { 
  return edge_printer<Tag>(type);
}

int 
main(int, char*[])
{

  
  using Graph = boost::adjacency_list<>;
  using E = std::pair<int,int>;
  const auto edges = { E(0, 2),
                E(1, 1), E(1, 3),
                E(2, 1), E(2, 3),
                E(3, 1), E(3, 4),
                E(4, 0), E(4, 1) };  
  Graph G(std::begin(edges), std::end(edges), 5);

  using size_type = boost::graph_traits<Graph>::vertices_size_type;
  
  std::vector<size_type> d(boost::num_vertices(G));  
  std::vector<size_type> f(boost::num_vertices(G));

  std::cout << "DFS categorized directed graph" << std::endl;
  boost::depth_first_search(G, boost::visitor(boost::make_dfs_visitor(
      make_list(print_edge("tree", boost::on_tree_edge()),
                print_edge("back", boost::on_back_edge()),
                print_edge("forward or cross", boost::on_forward_or_cross_edge())
                ))));

  std::cout << std::endl << "BFS categorized directed graph" << std::endl;
  boost::breadth_first_search
    (G, vertex(0, G), boost::visitor(boost::make_bfs_visitor(
     std::make_pair(print_edge("tree", boost::on_tree_edge()),
                    print_edge("cycle", boost::on_non_tree_edge())))));

  return 0;
}

