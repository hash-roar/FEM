// fem_mesh_loader.h
#pragma once
#include "fem_mesh.hh"
#include <filesystem> // 使用C++17的文件系统库
#include <string>

namespace fem {

class MeshLoader
{
public:
  /**
   * @brief 从MATLAB PDE工具箱格式的文件中加载网格。
   *
   * @param p_path 指向节点坐标文件 (e.g., "mesh.p.dat") 的路径。
   * @param t_path 指向单元拓扑文件 (e.g., "mesh.t.dat") 的路径。
   * @return std::shared_ptr<Mesh> 指向新创建的Mesh对象的智能指针。
   * @throws std::runtime_error 如果文件无法打开或格式不正确。
   */
  static std::shared_ptr<Mesh> loadFromMatlab( const std::filesystem::path& p_path,
                                               const std::filesystem::path& t_path );
};

} // namespace fem
