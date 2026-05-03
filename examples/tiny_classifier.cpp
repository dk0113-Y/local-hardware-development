#include "aihw/ops.hpp"

#include <iostream>

namespace {

void print_shape(const char* name, const aihw::Tensor& tensor) {
  std::cout << name << "=";
  if (tensor.ndim() == 1) {
    std::cout << tensor.size();
  } else {
    std::cout << tensor.rows() << "x" << tensor.cols();
  }
  std::cout << "\n";
}

float checksum(const aihw::Tensor& tensor) {
  float sum = 0.0f;
  for (float value : tensor.values()) {
    sum += value;
  }
  return sum;
}

}  // namespace

int main() {
  const aihw::Tensor input({1, 4}, {1.0f, -2.0f, 3.0f, 0.5f});
  const aihw::Tensor w1(
      {4, 8},
      {
          0.1f,  -0.2f, 0.3f,  0.4f,  -0.5f, 0.6f,  -0.7f, 0.8f,
          0.2f,  0.1f,  -0.4f, 0.3f,  0.7f,  -0.6f, 0.5f,  -0.8f,
          -0.3f, 0.5f,  0.2f,  -0.1f, 0.4f,  0.9f,  -0.2f, 0.1f,
          0.6f,  -0.7f, 0.8f,  -0.9f, 0.2f,  -0.1f, 0.3f,  -0.4f,
      });
  const aihw::Tensor w2(
      {8, 3},
      {
          0.2f,  -0.1f, 0.3f,
          -0.3f, 0.4f,  -0.2f,
          0.5f,  0.2f,  0.1f,
          -0.6f, 0.7f,  0.4f,
          0.1f,  -0.2f, 0.5f,
          0.3f,  0.6f,  -0.4f,
          -0.4f, 0.8f,  0.2f,
          0.9f,  -0.5f, 0.6f,
      });

  const aihw::Tensor hidden = aihw::relu(aihw::matmul(input, w1));
  const aihw::Tensor logits = aihw::matmul(hidden, w2);
  const aihw::Tensor normalized = aihw::layer_norm(logits);
  const aihw::Tensor probabilities = aihw::softmax(normalized);

  std::cout << "tiny_classifier\n";
  print_shape("input_shape", input);
  print_shape("hidden_shape", hidden);
  print_shape("logits_shape", logits);
  print_shape("probabilities_shape", probabilities);
  std::cout << "probabilities";
  for (float value : probabilities.values()) {
    std::cout << "," << value;
  }
  std::cout << "\n";
  std::cout << "sum_probabilities=" << checksum(probabilities) << "\n";

  return 0;
}
