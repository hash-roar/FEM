// fem_core.h
#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <cstddef>

namespace fem {

// 类型别名，方便使用
using Index = std::size_t;
template<int Dim>
using Point = Eigen::Matrix<double, Dim, 1>;

using Vector = Eigen::VectorXd;
using Matrix = Eigen::SparseMatrix<double>;

} // namespace fem
