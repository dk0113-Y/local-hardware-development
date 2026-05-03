#pragma once

#include "aihw/tensor.hpp"

namespace aihw {

Tensor matmul(const Tensor& a, const Tensor& b);
Tensor relu(const Tensor& x);
Tensor add(const Tensor& a, const Tensor& b);
Tensor softmax(const Tensor& x);
Tensor layer_norm(const Tensor& x, float epsilon = 1e-5f);

struct TinyTransformerBlockWeights {
  Tensor w_q;
  Tensor w_k;
  Tensor w_v;
  Tensor w_o;
  Tensor w_ff1;
  Tensor w_ff2;
};

Tensor scaled_dot_product_attention(const Tensor& q,
                                    const Tensor& k,
                                    const Tensor& v,
                                    bool causal = true);

Tensor transformer_block(const Tensor& x,
                         const TinyTransformerBlockWeights& weights,
                         bool causal = true);

}  // namespace aihw
