#include "mesh/triangle_element.hh"
#include <iostream>

namespace fem {

Eigen::MatrixXd TriangleElement::calculateElementMatrix( const Mesh& mesh, const Material& material ) const
{
  // 获取三个顶点的坐标
  const auto& p1 = mesh.getNode( node_indices_[0] ).coords;
  const auto& p2 = mesh.getNode( node_indices_[1] ).coords;
  const auto& p3 = mesh.getNode( node_indices_[2] ).coords;

  // 计算B矩阵和面积 (标准线性三角形单元公式)
  Eigen::Matrix<double, 3, 2> C;
  C << ( p2.y() - p3.y() ), ( p3.x() - p2.x() ), ( p3.y() - p1.y() ), ( p1.x() - p3.x() ), ( p1.y() - p2.y() ),
    ( p2.x() - p1.x() );

  double area = 0.5
                * std::abs( ( p1.x() * ( p2.y() - p3.y() ) ) + ( p2.x() * ( p3.y() - p1.y() ) )
                            + ( p3.x() * ( p1.y() - p2.y() ) ) );
  if ( area < 1e-12 ) {
    throw std::runtime_error( "Triangle area is zero." );
  }

  Eigen::Matrix<double, 2, 3> B = C.transpose() / ( 2.0 * area );

  // Ke = c * area * (B^T * B)
  Eigen::MatrixXd Ke = material.coefficient_c * area * ( B.transpose() * B );
  return Ke;
}

Eigen::VectorXd TriangleElement::calculateElementVector( const Mesh& mesh, SourceTermFunc source_term ) const
{
  // 为简单起见，使用节点平均值法计算源项
  // f_e = (Area / 3) * [f(p1), f(p2), f(p3)]^T
  const auto& p1 = mesh.getNode( node_indices_[0] ).coords;
  const auto& p2 = mesh.getNode( node_indices_[1] ).coords;
  const auto& p3 = mesh.getNode( node_indices_[2] ).coords;
  double area = 0.5
                * std::abs( ( p1.x() * ( p2.y() - p3.y() ) ) + ( p2.x() * ( p3.y() - p1.y() ) )
                            + ( p3.x() * ( p1.y() - p2.y() ) ) );

  Eigen::Vector3d fe;
  fe << source_term( { p1.x(), p1.y() } ), source_term( { p2.x(), p2.y() } ), source_term( { p3.x(), p3.y() } );

  return ( area / 3.0 ) * fe;
}

} // namespace fem