#include "aihw/cpu_affinity.hpp"

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <windows.h>
#endif

#include <limits>
#include <sstream>

namespace aihw {

namespace {

#ifdef _WIN32

std::string windows_error_message(const std::string& prefix, DWORD error) {
  std::ostringstream out;
  out << prefix << " failed with error " << error;
  return out.str();
}

#endif

}  // namespace

ProcessorLocation current_processor_location() {
#ifdef _WIN32
  PROCESSOR_NUMBER number{};
  GetCurrentProcessorNumberEx(&number);
  return {static_cast<unsigned>(number.Group),
          static_cast<unsigned>(number.Number), true};
#else
  return {};
#endif
}

PinCpuResult pin_current_thread_to_cpu(unsigned logical_cpu) {
  PinCpuResult result;
  result.requested = true;
  result.requested_cpu = logical_cpu;
  result.before = current_processor_location();

#ifdef _WIN32
  constexpr unsigned mask_bits = sizeof(DWORD_PTR) * 8;
  if (logical_cpu >= mask_bits) {
    result.after = current_processor_location();
    result.error = "logical CPU exceeds affinity mask width";
    return result;
  }

  const DWORD active_processors = GetActiveProcessorCount(0);
  if (logical_cpu >= active_processors) {
    result.after = current_processor_location();
    result.error = "logical CPU is outside processor group 0 active count";
    return result;
  }

  const DWORD_PTR mask = DWORD_PTR{1} << logical_cpu;
  const DWORD_PTR previous =
      SetThreadAffinityMask(GetCurrentThread(), static_cast<DWORD_PTR>(mask));
  if (previous == 0) {
    result.after = current_processor_location();
    result.error =
        windows_error_message("SetThreadAffinityMask", GetLastError());
    return result;
  }

  result.success = true;
  result.previous_mask = static_cast<std::uint64_t>(previous);
  result.after = current_processor_location();
  return result;
#else
  result.after = current_processor_location();
  result.error = "CPU pinning is only supported on Windows in this build.";
  return result;
#endif
}

}  // namespace aihw
