#include "aihw/ops.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

bool close_enough(float actual, float expected, float tolerance) {
  return std::fabs(actual - expected) <= tolerance;
}

int fail(const std::string& message) {
  std::cerr << message << "\n";
  return 1;
}

int expect_vector_near(const std::string& name,
                       const std::vector<float>& actual,
                       const std::vector<float>& expected,
                       float tolerance) {
  if (actual.size() != expected.size()) {
    return fail(name + " size mismatch");
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

int test_tensor_shape_and_size() {
  const aihw::Tensor vector({3}, {1.0f, 2.0f, 3.0f});
  if (vector.ndim() != 1 || vector.size() != 3 || vector.empty()) {
    return fail("1D tensor shape/size mismatch");
  }

  const aihw::Tensor matrix({2, 3});
  if (matrix.ndim() != 2 || matrix.size() != 6 || matrix.rows() != 2 ||
      matrix.cols() != 3) {
    return fail("2D tensor shape/size mismatch");
  }

  try {
    const aihw::Tensor invalid({2, 2}, {1.0f, 2.0f, 3.0f});
    (void)invalid;
    return fail("invalid tensor data size did not throw");
  } catch (const std::invalid_argument&) {
  }

  return 0;
}

int test_at2d() {
  aihw::Tensor matrix({2, 2}, {1.0f, 2.0f, 3.0f, 4.0f});
  matrix.at2d(1, 0) = 5.0f;
  if (!close_enough(matrix.at2d(1, 0), 5.0f, 1e-6f)) {
    return fail("at2d write/read mismatch");
  }

  try {
    (void)matrix.at2d(2, 0);
    return fail("at2d out-of-range did not throw");
  } catch (const std::out_of_range&) {
  }

  return 0;
}

int test_runtime_matmul() {
  const aihw::Tensor a({2, 3}, {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f});
  const aihw::Tensor b({3, 2}, {7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f});
  const aihw::Tensor c = aihw::matmul(a, b);

  if (c.rows() != 2 || c.cols() != 2) {
    return fail("matmul output shape mismatch");
  }
  return expect_vector_near("matmul", c.values(),
                            {58.0f, 64.0f, 139.0f, 154.0f}, 1e-5f);
}

int test_relu() {
  const aihw::Tensor x({5}, {-2.0f, -0.5f, 0.0f, 1.5f, 3.0f});
  const aihw::Tensor y = aihw::relu(x);
  return expect_vector_near("relu", y.values(), {0.0f, 0.0f, 0.0f, 1.5f, 3.0f},
                            1e-6f);
}

int test_add() {
  const aihw::Tensor a({2, 2}, {1.0f, 2.0f, 3.0f, 4.0f});
  const aihw::Tensor b({2, 2}, {0.5f, -1.0f, 2.0f, 3.0f});
  const aihw::Tensor c = aihw::add(a, b);
  return expect_vector_near("add", c.values(), {1.5f, 1.0f, 5.0f, 7.0f},
                            1e-6f);
}

int test_tiny_pipeline_shape() {
  const aihw::Tensor input({1, 4}, {1.0f, -2.0f, 3.0f, 0.5f});
  const aihw::Tensor w1({4, 8});
  const aihw::Tensor w2({8, 2});

  const aihw::Tensor hidden = aihw::relu(aihw::matmul(input, w1));
  const aihw::Tensor output = aihw::matmul(hidden, w2);

  if (hidden.rows() != 1 || hidden.cols() != 8 || output.rows() != 1 ||
      output.cols() != 2) {
    return fail("tiny pipeline output shape mismatch");
  }
  return 0;
}

}  // namespace

int main() {
  if (test_tensor_shape_and_size() || test_at2d() || test_runtime_matmul() ||
      test_relu() || test_add() || test_tiny_pipeline_shape()) {
    return 1;
  }

  std::cout << "tiny runtime tests passed\n";
  return 0;
}
