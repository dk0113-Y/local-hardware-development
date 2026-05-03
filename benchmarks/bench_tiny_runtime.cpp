#include "aihw/ops.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {

volatile float g_sink = 0.0f;

aihw::Tensor make_patterned_tensor(std::vector<std::size_t> shape,
                                   float scale) {
  aihw::Tensor tensor(shape);
  for (std::size_t i = 0; i < tensor.size(); ++i) {
    const int centered = static_cast<int>(i % 17) - 8;
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
  constexpr std::size_t batch = 1;
  constexpr std::size_t input_dim = 256;
  constexpr std::size_t hidden_dim = 512;
  constexpr std::size_t output_dim = 128;
  constexpr int iterations = 20;

  const aihw::Tensor input = make_patterned_tensor({batch, input_dim}, 0.01f);
  const aihw::Tensor w1 = make_patterned_tensor({input_dim, hidden_dim}, 0.002f);
  const aihw::Tensor w2 = make_patterned_tensor({hidden_dim, output_dim}, 0.003f);

  double best_ms = 1e100;
  float best_sink = 0.0f;
  for (int iter = 0; iter < iterations; ++iter) {
    const auto start = std::chrono::steady_clock::now();
    const aihw::Tensor hidden = aihw::relu(aihw::matmul(input, w1));
    const aihw::Tensor output = aihw::matmul(hidden, w2);
    const auto stop = std::chrono::steady_clock::now();

    const float current_sink = checksum(output);
    g_sink = current_sink;
    const double ms =
        std::chrono::duration<double, std::milli>(stop - start).count();
    if (ms < best_ms) {
      best_ms = ms;
      best_sink = current_sink;
    }
  }

  std::cout << "benchmark,batch,input_dim,hidden_dim,output_dim,iterations,"
               "best_ms,sink\n";
  std::cout << "tiny_mlp_runtime," << batch << "," << input_dim << ","
            << hidden_dim << "," << output_dim << "," << iterations << ","
            << std::fixed << std::setprecision(3) << best_ms << ","
            << best_sink << "\n";

  return static_cast<int>(g_sink == 123456.0f);
}
