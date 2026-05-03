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

void validate_2d(const Tensor& x, const std::string& name) {
  if (x.ndim() != 2) {
    throw std::invalid_argument(name + " must be a 2D tensor");
  }
}

std::size_t row_count(const Tensor& x) {
  return x.ndim() == 1 ? 1 : x.rows();
}

std::size_t row_width(const Tensor& x) {
  return x.ndim() == 1 ? x.size() : x.cols();
}

float dot_row_row(const Tensor& a,
                  std::size_t row_a,
                  const Tensor& b,
                  std::size_t row_b) {
  float sum = 0.0f;
  for (std::size_t col = 0; col < a.cols(); ++col) {
    sum += a.at2d(row_a, col) * b.at2d(row_b, col);
  }
  return sum;
}

void validate_transformer_weights(const Tensor& x,
                                  const TinyTransformerBlockWeights& weights) {
  validate_2d(x, "transformer_block input");
  const std::size_t model_dim = x.cols();

  if (weights.w_q.ndim() != 2 || weights.w_q.rows() != model_dim ||
      weights.w_q.cols() != model_dim) {
    throw std::invalid_argument("w_q must have shape [model_dim, model_dim]");
  }
  if (weights.w_k.ndim() != 2 || weights.w_k.rows() != model_dim ||
      weights.w_k.cols() != model_dim) {
    throw std::invalid_argument("w_k must have shape [model_dim, model_dim]");
  }
  if (weights.w_v.ndim() != 2 || weights.w_v.rows() != model_dim ||
      weights.w_v.cols() != model_dim) {
    throw std::invalid_argument("w_v must have shape [model_dim, model_dim]");
  }
  if (weights.w_o.ndim() != 2 || weights.w_o.rows() != model_dim ||
      weights.w_o.cols() != model_dim) {
    throw std::invalid_argument("w_o must have shape [model_dim, model_dim]");
  }
  if (weights.w_ff1.ndim() != 2 || weights.w_ff1.rows() != model_dim) {
    throw std::invalid_argument("w_ff1 must have shape [model_dim, ff_dim]");
  }
  const std::size_t ff_dim = weights.w_ff1.cols();
  if (weights.w_ff2.ndim() != 2 || weights.w_ff2.rows() != ff_dim ||
      weights.w_ff2.cols() != model_dim) {
    throw std::invalid_argument("w_ff2 must have shape [ff_dim, model_dim]");
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

Tensor scaled_dot_product_attention(const Tensor& q,
                                    const Tensor& k,
                                    const Tensor& v,
                                    bool causal) {
  validate_2d(q, "q");
  validate_2d(k, "k");
  validate_2d(v, "v");
  if (q.shape() != k.shape() || q.shape() != v.shape()) {
    throw std::invalid_argument("q, k, and v must have identical shapes");
  }

  const std::size_t seq_len = q.rows();
  const std::size_t model_dim = q.cols();
  if (model_dim == 0) {
    throw std::invalid_argument("model_dim must be positive");
  }

  Tensor scores({seq_len, seq_len});
  const float scale = 1.0f / std::sqrt(static_cast<float>(model_dim));
  for (std::size_t i = 0; i < seq_len; ++i) {
    for (std::size_t j = 0; j < seq_len; ++j) {
      scores.at2d(i, j) =
          (causal && j > i) ? -1.0e9f : dot_row_row(q, i, k, j) * scale;
    }
  }

  const Tensor attention_weights = softmax(scores);
  return matmul(attention_weights, v);
}

Tensor transformer_block(const Tensor& x,
                         const TinyTransformerBlockWeights& weights,
                         bool causal) {
  validate_transformer_weights(x, weights);

  const Tensor x_norm = layer_norm(x);
  const Tensor q = matmul(x_norm, weights.w_q);
  const Tensor k = matmul(x_norm, weights.w_k);
  const Tensor v = matmul(x_norm, weights.w_v);

  const Tensor attn = scaled_dot_product_attention(q, k, v, causal);
  const Tensor attn_projected = matmul(attn, weights.w_o);
  const Tensor residual_1 = add(x, attn_projected);

  const Tensor ff_norm = layer_norm(residual_1);
  const Tensor ff_hidden = relu(matmul(ff_norm, weights.w_ff1));
  const Tensor ff_out = matmul(ff_hidden, weights.w_ff2);

  return add(residual_1, ff_out);
}

}  // namespace aihw
