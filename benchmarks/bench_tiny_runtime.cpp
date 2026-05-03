#include "aihw/cpu_affinity.hpp"
#include "aihw/ops.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cerrno>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

volatile float g_sink = 0.0f;

struct Options {
  bool show_help = false;
  bool pin_cpu = false;
  unsigned requested_cpu = 0;
  std::size_t batch = 1;
  std::size_t input_dim = 256;
  std::size_t hidden_dim = 512;
  std::size_t output_dim = 128;
  int iterations = 20;
};

bool parse_positive_size(const char* text, std::size_t* value) {
  if (text == nullptr || text[0] == '-' || text[0] == '\0') {
    return false;
  }
  char* end = nullptr;
  errno = 0;
  const unsigned long long parsed = std::strtoull(text, &end, 10);
  if (end == text || *end != '\0' || errno == ERANGE || parsed == 0 ||
      parsed > static_cast<unsigned long long>(
                   std::numeric_limits<std::size_t>::max())) {
    return false;
  }
  *value = static_cast<std::size_t>(parsed);
  return true;
}

bool parse_uint(const char* text, unsigned* value) {
  if (text == nullptr || text[0] == '-' || text[0] == '\0') {
    return false;
  }
  char* end = nullptr;
  errno = 0;
  const unsigned long long parsed = std::strtoull(text, &end, 10);
  if (end == text || *end != '\0' || errno == ERANGE ||
      parsed > static_cast<std::size_t>(std::numeric_limits<unsigned>::max())) {
    return false;
  }
  *value = static_cast<unsigned>(parsed);
  return true;
}

void fail_argument(const std::string& message) {
  std::cerr << message << "\n";
  std::exit(2);
}

std::size_t parse_required_size(int argc,
                                char** argv,
                                int* index,
                                const std::string& name) {
  if (*index + 1 >= argc) {
    fail_argument("missing value for " + name);
  }
  std::size_t value = 0;
  if (!parse_positive_size(argv[*index + 1], &value)) {
    fail_argument("invalid " + name + " argument");
  }
  ++(*index);
  return value;
}

Options parse_options(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help") {
      options.show_help = true;
    } else if (arg == "--batch") {
      options.batch = parse_required_size(argc, argv, &i, arg);
    } else if (arg == "--input-dim") {
      options.input_dim = parse_required_size(argc, argv, &i, arg);
    } else if (arg == "--hidden-dim") {
      options.hidden_dim = parse_required_size(argc, argv, &i, arg);
    } else if (arg == "--output-dim") {
      options.output_dim = parse_required_size(argc, argv, &i, arg);
    } else if (arg == "--iterations") {
      const std::size_t value = parse_required_size(argc, argv, &i, arg);
      if (value >
          static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        fail_argument("invalid --iterations argument");
      }
      options.iterations = static_cast<int>(value);
    } else if (arg == "--pin-cpu") {
      if (i + 1 >= argc ||
          !parse_uint(argv[i + 1], &options.requested_cpu)) {
        fail_argument("invalid --pin-cpu argument");
      }
      options.pin_cpu = true;
      ++i;
    } else {
      fail_argument("unknown argument: " + arg);
    }
  }
  return options;
}

void print_usage() {
  std::cout << "Usage:\n"
            << "  bench_tiny_runtime [options]\n\n"
            << "Options:\n"
            << "  --batch N        Batch size.\n"
            << "  --input-dim N    Input dimension.\n"
            << "  --hidden-dim N   Hidden dimension.\n"
            << "  --output-dim N   Output dimension.\n"
            << "  --iterations N   Number of benchmark iterations.\n"
            << "  --pin-cpu N      Pin current benchmark thread to logical CPU N "
               "on Windows.\n"
            << "  --help           Show this help.\n";
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

  aihw::PinCpuResult affinity;
  if (options.pin_cpu) {
    affinity = aihw::pin_current_thread_to_cpu(options.requested_cpu);
  } else {
    affinity.before = aihw::current_processor_location();
    affinity.after = aihw::current_processor_location();
  }

  const aihw::Tensor input =
      make_patterned_tensor({options.batch, options.input_dim}, 0.01f);
  const aihw::Tensor w1 =
      make_patterned_tensor({options.input_dim, options.hidden_dim}, 0.002f);
  const aihw::Tensor w2 =
      make_patterned_tensor({options.hidden_dim, options.output_dim}, 0.003f);

  double best_ms = 1e100;
  float best_sink = 0.0f;
  for (int iter = 0; iter < options.iterations; ++iter) {
    const auto start = std::chrono::steady_clock::now();
    const aihw::Tensor hidden = aihw::relu(aihw::matmul(input, w1));
    const aihw::Tensor logits = aihw::matmul(hidden, w2);
    const aihw::Tensor probabilities = aihw::softmax(aihw::layer_norm(logits));
    const auto stop = std::chrono::steady_clock::now();

    const float current_sink = checksum(probabilities);
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
  std::cout << "tiny_classifier_runtime," << options.batch << ","
            << options.input_dim << "," << options.hidden_dim << ","
            << options.output_dim << "," << options.iterations << ","
            << std::fixed << std::setprecision(3) << best_ms << ","
            << best_sink << "\n";

  return static_cast<int>(g_sink == 123456.0f);
}
