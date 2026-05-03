#include "aihw/tensor.hpp"

#include <numeric>
#include <stdexcept>
#include <utility>

namespace aihw {

namespace {

std::size_t element_count(const std::vector<std::size_t>& shape) {
  if (shape.empty()) {
    return 0;
  }
  return std::accumulate(shape.begin(), shape.end(), std::size_t{1},
                         [](std::size_t lhs, std::size_t rhs) {
                           if (rhs == 0) {
                             throw std::invalid_argument(
                                 "tensor shape dimensions must be nonzero");
                           }
                           return lhs * rhs;
                         });
}

void validate_supported_shape(const std::vector<std::size_t>& shape) {
  if (shape.size() > 2) {
    throw std::invalid_argument("Tensor only supports 1D and 2D shapes");
  }
  (void)element_count(shape);
}

}  // namespace

Tensor::Tensor() = default;

Tensor::Tensor(std::vector<std::size_t> shape) : shape_(std::move(shape)) {
  validate_supported_shape(shape_);
  data_.assign(element_count(shape_), 0.0f);
}

Tensor::Tensor(std::vector<std::size_t> shape, std::vector<float> data)
    : shape_(std::move(shape)), data_(std::move(data)) {
  validate_supported_shape(shape_);
  if (element_count(shape_) != data_.size()) {
    throw std::invalid_argument("tensor data size does not match shape");
  }
}

const std::vector<std::size_t>& Tensor::shape() const {
  return shape_;
}

std::size_t Tensor::ndim() const {
  return shape_.size();
}

std::size_t Tensor::size() const {
  return data_.size();
}

bool Tensor::empty() const {
  return data_.empty();
}

float* Tensor::data() {
  return data_.data();
}

const float* Tensor::data() const {
  return data_.data();
}

std::vector<float>& Tensor::values() {
  return data_;
}

const std::vector<float>& Tensor::values() const {
  return data_;
}

float& Tensor::operator[](std::size_t index) {
  return data_.at(index);
}

const float& Tensor::operator[](std::size_t index) const {
  return data_.at(index);
}

float& Tensor::at2d(std::size_t row, std::size_t col) {
  if (ndim() != 2) {
    throw std::invalid_argument("at2d requires a 2D tensor");
  }
  if (row >= rows() || col >= cols()) {
    throw std::out_of_range("at2d index is out of range");
  }
  return data_[row * cols() + col];
}

const float& Tensor::at2d(std::size_t row, std::size_t col) const {
  if (ndim() != 2) {
    throw std::invalid_argument("at2d requires a 2D tensor");
  }
  if (row >= rows() || col >= cols()) {
    throw std::out_of_range("at2d index is out of range");
  }
  return data_[row * cols() + col];
}

std::size_t Tensor::rows() const {
  if (ndim() != 2) {
    throw std::invalid_argument("rows requires a 2D tensor");
  }
  return shape_[0];
}

std::size_t Tensor::cols() const {
  if (ndim() != 2) {
    throw std::invalid_argument("cols requires a 2D tensor");
  }
  return shape_[1];
}

}  // namespace aihw
