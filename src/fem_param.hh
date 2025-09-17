// fem_problem.h
#pragma once
#include "fem_core.hh"
#include <functional>

namespace fem {

// 材料属性 (示例：用于椭圆方程 -∇·(c∇u) = f 的系数c)
struct Material
{
  double coefficient_c = 1.0;
};

// 狄利克雷边界条件 (Dirichlet BC) u = value
struct DirichletBC
{
  Index nodeId;
  double value;
};

// 源项函数 f(x, y) 的类型别名
using SourceTermFunc = std::function<double( const Point<2>& )>;

} // namespace fem
