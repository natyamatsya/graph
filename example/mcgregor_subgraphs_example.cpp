//=======================================================================
// Copyright 2009 Trustees of Indiana University.
// Authors: Michael Hansen
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <fstream>
#include <iostream>
#include <string>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/mcgregor_common_subgraphs.hpp>
#include <boost/property_map/shared_array_property_map.hpp>

// Callback that looks for the first common subgraph whose size
// matches the user's preference.
template <typename Graph>
struct example_callback {

  using VertexSizeFirst = typename boost::graph_traits<Graph>::vertices_size_type;

  example_callback(const Graph& graph1) :
    m_graph1(graph1) { }

  template <typename CorrespondenceMapFirstToSecond,
            typename CorrespondenceMapSecondToFirst>
  bool operator()(CorrespondenceMapFirstToSecond correspondence_map_1_to_2,
                  CorrespondenceMapSecondToFirst correspondence_map_2_to_1,
                  VertexSizeFirst subgraph_size) {

    // Fill membership map for first graph
    using VertexIndexMap = typename boost::property_map<Graph, boost::vertex_index_t>::type;
    using MembershipMap = boost::shared_array_property_map<bool, VertexIndexMap>;
      
    MembershipMap membership_map1(boost::num_vertices(m_graph1),
                                  boost::get(boost::vertex_index, m_graph1));

    boost::fill_membership_map<Graph>(m_graph1, correspondence_map_1_to_2, membership_map1);

    // Generate filtered graphs using membership map
    using MembershipFilteredGraph =
      typename boost::membership_filtered_graph_traits<Graph, MembershipMap>::graph_type;

    MembershipFilteredGraph subgraph1 =
      make_membership_filtered_graph(m_graph1, membership_map1);

    // Print the graph out to the console
    std::cout << "Found common subgraph (size " << subgraph_size << ")" << std::endl;
    boost::print_graph(subgraph1);
    std::cout << std::endl;

    // Explore the entire space
    return (true);
  }

private:
  const Graph& m_graph1;
  VertexSizeFirst m_max_subgraph_size;
};

int main (int argc, char *argv[]) {

  // Using a boost::vecS graph here so that we don't have to mess around with
  // a vertex index map; it will be implicit.
  using Graph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS,
    boost::property<boost::vertex_name_t, unsigned int,
    boost::property<boost::vertex_index_t, unsigned int>>,
    boost::property<boost::edge_name_t, unsigned int>>;

  // Test maximum and unique variants on known graphs
  Graph graph_simple1, graph_simple2;
  example_callback<Graph> user_callback(graph_simple1);

  auto vname_map_simple1 = boost::get(boost::vertex_name, graph_simple1);
  auto vname_map_simple2 = boost::get(boost::vertex_name, graph_simple2);

  // Graph that looks like a triangle
  put(vname_map_simple1, add_vertex(graph_simple1), 1);
  put(vname_map_simple1, add_vertex(graph_simple1), 2);
  put(vname_map_simple1, add_vertex(graph_simple1), 3);

  boost::add_edge(0, 1, graph_simple1);
  boost::add_edge(0, 2, graph_simple1);
  boost::add_edge(1, 2, graph_simple1);

  std::cout << "First graph:" << std::endl;
  boost::print_graph(graph_simple1);
  std::cout << std::endl;

  // Triangle with an extra vertex
  put(vname_map_simple2, add_vertex(graph_simple2), 1);
  put(vname_map_simple2, add_vertex(graph_simple2), 2);
  put(vname_map_simple2, add_vertex(graph_simple2), 3);
  put(vname_map_simple2, add_vertex(graph_simple2), 4);

  boost::add_edge(0, 1, graph_simple2);
  boost::add_edge(0, 2, graph_simple2);
  boost::add_edge(1, 2, graph_simple2);
  boost::add_edge(1, 3, graph_simple2);

  std::cout << "Second graph:" << std::endl;
  boost::print_graph(graph_simple2);
  std::cout << std::endl;

  // All subgraphs
  std::cout << "mcgregor_common_subgraphs:" << std::endl;
  boost::mcgregor_common_subgraphs
    (graph_simple1, graph_simple2, true, user_callback,
     boost::vertices_equivalent(boost::make_property_map_equivalent(vname_map_simple1, vname_map_simple2))); 
  std::cout << std::endl;

  // Unique subgraphs
  std::cout << "mcgregor_common_subgraphs_unique:" << std::endl;
  boost::mcgregor_common_subgraphs_unique
    (graph_simple1, graph_simple2, true, user_callback,
     boost::vertices_equivalent(boost::make_property_map_equivalent(vname_map_simple1, vname_map_simple2))); 
  std::cout << std::endl;

  // Maximum subgraphs
  std::cout << "mcgregor_common_subgraphs_maximum:" << std::endl;
  boost::mcgregor_common_subgraphs_maximum
    (graph_simple1, graph_simple2, true, user_callback,
     boost::vertices_equivalent(boost::make_property_map_equivalent(vname_map_simple1, vname_map_simple2))); 
  std::cout << std::endl;

  // Maximum, unique subgraphs
  std::cout << "mcgregor_common_subgraphs_maximum_unique:" << std::endl;
  boost::mcgregor_common_subgraphs_maximum_unique
    (graph_simple1, graph_simple2, true, user_callback,
     boost::vertices_equivalent(boost::make_property_map_equivalent(vname_map_simple1, vname_map_simple2))); 

  return 0;
}
