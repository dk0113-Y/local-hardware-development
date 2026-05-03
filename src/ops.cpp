#include "aihw/ops.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace aihw {

namespace {

void matmul_ikj_contiguous(const float* a,
                           const float* b,
                           float* c,
                           std::size_t m,
                           std::size_t n,
                           std::size_t k) {
  std::fill(c, c + m * n, 0.0f);

  for (std::size_t i = 0; i < m; ++i) {
    float* c_row = c + i * n;
    for (std::size_t p = 0; p < k; ++p) {
      const float a_ip = a[i * k + p];
      const float* b_row = b + p * n;
      for (std::size_t j = 0; j < n; ++j) {
        c_row[j] += a_ip * b_row[j];
      }
    }
  }
}

}  // namespace

Tensor matmul(const Tensor& a, const Tensor& b) {
  if (a.ndim() != 2 || b.ndim() != 2) {
    throw std::invalid_argument("matmul requires two 2D tensors");
  }
  if (a.cols() != b.rows()) {
    throw std::invalid_argument("matmul shape mismatch");
  }

  Tensor out({a.rows(), b.cols()});
  matmul_ikj_contiguous(
      a.data(), b.data(), out.data(), a.rows(), b.cols(), a.cols());
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
