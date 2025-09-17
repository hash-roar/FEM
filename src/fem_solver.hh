// fem_solver.h
#pragma once
#include <utility>

#include "fem_mesh.hh"
#include "fem_param.hh"

namespace fem {

class Solver
{
public:
  // 构造函数：通过依赖注入接收网格和问题维度
  explicit Solver( std::shared_ptr<Mesh> mesh, int dofs_per_node = 1 )
    : mesh_( std::move( mesh ) ), dofs_per_node_( dofs_per_node )
  {}

  // 设置求解所需的组件
  void setMaterial( std::shared_ptr<Material> material ) { material_ = std::move( material ); }
  void setSourceTerm( SourceTermFunc func ) { source_term_ = std::move( func ); }
  void addDirichletBC( const DirichletBC& bc ) { bcs_.push_back( bc ); }

  // 核心求解方法
  Vector solve();

private:
  void assemble();
  void applyBCs();

  std::shared_ptr<Mesh> mesh_;
  std::shared_ptr<Material> material_;
  SourceTermFunc source_term_;
  std::vector<DirichletBC> bcs_;

  int dofs_per_node_;
  Matrix K_global_;
  Vector F_global_;
  Vector U_global_;
};

} // namespace fem
