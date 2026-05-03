#include "aihw/ops.hpp"

#include <iostream>

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
      {8, 2},
      {
          0.2f,  -0.1f,
          -0.3f, 0.4f,
          0.5f,  0.2f,
          -0.6f, 0.7f,
          0.1f,  -0.2f,
          0.3f,  0.6f,
          -0.4f, 0.8f,
          0.9f,  -0.5f,
      });

  const aihw::Tensor hidden = aihw::relu(aihw::matmul(input, w1));
  const aihw::Tensor output = aihw::matmul(hidden, w2);

  std::cout << "tiny_mlp\n";
  std::cout << "input_shape=" << input.rows() << "x" << input.cols() << "\n";
  std::cout << "hidden_shape=" << hidden.rows() << "x" << hidden.cols()
            << "\n";
  std::cout << "output_shape=" << output.rows() << "x" << output.cols()
            << "\n";
  std::cout << "output_values";
  for (float value : output.values()) {
    std::cout << "," << value;
  }
  std::cout << "\n";

  return 0;
}
