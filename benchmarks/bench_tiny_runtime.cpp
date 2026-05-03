#include "aihw/cpu_affinity.hpp"
#include "aihw/ops.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

volatile float g_sink = 0.0f;

struct Options {
  bool show_help = false;
  bool pin_cpu = false;
  unsigned requested_cpu = 0;
};

bool parse_uint(const char* text, unsigned* value) {
  char* end = nullptr;
  const unsigned long parsed = std::strtoul(text, &end, 10);
  if (end == text || *end != '\0') {
    return false;
  }
  *value = static_cast<unsigned>(parsed);
  return true;
}

Options parse_options(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help") {
      options.show_help = true;
    } else if (arg == "--pin-cpu") {
      if (i + 1 >= argc ||
          !parse_uint(argv[i + 1], &options.requested_cpu)) {
        std::cerr << "invalid --pin-cpu argument\n";
        std::exit(2);
      }
      options.pin_cpu = true;
      ++i;
    } else {
      std::cerr << "unknown argument: " << arg << "\n";
      std::exit(2);
    }
  }
  return options;
}

void print_usage() {
  std::cout << "Usage:\n"
            << "  bench_tiny_runtime [--pin-cpu N] [--help]\n\n"
            << "Options:\n"
            << "  --pin-cpu N  Pin current benchmark thread to logical CPU N "
               "on Windows.\n"
            << "  --help       Show this help.\n";
}

std::string csv_error(std::string value) {
  for (char& ch : value) {
    if (ch == ',') {
      ch = ' ';
    }
  }
  return value;
}

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

int main(int argc, char** argv) {
  const Options options = parse_options(argc, argv);
  if (options.show_help) {
    print_usage();
    return 0;
  }

  constexpr std::size_t batch = 1;
  constexpr std::size_t input_dim = 256;
  constexpr std::size_t hidden_dim = 512;
  constexpr std::size_t output_dim = 128;
  constexpr int iterations = 20;

  aihw::PinCpuResult affinity;
  if (options.pin_cpu) {
    affinity = aihw::pin_current_thread_to_cpu(options.requested_cpu);
  } else {
    affinity.before = aihw::current_processor_location();
    affinity.after = aihw::current_processor_location();
  }

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

  std::cout << "section,tiny_runtime_affinity\n";
  std::cout << "requested_cpu,pin_requested,pin_success,before_group,"
               "before_processor,after_group,after_processor,error\n";
  std::cout << (options.pin_cpu ? static_cast<int>(options.requested_cpu) : -1)
            << "," << (affinity.requested ? 1 : 0) << ","
            << (affinity.success ? 1 : 0) << "," << affinity.before.group
            << "," << affinity.before.processor << "," << affinity.after.group
            << "," << affinity.after.processor << ","
            << csv_error(affinity.error) << "\n\n";

  std::cout << "section,tiny_runtime_benchmark\n";
  std::cout << "benchmark,batch,input_dim,hidden_dim,output_dim,iterations,"
               "best_ms,sink\n";
  std::cout << "tiny_mlp_runtime," << batch << "," << input_dim << ","
            << hidden_dim << "," << output_dim << "," << iterations << ","
            << std::fixed << std::setprecision(3) << best_ms << ","
            << best_sink << "\n";

  return static_cast<int>(g_sink == 123456.0f);
}
