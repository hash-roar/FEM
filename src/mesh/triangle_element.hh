
#pragma once
#include "fem_mesh.hh"
#include "fem_param.hh"

namespace fem {

class TriangleElement : public IElement
{
public:
  TriangleElement( Index n1, Index n2, Index n3 ) : node_indices_( { n1, n2, n3 } ) {}

  Index getNumNodes() const override { return 3; }
  const std::vector<Index>& getNodeIndices() const override { return node_indices_; }

  Eigen::MatrixXd calculateElementMatrix( const Mesh& mesh, const Material& material ) const override;
  Eigen::VectorXd calculateElementVector( const Mesh& mesh, SourceTermFunc source_term ) const override;

private:
  std::vector<Index> node_indices_;
};

} // namespace fem