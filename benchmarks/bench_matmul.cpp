#include "aihw/matmul.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

using MatmulFn = void (*)(const float*,
                         const float*,
                         float*,
                         std::size_t,
                         std::size_t,
                         std::size_t);

struct BenchResult {
  std::string name;
  double best_ms;
  double gflops;
  double max_diff;
};

std::size_t parse_arg(char** argv, int index, std::size_t fallback) {
  if (argv[index] == nullptr) {
    return fallback;
  }
  const long long parsed = std::atoll(argv[index]);
  if (parsed <= 0) {
    return fallback;
  }
  return static_cast<std::size_t>(parsed);
}

BenchResult run_kernel(const std::string& name,
                       MatmulFn fn,
                       const std::vector<float>& a,
                       const std::vector<float>& b,
                       const std::vector<float>& reference,
                       std::size_t m,
                       std::size_t n,
                       std::size_t k,
                       int iterations) {
  std::vector<float> c(m * n);
  fn(a.data(), b.data(), c.data(), m, n, k);

  double best_ms = 1e100;
  for (int iter = 0; iter < iterations; ++iter) {
    const auto start = std::chrono::steady_clock::now();
    fn(a.data(), b.data(), c.data(), m, n, k);
    const auto stop = std::chrono::steady_clock::now();
    const double ms =
        std::chrono::duration<double, std::milli>(stop - start).count();
    best_ms = std::min(best_ms, ms);
  }

  const double operations = 2.0 * static_cast<double>(m) *
                            static_cast<double>(n) *
                            static_cast<double>(k);
  const double seconds = best_ms / 1000.0;
  return {name, best_ms, operations / seconds / 1e9,
          aihw::max_abs_diff(c, reference)};
}

void print_usage(const char* program) {
  std::cout << "Usage: " << program << " [m] [n] [k] [iterations]\n"
            << "Default: " << program << " 256 256 256 5\n";
}

}  // namespace

int main(int argc, char** argv) {
  if (argc > 1 && std::string(argv[1]) == "--help") {
    print_usage(argv[0]);
    return 0;
  }

  const std::size_t m = argc > 1 ? parse_arg(argv, 1, 256) : 256;
  const std::size_t n = argc > 2 ? parse_arg(argv, 2, 256) : 256;
  const std::size_t k = argc > 3 ? parse_arg(argv, 3, 256) : 256;
  const int iterations =
      argc > 4 ? static_cast<int>(parse_arg(argv, 4, 5)) : 5;

  const auto a = aihw::make_random_matrix(m, k, 123);
  const auto b = aihw::make_random_matrix(k, n, 456);
  std::vector<float> reference(m * n);
  aihw::matmul_ijk(a.data(), b.data(), reference.data(), m, n, k);

  const std::vector<BenchResult> results = {
      run_kernel("ijk_baseline", aihw::matmul_ijk, a, b, reference, m, n, k,
                 iterations),
      run_kernel("ikj_cache_order", aihw::matmul_ikj, a, b, reference, m, n, k,
                 iterations),
      run_kernel("blocked_ikj_bs64", aihw::matmul_blocked_ikj, a, b, reference,
                 m, n, k, iterations),
  };

  std::cout << "matrix_shape,m=" << m << ",n=" << n << ",k=" << k
            << ",iterations=" << iterations << "\n";
  std::cout << std::left << std::setw(18) << "kernel" << std::right
            << std::setw(14) << "best_ms" << std::setw(14) << "gflops"
            << std::setw(14) << "max_diff" << "\n";

  for (const BenchResult& result : results) {
    std::cout << std::left << std::setw(18) << result.name << std::right
              << std::setw(14) << std::fixed << std::setprecision(3)
              << result.best_ms << std::setw(14) << std::setprecision(3)
              << result.gflops << std::setw(14) << std::scientific
              << std::setprecision(2) << result.max_diff << std::defaultfloat
              << "\n";
  }

  return 0;
}
