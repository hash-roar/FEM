// fem_mesh.h
#pragma once
#include "fem_core.hh"
#include <functional>
#include <memory>
#include <string>

namespace fem {

// 前向声明，解决头文件循环依赖
class Mesh;
struct Material;

// 节点结构体
struct Node
{
  Index id;
  Point<3> coords; // 统一使用3D坐标，2D问题可忽略z分量
};

// 单元的抽象基类 (接口)
class IElement
{
public:
  virtual ~IElement() = default;

  // 获取单元包含的节点数量
  virtual Index getNumNodes() const = 0;

  // 获取组成该单元的节点ID列表
  virtual const std::vector<Index>& getNodeIndices() const = 0;

  // 计算单元刚度/贡献矩阵 (例如，用于刚度矩阵K)
  // 这是接口的核心，不同物理问题和单元类型将有不同实现
  virtual Eigen::MatrixXd calculateElementMatrix( const Mesh& mesh, const Material& material ) const = 0;

  // 计算单元载荷/源项向量 (例如，用于载荷向量F)
  virtual Eigen::VectorXd calculateElementVector( const Mesh& mesh,
                                                  std::function<double( const Point<2>& )> source_term ) const
    = 0;
};

// 网格类，作为节点和单元的容器
class Mesh
{
public:
  std::vector<Node> nodes;
  // todo: make dense memory storage
  std::vector<std::unique_ptr<IElement>> elements;

  // 辅助函数
  const Node& getNode( Index node_id ) const
  {
    // 实际项目中应有更健壮的查找方式
    return nodes[node_id];
  }

  Index getNumNodes() const { return nodes.size(); }
  Index getNumDofs( int dofs_per_node ) const { return nodes.size() * dofs_per_node; }
};

} // namespace fem
