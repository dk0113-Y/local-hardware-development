#include "aihw/ops.hpp"

#include "aihw/matmul.hpp"

#include <algorithm>
#include <stdexcept>

namespace aihw {

Tensor matmul(const Tensor& a, const Tensor& b) {
  if (a.ndim() != 2 || b.ndim() != 2) {
    throw std::invalid_argument("matmul requires two 2D tensors");
  }
  if (a.cols() != b.rows()) {
    throw std::invalid_argument("matmul shape mismatch");
  }

  Tensor out({a.rows(), b.cols()});
  matmul_ikj(a.data(), b.data(), out.data(), a.rows(), b.cols(), a.cols());
  return out;
}

Tensor relu(const Tensor& x) {
  Tensor out(x.shape());
  const auto& input = x.values();
  auto& output = out.values();
  std::transform(input.begin(), input.end(), output.begin(),
                 [](float value) { return std::max(value, 0.0f); });
  return out;
}

Tensor add(const Tensor& a, const Tensor& b) {
  if (a.shape() != b.shape()) {
    throw std::invalid_argument("add requires equal shapes");
  }

  Tensor out(a.shape());
  for (std::size_t i = 0; i < a.size(); ++i) {
    out[i] = a[i] + b[i];
  }
  return out;
}

}  // namespace aihw
