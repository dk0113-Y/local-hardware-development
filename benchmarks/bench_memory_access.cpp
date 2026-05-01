#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

namespace {

volatile std::uint64_t g_sink = 0;

constexpr std::size_t kib = 1024;
constexpr std::size_t mib = 1024 * kib;

struct BenchResult {
  std::string benchmark;
  std::size_t array_kib;
  std::string access_pattern;
  std::size_t stride_bytes;
  int iterations;
  std::uint64_t total_accesses;
  double best_ms;
  double ns_per_access;
  double gb_per_s;
  std::uint64_t sink;
};

double elapsed_ms(std::chrono::steady_clock::time_point start,
                  std::chrono::steady_clock::time_point stop) {
  return std::chrono::duration<double, std::milli>(stop - start).count();
}

std::vector<std::uint64_t> make_values(std::size_t count) {
  std::vector<std::uint64_t> values(count);
  std::iota(values.begin(), values.end(), std::uint64_t{1});
  return values;
}

void print_header() {
  std::cout << "benchmark,array_kib,access_pattern,stride_bytes,iterations,"
               "total_accesses,best_ms,ns_per_access,gb_per_s,sink\n";
}

void print_result(const BenchResult& result) {
  std::cout << result.benchmark << "," << result.array_kib << ","
            << result.access_pattern << "," << result.stride_bytes << ","
            << result.iterations << "," << result.total_accesses << ","
            << std::fixed << std::setprecision(3) << result.best_ms << ","
            << std::setprecision(3) << result.ns_per_access << ","
            << std::setprecision(3) << result.gb_per_s << ","
            << result.sink << "\n";
}

BenchResult run_sequential_read(std::size_t array_kib, int iterations) {
  const std::size_t bytes = array_kib * kib;
  const std::size_t count = std::max<std::size_t>(1, bytes / sizeof(std::uint64_t));
  const std::uint64_t target_accesses =
      std::max<std::uint64_t>(16ull * 1024ull * 1024ull,
                              static_cast<std::uint64_t>(count));
  const auto values = make_values(count);

  double best_ms = 1e100;
  std::uint64_t best_sink = 0;
  for (int iter = 0; iter < iterations; ++iter) {
    std::uint64_t sum = 0;
    std::uint64_t accesses = 0;
    const auto start = std::chrono::steady_clock::now();
    while (accesses < target_accesses) {
      for (std::uint64_t value : values) {
        sum += value;
        ++accesses;
        if (accesses == target_accesses) {
          break;
        }
      }
    }
    const auto stop = std::chrono::steady_clock::now();
    g_sink = sum;

    const double ms = elapsed_ms(start, stop);
    if (ms < best_ms) {
      best_ms = ms;
      best_sink = sum;
    }
  }

  const double ns_per_access =
      best_ms * 1.0e6 / static_cast<double>(target_accesses);
  const double gb_per_s =
      (static_cast<double>(target_accesses * sizeof(std::uint64_t)) /
       (best_ms / 1000.0)) /
      1.0e9;

  return {"sequential_read", array_kib, "sequential", sizeof(std::uint64_t),
          iterations, target_accesses, best_ms, ns_per_access, gb_per_s,
          best_sink};
}

BenchResult run_strided_read(std::size_t array_kib,
                             std::size_t stride_bytes,
                             int iterations,
                             std::uint64_t target_accesses) {
  const std::size_t bytes = array_kib * kib;
  const std::size_t count = std::max<std::size_t>(1, bytes / sizeof(std::uint64_t));
  const std::size_t stride_elems =
      std::max<std::size_t>(1, stride_bytes / sizeof(std::uint64_t));
  const auto values = make_values(count);

  double best_ms = 1e100;
  std::uint64_t best_sink = 0;
  for (int iter = 0; iter < iterations; ++iter) {
    std::uint64_t sum = 0;
    std::uint64_t accesses = 0;
    const auto start = std::chrono::steady_clock::now();
    while (accesses < target_accesses) {
      for (std::size_t index = 0; index < count && accesses < target_accesses;
           index += stride_elems) {
        sum += values[index];
        ++accesses;
      }
    }
    const auto stop = std::chrono::steady_clock::now();
    g_sink = sum;

    const double ms = elapsed_ms(start, stop);
    if (ms < best_ms) {
      best_ms = ms;
      best_sink = sum;
    }
  }

  const double ns_per_access =
      best_ms * 1.0e6 / static_cast<double>(target_accesses);
  const double gb_per_s =
      (static_cast<double>(target_accesses * sizeof(std::uint64_t)) /
       (best_ms / 1000.0)) /
      1.0e9;

  return {"strided_read", array_kib, "stride", stride_bytes, iterations,
          target_accesses, best_ms, ns_per_access, gb_per_s, best_sink};
}

std::vector<std::size_t> make_random_cycle(std::size_t count) {
  std::vector<std::size_t> permutation(count);
  std::iota(permutation.begin(), permutation.end(), std::size_t{0});

  std::mt19937 rng(12345);
  std::shuffle(permutation.begin(), permutation.end(), rng);

  std::vector<std::size_t> next(count);
  for (std::size_t i = 0; i < count; ++i) {
    next[permutation[i]] = permutation[(i + 1) % count];
  }
  return next;
}

std::uint64_t pointer_chasing_accesses(std::size_t array_kib) {
  if (array_kib <= 256) {
    return 64ull * 1024ull * 1024ull;
  }
  if (array_kib <= 16 * 1024) {
    return 32ull * 1024ull * 1024ull;
  }
  return 8ull * 1024ull * 1024ull;
}

BenchResult run_pointer_chasing(std::size_t array_kib, int iterations) {
  const std::size_t bytes = array_kib * kib;
  const std::size_t count = std::max<std::size_t>(1, bytes / sizeof(std::size_t));
  const auto next = make_random_cycle(count);
  const std::uint64_t target_accesses = pointer_chasing_accesses(array_kib);

  double best_ms = 1e100;
  std::size_t best_index = 0;
  for (int iter = 0; iter < iterations; ++iter) {
    std::size_t index = iter % count;
    const auto start = std::chrono::steady_clock::now();
    for (std::uint64_t step = 0; step < target_accesses; ++step) {
      index = next[index];
    }
    const auto stop = std::chrono::steady_clock::now();
    g_sink = static_cast<std::uint64_t>(index);

    const double ms = elapsed_ms(start, stop);
    if (ms < best_ms) {
      best_ms = ms;
      best_index = index;
    }
  }

  const double ns_per_access =
      best_ms * 1.0e6 / static_cast<double>(target_accesses);

  return {"pointer_chasing", array_kib, "random_pointer", 0, iterations,
          target_accesses, best_ms, ns_per_access, 0.0,
          static_cast<std::uint64_t>(best_index)};
}

}  // namespace

int main() {
  print_header();

  const std::vector<std::size_t> sequential_sizes_kib = {
      4,      8,      16,     32,     64,      128,     256,
      512,    1024,   2048,   4096,   8192,    16384,   32768,
      65536,  131072, 262144,
  };
  for (std::size_t size_kib : sequential_sizes_kib) {
    print_result(run_sequential_read(size_kib, 5));
  }

  const std::size_t strided_array_kib = 64 * 1024;
  const std::vector<std::size_t> stride_bytes = {
      8, 16, 32, 64, 128, 256, 512, 1024, 4096,
  };
  for (std::size_t stride : stride_bytes) {
    print_result(
        run_strided_read(strided_array_kib, stride, 5, 16ull * 1024ull * 1024ull));
  }

  const std::vector<std::size_t> pointer_sizes_kib = {
      4, 16, 64, 256, 1024, 4096, 16384, 65536, 131072, 262144,
  };
  for (std::size_t size_kib : pointer_sizes_kib) {
    print_result(run_pointer_chasing(size_kib, 3));
  }

  return static_cast<int>(g_sink == 0xFFFFFFFFFFFFFFFFull);
}
