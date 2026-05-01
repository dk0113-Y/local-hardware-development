#include "aihw/matmul.hpp"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool close_enough(float actual, float expected, float tolerance) {
  return std::fabs(actual - expected) <= tolerance;
}

int expect_vector_near(const std::string& name,
                       const std::vector<float>& actual,
                       const std::vector<float>& expected,
                       float tolerance) {
  if (actual.size() != expected.size()) {
    std::cerr << name << " size mismatch: got " << actual.size()
              << ", expected " << expected.size() << "\n";
    return 1;
  }

  for (std::size_t i = 0; i < actual.size(); ++i) {
    if (!close_enough(actual[i], expected[i], tolerance)) {
      std::cerr << name << " mismatch at " << i << ": got " << actual[i]
                << ", expected " << expected[i] << "\n";
      return 1;
    }
  }
  return 0;
}

int test_known_values() {
  const std::size_t m = 2;
  const std::size_t n = 2;
  const std::size_t k = 3;

  const std::vector<float> a = {
      1.0f, 2.0f, 3.0f,
      4.0f, 5.0f, 6.0f,
  };
  const std::vector<float> b = {
      7.0f, 8.0f,
      9.0f, 10.0f,
      11.0f, 12.0f,
  };
  const std::vector<float> expected = {
      58.0f, 64.0f,
      139.0f, 154.0f,
  };

  std::vector<float> c_ijk(m * n);
  std::vector<float> c_ikj(m * n);
  std::vector<float> c_blocked(m * n);
  std::vector<float> c_blocked_bs2(m * n);
  aihw::matmul_ijk(a.data(), b.data(), c_ijk.data(), m, n, k);
  aihw::matmul_ikj(a.data(), b.data(), c_ikj.data(), m, n, k);
  aihw::matmul_blocked_ikj(
      a.data(), b.data(), c_blocked.data(), m, n, k);
  aihw::matmul_blocked_ikj_with_block_size(
      a.data(), b.data(), c_blocked_bs2.data(), m, n, k, 2);

  if (expect_vector_near("matmul_ijk known-values", c_ijk, expected, 1e-5f)) {
    return 1;
  }
  if (expect_vector_near("matmul_ikj known-values", c_ikj, expected, 1e-5f)) {
    return 1;
  }
  if (expect_vector_near(
          "matmul_blocked_ikj known-values", c_blocked, expected, 1e-5f)) {
    return 1;
  }
  if (expect_vector_near("matmul_blocked_ikj_with_block_size known-values",
                         c_blocked_bs2, expected, 1e-5f)) {
    return 1;
  }
  return 0;
}

int test_random_shapes() {
  const std::vector<std::size_t> sizes = {1, 2, 3, 7, 16, 31};
  const std::vector<std::size_t> block_sizes = {1, 2, 3, 4, 8, 16, 32, 64, 128};

  for (std::size_t m : sizes) {
    for (std::size_t n : sizes) {
      for (std::size_t k : sizes) {
        const auto a = aihw::make_random_matrix(m, k, 1000 + m);
        const auto b = aihw::make_random_matrix(k, n, 2000 + n);
        std::vector<float> c_ref(m * n);
        std::vector<float> c_ikj(m * n);

        aihw::matmul_ijk(a.data(), b.data(), c_ref.data(), m, n, k);
        aihw::matmul_ikj(a.data(), b.data(), c_ikj.data(), m, n, k);

        const double ikj_max_diff = aihw::max_abs_diff(c_ref, c_ikj);
        if (ikj_max_diff > 1e-4) {
          std::cerr << "matmul_ikj random shape mismatch for m=" << m
                    << ", n=" << n << ", k=" << k
                    << ", max_diff=" << ikj_max_diff << "\n";
          return 1;
        }

        for (std::size_t block_size : block_sizes) {
          std::vector<float> c_blocked(m * n);
          aihw::matmul_blocked_ikj_with_block_size(
              a.data(), b.data(), c_blocked.data(), m, n, k, block_size);

          const double blocked_max_diff =
              aihw::max_abs_diff(c_ref, c_blocked);
          if (blocked_max_diff > 1e-4) {
            std::cerr << "matmul_blocked_ikj block_size=" << block_size
                      << " random shape mismatch for m=" << m << ", n=" << n
                      << ", k=" << k << ", max_diff=" << blocked_max_diff
                      << "\n";
            return 1;
          }
        }
      }
    }
  }

  return 0;
}

}  // namespace

int main() {
  if (test_known_values()) {
    return 1;
  }
  if (test_random_shapes()) {
    return 1;
  }

  std::cout << "matmul tests passed\n";
  return 0;
}
