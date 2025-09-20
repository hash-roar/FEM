#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "fem_core.hh"
#include "fem_mesh_loader.hh"
#include "mesh/triangle_element.hh"

namespace fem {

void MeshLoader::loadNodes( Mesh& mesh, const std::filesystem::path& path )
{
  std::ifstream file( path );
  if ( !file ) {
    throw std::runtime_error( fmt::format( "Could not open points file: {}", path.string() ) );
  }

  std::string line;
  while ( std::getline( file, line ) ) {
    if ( auto parsed_data = parseLine<double, double>( line ) ) {

      auto& [x, y] = *parsed_data;
      Index current_node_id = mesh.getNumNodes();

      mesh.nodes.push_back( { current_node_id, { x, y, 0.0 } } );
    }
  }
  std::cout << fmt::format( "Loaded {} nodes.\n", mesh.getNumNodes() );
}

void MeshLoader::loadElements( Mesh& mesh, const std::filesystem::path& path )
{
  std::ifstream file( path );
  if ( !file ) {
    throw std::runtime_error( fmt::format( "Could not open triangles file: {}", path.string() ) );
  }

  const auto num_nodes = mesh.getNumNodes();

  std::string line;
  while ( std::getline( file, line ) ) {
    if ( auto parsed_data = parseLine<Index, Index, Index, Index>( line ) ) {
      auto& [n1, n2, n3, subdomain] = *parsed_data;

      Index idx1 = n1 - 1;
      Index idx2 = n2 - 1;
      Index idx3 = n3 - 1;

      if ( idx1 >= num_nodes || idx2 >= num_nodes || idx3 >= num_nodes ) {
        throw std::runtime_error( "Invalid node index found in triangle file." );
      }

      mesh.elements.emplace_back( std::make_unique<TriangleElement>( idx1, idx2, idx3 ) );
    }
  }
  std::cout << fmt::format( "Loaded {} elements.\n", mesh.elements.size() );
}

std::shared_ptr<Mesh> MeshLoader::loadFromMatlab( const std::filesystem::path& p_path,
                                                  const std::filesystem::path& t_path )
{
  auto mesh = std::make_shared<Mesh>();

  loadNodes( *mesh, p_path );
  loadElements( *mesh, t_path );

  return mesh;
}
} // namespace fem