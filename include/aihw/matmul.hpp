#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace aihw {

// All matrices are row-major:
// A is [m x k], B is [k x n], C is [m x n].
void matmul_ijk(const float* a,
                const float* b,
                float* c,
                std::size_t m,
                std::size_t n,
                std::size_t k);

void matmul_ikj(const float* a,
                const float* b,
                float* c,
                std::size_t m,
                std::size_t n,
                std::size_t k);

void matmul_blocked_ikj(const float* a,
                        const float* b,
                        float* c,
                        std::size_t m,
                        std::size_t n,
                        std::size_t k);

std::vector<float> make_random_matrix(std::size_t rows,
                                      std::size_t cols,
                                      std::uint32_t seed);

double max_abs_diff(const std::vector<float>& lhs,
                    const std::vector<float>& rhs);

}  // namespace aihw
