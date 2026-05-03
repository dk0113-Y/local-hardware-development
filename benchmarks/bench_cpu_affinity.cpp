#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <windows.h>
#ifndef LTP_PC_SMT
#define LTP_PC_SMT 0x1
#endif
#endif

namespace {

volatile std::uint64_t g_sink = 0;

struct Options {
  bool quick = false;
  bool single_cpu = false;
  unsigned cpu = 0;
};

struct ThreadPosition {
  unsigned short group = 0;
  unsigned char processor = 0;
};

struct BenchResult {
  double best_ms = 0.0;
  double ns_per_access = 0.0;
  double gb_per_s = 0.0;
  std::uint64_t sink = 0;
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
    if (arg == "--quick") {
      options.quick = true;
    } else if (arg == "--all") {
      options.single_cpu = false;
    } else if (arg == "--cpu") {
      if (i + 1 >= argc || !parse_uint(argv[i + 1], &options.cpu)) {
        std::cerr << "invalid --cpu argument\n";
        std::exit(2);
      }
      options.single_cpu = true;
      ++i;
    } else {
      std::cerr << "unknown argument: " << arg << "\n";
      std::exit(2);
    }
  }
  return options;
}

std::vector<std::uint64_t> make_values(std::size_t count) {
  std::vector<std::uint64_t> values(count);
  std::iota(values.begin(), values.end(), std::uint64_t{1});
  return values;
}

BenchResult run_sequential_read(bool quick) {
  const std::size_t bytes = quick ? 8ull * 1024ull * 1024ull
                                  : 64ull * 1024ull * 1024ull;
  const int iterations = quick ? 1 : 3;
  const std::size_t count = bytes / sizeof(std::uint64_t);
  const auto values = make_values(count);

  double best_ms = 1e100;
  std::uint64_t best_sink = 0;
  for (int iter = 0; iter < iterations; ++iter) {
    std::uint64_t sum = 0;
    const auto start = std::chrono::steady_clock::now();
    for (std::uint64_t value : values) {
      sum += value;
    }
    const auto stop = std::chrono::steady_clock::now();
    g_sink = sum;

    const double ms =
        std::chrono::duration<double, std::milli>(stop - start).count();
    if (ms < best_ms) {
      best_ms = ms;
      best_sink = sum;
    }
  }

  const double ns_per_access = best_ms * 1.0e6 / static_cast<double>(count);
  const double gb_per_s =
      (static_cast<double>(count * sizeof(std::uint64_t)) / (best_ms / 1000.0)) /
      1.0e9;
  return {best_ms, ns_per_access, gb_per_s, best_sink};
}

#ifdef _WIN32

ThreadPosition current_thread_position() {
  PROCESSOR_NUMBER number{};
  GetCurrentProcessorNumberEx(&number);
  return {number.Group, number.Number};
}

std::string mask_to_hex(KAFFINITY mask) {
  std::ostringstream out;
  out << "0x" << std::hex << std::setw(sizeof(KAFFINITY) * 2)
      << std::setfill('0') << static_cast<unsigned long long>(mask);
  return out.str();
}

std::string logical_processors_from_mask(KAFFINITY mask) {
  std::ostringstream out;
  bool first = true;
  for (unsigned bit = 0; bit < sizeof(KAFFINITY) * 8; ++bit) {
    if ((mask & (KAFFINITY{1} << bit)) != 0) {
      if (!first) {
        out << ";";
      }
      out << bit;
      first = false;
    }
  }
  return out.str();
}

void print_current_thread_section() {
  const ThreadPosition pos = current_thread_position();
  std::cout << "section,current_thread\n";
  std::cout << "group,processor\n";
  std::cout << pos.group << "," << static_cast<unsigned>(pos.processor)
            << "\n\n";
}

void print_topology_section() {
  DWORD bytes = 0;
  GetLogicalProcessorInformationEx(RelationProcessorCore, nullptr, &bytes);
  std::vector<unsigned char> buffer(bytes);
  if (!GetLogicalProcessorInformationEx(
          RelationProcessorCore,
          reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(
              buffer.data()),
          &bytes)) {
    std::cerr << "GetLogicalProcessorInformationEx failed: "
              << GetLastError() << "\n";
    return;
  }

  std::cout << "section,cpu_topology\n";
  std::cout << "core_index,efficiency_class,flags,smt_hint,group,mask_hex,"
               "logical_processors\n";

  DWORD offset = 0;
  unsigned core_index = 0;
  while (offset < bytes) {
    const auto* info =
        reinterpret_cast<const SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*>(
            buffer.data() + offset);
    const PROCESSOR_RELATIONSHIP& processor = info->Processor;
    const int smt_hint = (processor.Flags & LTP_PC_SMT) ? 1 : 0;
    for (WORD group_index = 0; group_index < processor.GroupCount;
         ++group_index) {
      const GROUP_AFFINITY& affinity = processor.GroupMask[group_index];
      std::cout << core_index << ","
                << static_cast<unsigned>(processor.EfficiencyClass) << ","
                << static_cast<unsigned>(processor.Flags) << "," << smt_hint
                << "," << affinity.Group << "," << mask_to_hex(affinity.Mask)
                << ",\"" << logical_processors_from_mask(affinity.Mask)
                << "\"\n";
    }
    offset += info->Size;
    ++core_index;
  }
  std::cout << "\n";
}

bool pin_current_thread_to_logical_processor(unsigned logical_cpu,
                                             DWORD_PTR* previous_mask) {
  const unsigned max_bits = sizeof(DWORD_PTR) * 8;
  if (logical_cpu >= max_bits) {
    std::cerr << "logical_cpu " << logical_cpu << " exceeds affinity mask width "
              << max_bits << "\n";
    return false;
  }

  const DWORD active = GetActiveProcessorCount(0);
  if (logical_cpu >= active) {
    std::cerr << "logical_cpu " << logical_cpu
              << " is outside active processor count " << active
              << " in group 0\n";
    return false;
  }

  const DWORD_PTR mask = DWORD_PTR{1} << logical_cpu;
  const DWORD_PTR old_mask = SetThreadAffinityMask(GetCurrentThread(), mask);
  if (old_mask == 0) {
    std::cerr << "SetThreadAffinityMask failed for logical_cpu=" << logical_cpu
              << ", error=" << GetLastError() << "\n";
    return false;
  }
  if (previous_mask != nullptr && *previous_mask == 0) {
    *previous_mask = old_mask;
  }
  return true;
}

void run_affinity_sweep(const Options& options) {
  const DWORD active = GetActiveProcessorCount(0);
  const unsigned begin_cpu = options.single_cpu ? options.cpu : 0;
  const unsigned end_cpu = options.single_cpu ? options.cpu + 1 : active;

  std::cout << "section,affinity_sweep\n";
  std::cout << "logical_cpu,before_group,before_processor,after_group,"
               "after_processor,best_ms,ns_per_access,gb_per_s,sink\n";

  DWORD_PTR original_mask = 0;
  for (unsigned cpu = begin_cpu; cpu < end_cpu; ++cpu) {
    const ThreadPosition before = current_thread_position();
    const bool pinned = pin_current_thread_to_logical_processor(cpu, &original_mask);
    const ThreadPosition after = current_thread_position();
    if (!pinned) {
      std::cout << cpu << "," << before.group << ","
                << static_cast<unsigned>(before.processor) << ","
                << after.group << "," << static_cast<unsigned>(after.processor)
                << ",nan,nan,nan,0\n";
      continue;
    }

    const BenchResult result = run_sequential_read(options.quick);
    std::cout << cpu << "," << before.group << ","
              << static_cast<unsigned>(before.processor) << "," << after.group
              << "," << static_cast<unsigned>(after.processor) << ","
              << std::fixed << std::setprecision(3) << result.best_ms << ","
              << result.ns_per_access << "," << result.gb_per_s << ","
              << result.sink << "\n";
  }

  if (original_mask != 0) {
    SetThreadAffinityMask(GetCurrentThread(), original_mask);
  }
}

#endif

}  // namespace

int main(int argc, char** argv) {
  const Options options = parse_options(argc, argv);

#ifdef _WIN32
  print_current_thread_section();
  print_topology_section();
  run_affinity_sweep(options);
  return static_cast<int>(g_sink == 0xFFFFFFFFFFFFFFFFull);
#else
  (void)options;
  std::cout
      << "bench_cpu_affinity is only supported on Windows in this first version.\n";
  return 0;
#endif
}
