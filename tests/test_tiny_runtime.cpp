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

float row_sum(const aihw::Tensor& tensor, std::size_t row) {
  float sum = 0.0f;
  for (std::size_t col = 0; col < tensor.cols(); ++col) {
    sum += tensor.at2d(row, col);
  }
  return sum;
}

float row_mean(const aihw::Tensor& tensor, std::size_t row) {
  return row_sum(tensor, row) / static_cast<float>(tensor.cols());
}

float row_variance(const aihw::Tensor& tensor, std::size_t row) {
  const float mean = row_mean(tensor, row);
  float variance = 0.0f;
  for (std::size_t col = 0; col < tensor.cols(); ++col) {
    const float centered = tensor.at2d(row, col) - mean;
    variance += centered * centered;
  }
  return variance / static_cast<float>(tensor.cols());
}

float vector_mean(const aihw::Tensor& tensor) {
  float sum = 0.0f;
  for (float value : tensor.values()) {
    sum += value;
  }
  return sum / static_cast<float>(tensor.size());
}

float vector_variance(const aihw::Tensor& tensor) {
  const float mean = vector_mean(tensor);
  float variance = 0.0f;
  for (float value : tensor.values()) {
    const float centered = value - mean;
    variance += centered * centered;
  }
  return variance / static_cast<float>(tensor.size());
}

int test_softmax_1d_known_values() {
  const aihw::Tensor x({3}, {1.0f, 2.0f, 3.0f});
  const aihw::Tensor y = aihw::softmax(x);
  const float e1 = std::exp(-2.0f);
  const float e2 = std::exp(-1.0f);
  const float e3 = 1.0f;
  const float sum = e1 + e2 + e3;
  return expect_vector_near("softmax 1D", y.values(),
                            {e1 / sum, e2 / sum, e3 / sum}, 1e-5f);
}

int test_softmax_2d_row_sums() {
  const aihw::Tensor x({2, 3}, {1.0f, 2.0f, 3.0f, -2.0f, 0.0f, 2.0f});
  const aihw::Tensor y = aihw::softmax(x);

  if (y.rows() != 2 || y.cols() != 3) {
    return fail("softmax 2D shape mismatch");
  }
  if (!close_enough(row_sum(y, 0), 1.0f, 1e-5f) ||
      !close_enough(row_sum(y, 1), 1.0f, 1e-5f)) {
    return fail("softmax 2D row sum mismatch");
  }
  return 0;
}

int test_layer_norm_1d_properties() {
  const aihw::Tensor x({4}, {1.0f, 2.0f, 3.0f, 4.0f});
  const aihw::Tensor y = aihw::layer_norm(x);

  if (y.shape() != x.shape()) {
    return fail("layer_norm 1D shape mismatch");
  }
  if (!close_enough(vector_mean(y), 0.0f, 1e-5f) ||
      !close_enough(vector_variance(y), 1.0f, 1e-4f)) {
    return fail("layer_norm 1D properties mismatch");
  }
  return 0;
}

int test_layer_norm_2d_properties() {
  const aihw::Tensor x({2, 4},
                       {1.0f, 2.0f, 3.0f, 4.0f, -3.0f, -1.0f, 1.0f, 3.0f});
  const aihw::Tensor y = aihw::layer_norm(x);

  if (y.rows() != 2 || y.cols() != 4) {
    return fail("layer_norm 2D shape mismatch");
  }
  for (std::size_t row = 0; row < y.rows(); ++row) {
    if (!close_enough(row_mean(y, row), 0.0f, 1e-5f) ||
        !close_enough(row_variance(y, row), 1.0f, 1e-4f)) {
      return fail("layer_norm 2D row properties mismatch");
    }
  }
  return 0;
}

int test_tiny_classifier_pipeline_shape() {
  const aihw::Tensor input({1, 4}, {1.0f, -2.0f, 3.0f, 0.5f});
  const aihw::Tensor w1({4, 8});
  const aihw::Tensor w2({8, 3});

  const aihw::Tensor hidden = aihw::relu(aihw::matmul(input, w1));
  const aihw::Tensor logits = aihw::matmul(hidden, w2);
  const aihw::Tensor normalized = aihw::layer_norm(logits);
  const aihw::Tensor probabilities = aihw::softmax(normalized);

  if (hidden.rows() != 1 || hidden.cols() != 8 || logits.rows() != 1 ||
      logits.cols() != 3 || normalized.rows() != 1 ||
      normalized.cols() != 3 || probabilities.rows() != 1 ||
      probabilities.cols() != 3) {
    return fail("tiny classifier pipeline output shape mismatch");
  }
  return 0;
}

int test_tiny_classifier_pipeline_probability_sum() {
  const aihw::Tensor input({2, 4},
                           {1.0f, -2.0f, 3.0f, 0.5f, 0.5f, 1.0f, -1.5f,
                            2.0f});
  const aihw::Tensor w1(
      {4, 5},
      {0.1f, -0.2f, 0.3f, 0.4f, -0.5f, 0.2f, 0.1f, -0.4f, 0.3f, 0.7f,
       -0.3f, 0.5f, 0.2f, -0.1f, 0.4f, 0.6f, -0.7f, 0.8f, -0.9f, 0.2f});
  const aihw::Tensor w2({5, 3},
                        {0.2f, -0.1f, 0.3f, -0.3f, 0.4f, -0.2f, 0.5f,
                         0.2f, 0.1f, -0.6f, 0.7f, 0.4f, 0.1f, -0.2f,
                         0.5f});

  const aihw::Tensor hidden = aihw::relu(aihw::matmul(input, w1));
  const aihw::Tensor logits = aihw::matmul(hidden, w2);
  const aihw::Tensor probabilities = aihw::softmax(aihw::layer_norm(logits));

  if (!close_enough(row_sum(probabilities, 0), 1.0f, 1e-5f) ||
      !close_enough(row_sum(probabilities, 1), 1.0f, 1e-5f)) {
    return fail("tiny classifier probability row sum mismatch");
  }
  return 0;
}

}  // namespace

int main() {
  if (test_tensor_shape_and_size() || test_at2d() || test_runtime_matmul() ||
      test_relu() || test_add() || test_softmax_1d_known_values() ||
      test_softmax_2d_row_sums() || test_layer_norm_1d_properties() ||
      test_layer_norm_2d_properties() || test_tiny_classifier_pipeline_shape() ||
      test_tiny_classifier_pipeline_probability_sum()) {
    return 1;
  }

  std::cout << "tiny runtime tests passed\n";
  return 0;
}
