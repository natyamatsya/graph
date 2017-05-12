//=======================================================================
// Copyright 2007 Aaron Windsor
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/property_map/property_map.hpp>
#include <vector>

#include <boost/graph/planar_canonical_ordering.hpp>
#include <boost/graph/is_straight_line_drawing.hpp>
#include <boost/graph/chrobak_payne_drawing.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

//a class to hold the coordinates of the straight line embedding
struct coord_t
{
  std::size_t x;
  std::size_t y;
};


int main(int argc, char** argv)
{
  using graph = boost::adjacency_list<boost::vecS, boost::vecS,
    boost::undirectedS,
    boost::property<boost::vertex_index_t, int>>;

  //Define the storage type for the planar embedding
  using embedding_storage_t = std::vector<std::vector<boost::graph_traits<graph>::edge_descriptor>>;
  using embedding_t = boost::iterator_property_map
    < embedding_storage_t::iterator, 
      boost::property_map<graph, boost::vertex_index_t>::type 
    >;



  // Create the graph - a maximal planar graph on 7 vertices. The functions
  // planar_canonical_ordering and chrobak_payne_straight_line_drawing both
  // require a maximal planar graph. If you start with a graph that isn't
  // maximal planar (or you're not sure), you can use the functions
  // make_connected, make_biconnected_planar, and make_maximal planar in
  // sequence to add a set of edges to any undirected planar graph to make
  // it maximal planar.
  
  graph g(7);
  boost::add_edge(0,1,g);
  boost::add_edge(1,2,g);
  boost::add_edge(2,3,g);
  boost::add_edge(3,0,g);
  boost::add_edge(3,4,g);
  boost::add_edge(4,5,g);
  boost::add_edge(5,6,g);
  boost::add_edge(6,3,g);
  boost::add_edge(0,4,g);
  boost::add_edge(1,3,g);
  boost::add_edge(3,5,g);
  boost::add_edge(2,6,g);
  boost::add_edge(1,4,g);
  boost::add_edge(1,5,g);
  boost::add_edge(1,6,g);



  // Create the planar embedding
  embedding_storage_t embedding_storage(boost::num_vertices(g));
  embedding_t embedding(embedding_storage.begin(), boost::get(boost::vertex_index,g));

  boost::boyer_myrvold_planarity_test(boost::boyer_myrvold_params::graph = g,
                               boost::boyer_myrvold_params::embedding = embedding
                               );



  // Find a canonical ordering
  std::vector<boost::graph_traits<graph>::vertex_descriptor> ordering;
  planar_canonical_ordering(g, embedding, std::back_inserter(ordering));


  //Set up a property map to hold the mapping from vertices to coord_t's
  using straight_line_drawing_storage_t = std::vector<coord_t>;
  using straight_line_drawing_t = boost::iterator_property_map
    < straight_line_drawing_storage_t::iterator, 
      boost::property_map<graph, boost::vertex_index_t>::type 
    >;

  straight_line_drawing_storage_t straight_line_drawing_storage
    (boost::num_vertices(g));
  straight_line_drawing_t straight_line_drawing
    (straight_line_drawing_storage.begin(), 
     boost::get(boost::vertex_index,g)
     );



  // Compute the straight line drawing
  chrobak_payne_straight_line_drawing(g, 
                                      embedding, 
                                      ordering.begin(),
                                      ordering.end(),
                                      straight_line_drawing
                                      );
  


  std::cout << "The straight line drawing is: " << std::endl;
  for(const auto& vertex : make_range_pair(boost::vertices(g)))
    {
      coord_t coord(boost::get(straight_line_drawing,vertex));
      std::cout << vertex << " -> (" << coord.x << ", " << coord.y << ")" 
                << std::endl;
    }

  // Verify that the drawing is actually a plane drawing
  if (is_straight_line_drawing(g, straight_line_drawing))
    std::cout << "Is a plane drawing." << std::endl;
  else
    std::cout << "Is not a plane drawing." << std::endl;

  return 0;
}
