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
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include "range_pair.hpp"

namespace std
{
  template <typename T>
    std::istream & operator >> (std::istream & in, std::pair<T, T> &p)
  {
    in >> p.first >> p.second;
    return in;
  }
}

using file_dep_graph = boost::adjacency_list<boost::listS, // Store out-edges of each vertex in a std::list
  boost::vecS,                         // Store vertex set in a std::vector
  boost::directedS                     // The file dependency graph is directed
>;

using vertex_t = boost::graph_traits<file_dep_graph>::vertex_descriptor;
using edge_t = boost::graph_traits<file_dep_graph>::edge_descriptor;

bool
has_cycle_dfs(const file_dep_graph & g, vertex_t u,
              boost::default_color_type * color)
{
  color[u] = boost::gray_color;
  for (const auto& vertex : make_range_pair(boost::adjacent_vertices(u, g)))
    if (color[vertex] == boost::white_color) {
      if (has_cycle_dfs(g, vertex, color))
        return true;            // cycle detected, return immediately
    } else if (color[vertex] == boost::gray_color)        // vertex is an ancestor!
      return true;
  color[u] = boost::black_color;
  return false;
}

bool
has_cycle(const file_dep_graph & g)
{
  std::vector<boost::default_color_type> color(boost::num_vertices(g), boost::white_color);
  for (const auto& vertex : make_range_pair(boost::vertices(g)))
    if (color[vertex] == boost::white_color)
      if (has_cycle_dfs(g, vertex, &color[0]))
        return true;
  return false;
}


int
main()
{
  std::ifstream file_in("makefile-dependencies.dat");
  using size_type = boost::graph_traits<file_dep_graph>::vertices_size_type;
  size_type n_vertices;
  file_in >> n_vertices;        // read in number of vertices
  std::istream_iterator<std::pair<size_type, size_type>> input_begin(file_in), input_end;
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
  // VC++ has trouble with the edge iterator constructor
  file_dep_graph g(n_vertices);
  while (input_begin != input_end) {
    const auto [i, j] = *input_begin++;
    boost::add_edge(i, j, g);
  }
#else
  file_dep_graph g(input_begin, input_end, n_vertices);
#endif

  std::vector<std::string> name(boost::num_vertices(g));
  std::ifstream name_in("makefile-target-names.dat");
  for (const auto& vertex : make_range_pair(boost::vertices(g)))
    name_in >> name[vertex];

  assert(has_cycle(g) == false);
  return 0;
}
