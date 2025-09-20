// fem_mesh_loader.h
#pragma once
#include "fem_mesh.hh"
#include <filesystem> // 使用C++17的文件系统库
#include <optional>
#include <string>

namespace fem {

class MeshLoader
{
public:
  std::shared_ptr<Mesh> loadFromMatlab( const std::filesystem::path& p_path, const std::filesystem::path& t_path );

private:
  template<typename... Args, std::size_t... I>
  std::optional<std::tuple<Args...>> parseLine_impl( const std::string& line, std::index_sequence<I...> /*unused*/ )
  {
    std::stringstream ss( line );
    std::tuple<Args...> values;

    if ( ( ( ss >> std::get<I>( values ) ) && ... ) ) {

      if ( ss.rdbuf()->in_avail() == 0 ) {
        return values;
      }
    }
    return std::nullopt;
  }

  template<typename... Args>
  std::optional<std::tuple<Args...>> parseLine( const std::string& line )
  {

    return parseLine_impl<Args...>( line, std::index_sequence_for<Args...> {} );
  }

  void loadNodes( Mesh& mesh, const std::filesystem::path& path );
  void loadElements( Mesh& mesh, const std::filesystem::path& path );
};

} // namespace fem
