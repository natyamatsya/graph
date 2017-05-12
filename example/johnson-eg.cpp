//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee, 
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <boost/config.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include "range_pair.hpp"

int
main()
{
  using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property,
    boost::property<boost::edge_weight_t, int, boost::property<boost::edge_weight2_t, int>>>;
  const int V = 6;
  using Edge = std::pair<int, int>;
  Edge edge_array[] =
    { Edge(0, 1), Edge(0, 2), Edge(0, 3), Edge(0, 4), Edge(0, 5),
    Edge(1, 2), Edge(1, 5), Edge(1, 3), Edge(2, 4), Edge(2, 5),
    Edge(3, 2), Edge(4, 3), Edge(4, 1), Edge(5, 4)
  };
  const std::size_t E = sizeof(edge_array) / sizeof(Edge);
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
  // VC++ can't handle the iterator constructor
  Graph g(V);
  for (std::size_t j = 0; j < E; ++j)
    boost::add_edge(edge_array[j].first, edge_array[j].second, g);
#else
  Graph g(edge_array, edge_array + E, V);
#endif

  auto w = boost::get(boost::edge_weight, g);
  int weights[] = { 0, 0, 0, 0, 0, 3, -4, 8, 1, 7, 4, -5, 2, 6 };
  int *wp = weights;

  for (const auto& edge : make_range_pair(boost::edges(g)))
    w[edge] = *wp++;

  std::vector<int >d(V, (std::numeric_limits < int>::max)());
  int D[V][V];
  johnson_all_pairs_shortest_paths(g, D, boost::distance_map(&d[0]));

  std::cout << "       ";
  for (int k = 0; k < V; ++k)
    std::cout << std::setw(5) << k;
  std::cout << std::endl;
  for (int i = 0; i < V; ++i) {
    std::cout << std::setw(3) << i << " -> ";
    for (int j = 0; j < V; ++j) {
      if (D[i][j] == (std::numeric_limits<int>::max)())
        std::cout << std::setw(5) << "inf";
      else
        std::cout << std::setw(5) << D[i][j];
    }
    std::cout << std::endl;
  }

  std::ofstream fout("figs/johnson-eg.dot");
  fout << "digraph A {\n"
    << "  rankdir=LR\n"
    << "size=\"5,3\"\n"
    << "ratio=\"fill\"\n"
    << "edge[style=\"bold\"]\n" << "node[shape=\"circle\"]\n";

  for (const auto& edge : make_range_pair(boost::edges(g)))
    fout << boost::source(edge, g) << " -> " << boost::target(edge, g)
      << "[label=" << boost::get(boost::edge_weight, g)[edge] << "]\n";

  fout << "}\n";
  return 0;
}
