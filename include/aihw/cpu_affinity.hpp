#pragma once

#include <cstdint>
#include <string>

namespace aihw {

struct ProcessorLocation {
  unsigned group = 0;
  unsigned processor = 0;
  bool available = false;
};

struct PinCpuResult {
  bool requested = false;
  bool success = false;
  unsigned requested_cpu = 0;
  ProcessorLocation before;
  ProcessorLocation after;
  std::uint64_t previous_mask = 0;
  std::string error;
};

ProcessorLocation current_processor_location();

PinCpuResult pin_current_thread_to_cpu(unsigned logical_cpu);

}  // namespace aihw
