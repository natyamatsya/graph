//=======================================================================
// Copyright 2001 Indiana University.
// Author: Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iostream>

int
main()
{
  using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, 
    boost::property<int, boost::edge_weight_t>, boost::no_property, boost::vecS>;

  const std::size_t n = 3;
  using E = std::pair<std::size_t, std::size_t>;
  E edge_array[] = { E(0,1), E(0,2), E(0,1) };
  const std::size_t m = sizeof(edge_array) / sizeof(E);
  Graph g(edge_array, edge_array + m, n);

  for (std::size_t i = 0; i < m; ++i) {
    const auto e = boost::edges(g).first + i;
    std::cout << *e << " ";
  }
  std::cout << std::endl;
  
  return 0;
}
