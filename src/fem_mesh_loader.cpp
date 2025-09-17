// fem_mesh_loader.cpp
#include "fem_mesh_loader.hh"
#include "mesh/triangle_element.hh" // 需要包含具体的单元实现
#include <cmath>

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace fem {

std::shared_ptr<Mesh> MeshLoader::loadFromMatlab( const std::filesystem::path& p_path,
                                                  const std::filesystem::path& t_path )
{
  auto mesh = std::make_shared<Mesh>();

  // --- 1. 加载节点 (Points) ---
  std::ifstream p_file( p_path );
  if ( !p_file.is_open() ) {
    throw std::runtime_error( "Could not open points file: " + p_path.string() );
  }

  std::string line;
  Index current_node_id = 0;
  double x = NAN;
  double y = NAN;
  while ( std::getline( p_file, line ) ) {
    std::stringstream ss( line );
    if ( !( ss >> x >> y ) ) {
      // 跳过空行或格式不正确的行
      continue;
    }
    mesh->nodes.push_back( { current_node_id++, { x, y, 0.0 } } );
  }
  p_file.close();
  std::cout << "Loaded " << mesh->getNumNodes() << " nodes." << '\n';

  // --- 2. 加载单元 (Triangles) ---
  std::ifstream t_file( t_path );
  if ( !t_file.is_open() ) {
    throw std::runtime_error( "Could not open triangles file: " + t_path.string() );
  }

  Index n1 = 0;
  Index n2 = 0;
  Index n3 = 0;
  Index subdomain = 0;
  while ( std::getline( t_file, line ) ) {
    std::stringstream ss( line );
    if ( !( ss >> n1 >> n2 >> n3 >> subdomain ) ) {
      continue;
    }

    // 关键：将1-based索引转换为0-based索引！
    Index idx1 = n1 - 1;
    Index idx2 = n2 - 1;
    Index idx3 = n3 - 1;

    // 安全检查，确保节点索引有效
    if ( idx1 >= mesh->getNumNodes() || idx2 >= mesh->getNumNodes() || idx3 >= mesh->getNumNodes() ) {
      throw std::runtime_error( "Invalid node index found in triangle file." );
    }

    mesh->elements.push_back( std::make_unique<TriangleElement>( idx1, idx2, idx3 ) );
  }
  t_file.close();
  std::cout << "Loaded " << mesh->elements.size() << " elements.\n";

  return mesh;
}

} // namespace fem
