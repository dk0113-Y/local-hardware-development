#include "aihw/ops.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>

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

void validate_rowwise_1d_or_2d(const Tensor& x, const char* op_name) {
  if (x.ndim() != 1 && x.ndim() != 2) {
    throw std::invalid_argument(std::string(op_name) +
                                " requires a 1D or 2D tensor");
  }
}

std::size_t row_count(const Tensor& x) {
  return x.ndim() == 1 ? 1 : x.rows();
}

std::size_t row_width(const Tensor& x) {
  return x.ndim() == 1 ? x.size() : x.cols();
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

Tensor softmax(const Tensor& x) {
  validate_rowwise_1d_or_2d(x, "softmax");

  Tensor out(x.shape());
  const std::size_t rows = row_count(x);
  const std::size_t cols = row_width(x);

  for (std::size_t row = 0; row < rows; ++row) {
    const std::size_t offset = row * cols;
    const float max_value =
        *std::max_element(x.values().begin() + offset,
                          x.values().begin() + offset + cols);

    float sum = 0.0f;
    for (std::size_t col = 0; col < cols; ++col) {
      const float exp_value = std::exp(x[offset + col] - max_value);
      out[offset + col] = exp_value;
      sum += exp_value;
    }
    for (std::size_t col = 0; col < cols; ++col) {
      out[offset + col] /= sum;
    }
  }

  return out;
}

Tensor layer_norm(const Tensor& x, float epsilon) {
  validate_rowwise_1d_or_2d(x, "layer_norm");
  if (epsilon <= 0.0f) {
    throw std::invalid_argument("layer_norm epsilon must be positive");
  }

  Tensor out(x.shape());
  const std::size_t rows = row_count(x);
  const std::size_t cols = row_width(x);

  for (std::size_t row = 0; row < rows; ++row) {
    const std::size_t offset = row * cols;
    float mean = 0.0f;
    for (std::size_t col = 0; col < cols; ++col) {
      mean += x[offset + col];
    }
    mean /= static_cast<float>(cols);

    float variance = 0.0f;
    for (std::size_t col = 0; col < cols; ++col) {
      const float centered = x[offset + col] - mean;
      variance += centered * centered;
    }
    variance /= static_cast<float>(cols);

    const float inv_std = 1.0f / std::sqrt(variance + epsilon);
    for (std::size_t col = 0; col < cols; ++col) {
      out[offset + col] = (x[offset + col] - mean) * inv_std;
    }
  }

  return out;
}

}  // namespace aihw
