#pragma once

#include "aihw/tensor.hpp"

namespace aihw {

Tensor matmul(const Tensor& a, const Tensor& b);
Tensor relu(const Tensor& x);
Tensor add(const Tensor& a, const Tensor& b);
Tensor softmax(const Tensor& x);
Tensor layer_norm(const Tensor& x, float epsilon = 1e-5f);

}  // namespace aihw
