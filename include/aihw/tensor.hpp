#pragma once

#include <cstddef>
#include <vector>

namespace aihw {

class Tensor {
 public:
  Tensor();
  explicit Tensor(std::vector<std::size_t> shape);
  Tensor(std::vector<std::size_t> shape, std::vector<float> data);

  const std::vector<std::size_t>& shape() const;
  std::size_t ndim() const;
  std::size_t size() const;
  bool empty() const;

  float* data();
  const float* data() const;

  std::vector<float>& values();
  const std::vector<float>& values() const;

  float& operator[](std::size_t index);
  const float& operator[](std::size_t index) const;

  float& at2d(std::size_t row, std::size_t col);
  const float& at2d(std::size_t row, std::size_t col) const;

  std::size_t rows() const;
  std::size_t cols() const;

 private:
  std::vector<std::size_t> shape_;
  std::vector<float> data_;
};

}  // namespace aihw
