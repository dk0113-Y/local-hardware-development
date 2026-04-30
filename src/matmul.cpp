#include "aihw/matmul.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

namespace aihw {

void matmul_ijk(const float* a,
                const float* b,
                float* c,
                std::size_t m,
                std::size_t n,
                std::size_t k) {
  std::fill(c, c + m * n, 0.0f);

  for (std::size_t i = 0; i < m; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      float acc = 0.0f;
      for (std::size_t p = 0; p < k; ++p) {
        acc += a[i * k + p] * b[p * n + j];
      }
      c[i * n + j] = acc;
    }
  }
}

void matmul_ikj(const float* a,
                const float* b,
                float* c,
                std::size_t m,
                std::size_t n,
                std::size_t k) {
  std::fill(c, c + m * n, 0.0f);

  for (std::size_t i = 0; i < m; ++i) {
    for (std::size_t p = 0; p < k; ++p) {
      const float a_ip = a[i * k + p];
      const float* b_row = b + p * n;
      float* c_row = c + i * n;
      for (std::size_t j = 0; j < n; ++j) {
        c_row[j] += a_ip * b_row[j];
      }
    }
  }
}

std::vector<float> make_random_matrix(std::size_t rows,
                                      std::size_t cols,
                                      std::uint32_t seed) {
  std::mt19937 rng(seed);
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

  std::vector<float> values(rows * cols);
  for (float& value : values) {
    value = dist(rng);
  }
  return values;
}

double max_abs_diff(const std::vector<float>& lhs,
                    const std::vector<float>& rhs) {
  if (lhs.size() != rhs.size()) {
    return std::numeric_limits<double>::infinity();
  }

  double max_diff = 0.0;
  for (std::size_t i = 0; i < lhs.size(); ++i) {
    const double diff =
        std::fabs(static_cast<double>(lhs[i]) - static_cast<double>(rhs[i]));
    max_diff = std::max(max_diff, diff);
  }
  return max_diff;
}

}  // namespace aihw
