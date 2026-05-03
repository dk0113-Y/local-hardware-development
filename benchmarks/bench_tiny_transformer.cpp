#include "aihw/cpu_affinity.hpp"
#include "aihw/ops.hpp"

#include <cerrno>
#include <chrono>
#include <cstddef>
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
  std::size_t seq_len = 16;
  std::size_t model_dim = 32;
  std::size_t ff_dim = 64;
  int iterations = 20;
  bool causal = true;
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
      parsed > static_cast<unsigned long long>(
                   std::numeric_limits<unsigned>::max())) {
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

bool parse_causal(int argc, char** argv, int* index) {
  if (*index + 1 >= argc) {
    fail_argument("missing value for --causal");
  }
  const std::string value = argv[*index + 1];
  if (value != "0" && value != "1") {
    fail_argument("invalid --causal argument");
  }
  ++(*index);
  return value == "1";
}

Options parse_options(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--help") {
      options.show_help = true;
    } else if (arg == "--seq-len") {
      options.seq_len = parse_required_size(argc, argv, &i, arg);
    } else if (arg == "--model-dim") {
      options.model_dim = parse_required_size(argc, argv, &i, arg);
    } else if (arg == "--ff-dim") {
      options.ff_dim = parse_required_size(argc, argv, &i, arg);
    } else if (arg == "--iterations") {
      const std::size_t value = parse_required_size(argc, argv, &i, arg);
      if (value >
          static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        fail_argument("invalid --iterations argument");
      }
      options.iterations = static_cast<int>(value);
    } else if (arg == "--causal") {
      options.causal = parse_causal(argc, argv, &i);
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
            << "  bench_tiny_transformer [options]\n\n"
            << "Options:\n"
            << "  --seq-len N      Sequence length.\n"
            << "  --model-dim N    Model dimension.\n"
            << "  --ff-dim N       Feed-forward hidden dimension.\n"
            << "  --iterations N   Number of benchmark iterations.\n"
            << "  --causal 0|1     Enable causal mask. Default: 1.\n"
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
    const int centered = static_cast<int>(i % 23) - 11;
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
      make_patterned_tensor({options.seq_len, options.model_dim}, 0.01f);
  const aihw::TinyTransformerBlockWeights weights{
      make_patterned_tensor({options.model_dim, options.model_dim}, 0.002f),
      make_patterned_tensor({options.model_dim, options.model_dim}, 0.003f),
      make_patterned_tensor({options.model_dim, options.model_dim}, 0.004f),
      make_patterned_tensor({options.model_dim, options.model_dim}, 0.005f),
      make_patterned_tensor({options.model_dim, options.ff_dim}, 0.006f),
      make_patterned_tensor({options.ff_dim, options.model_dim}, 0.007f),
  };

  double best_ms = 1e100;
  float best_sink = 0.0f;
  for (int iter = 0; iter < options.iterations; ++iter) {
    const auto start = std::chrono::steady_clock::now();
    const aihw::Tensor output =
        aihw::transformer_block(input, weights, options.causal);
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

  std::cout << "section,tiny_transformer_affinity\n";
  std::cout << "requested_cpu,pin_requested,pin_success,before_group,"
               "before_processor,after_group,after_processor,error\n";
  std::cout << (options.pin_cpu ? static_cast<int>(options.requested_cpu) : -1)
            << "," << (affinity.requested ? 1 : 0) << ","
            << (affinity.success ? 1 : 0) << "," << affinity.before.group
            << "," << affinity.before.processor << "," << affinity.after.group
            << "," << affinity.after.processor << ","
            << csv_error(affinity.error) << "\n\n";

  std::cout << "section,tiny_transformer_benchmark\n";
  std::cout << "benchmark,seq_len,model_dim,ff_dim,causal,iterations,best_ms,"
               "sink\n";
  std::cout << "tiny_transformer_block," << options.seq_len << ","
            << options.model_dim << "," << options.ff_dim << ","
            << (options.causal ? 1 : 0) << "," << options.iterations << ","
            << std::fixed << std::setprecision(3) << best_ms << ","
            << best_sink << "\n";

  return static_cast<int>(g_sink == 123456.0f);
}
