#pragma once

#include "aihw/tensor.hpp"

namespace aihw {

Tensor matmul(const Tensor& a, const Tensor& b);
Tensor relu(const Tensor& x);
Tensor add(const Tensor& a, const Tensor& b);

}  // namespace aihw
