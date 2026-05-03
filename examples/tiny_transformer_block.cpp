#include "aihw/ops.hpp"

#include <cstddef>
#include <iostream>
#include <vector>

namespace {

aihw::Tensor make_patterned_tensor(std::vector<std::size_t> shape,
                                   float scale) {
  aihw::Tensor tensor(shape);
  for (std::size_t i = 0; i < tensor.size(); ++i) {
    const int centered = static_cast<int>(i % 19) - 9;
    tensor[i] = static_cast<float>(centered) * scale;
  }
  return tensor;
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
  const std::size_t seq_len = 4;
  const std::size_t model_dim = 8;
  const std::size_t ff_dim = 16;

  const aihw::Tensor input =
      make_patterned_tensor({seq_len, model_dim}, 0.02f);
  const aihw::TinyTransformerBlockWeights weights{
      make_patterned_tensor({model_dim, model_dim}, 0.010f),
      make_patterned_tensor({model_dim, model_dim}, 0.011f),
      make_patterned_tensor({model_dim, model_dim}, 0.012f),
      make_patterned_tensor({model_dim, model_dim}, 0.013f),
      make_patterned_tensor({model_dim, ff_dim}, 0.014f),
      make_patterned_tensor({ff_dim, model_dim}, 0.015f),
  };

  const aihw::Tensor output = aihw::transformer_block(input, weights, true);

  std::cout << "tiny_transformer_block\n";
  std::cout << "input_shape=" << input.rows() << "x" << input.cols() << "\n";
  std::cout << "output_shape=" << output.rows() << "x" << output.cols()
            << "\n";
  std::cout << "output_checksum=" << checksum(output) << "\n";
  std::cout << "first_row";
  for (std::size_t col = 0; col < output.cols(); ++col) {
    std::cout << "," << output.at2d(0, col);
  }
  std::cout << "\n";

  return 0;
}
