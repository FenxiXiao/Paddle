/* Copyright (c) 2021 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include <gtest/gtest.h>
#include <memory>

#include "paddle/pten/backends/cpu/cpu_context.h"
#include "paddle/pten/kernels/math_kernel.h"

#include "paddle/fluid/memory/allocation/allocator_facade.h"
#include "paddle/pten/api/lib/utils/allocator.h"
#include "paddle/pten/core/dense_tensor.h"
#include "paddle/pten/core/kernel_registry.h"

namespace pten {
namespace tests {

namespace framework = paddle::framework;
using DDim = pten::DDim;

TEST(DEV_API, add) {
  // 1. create tensor
  const auto alloc = std::make_unique<paddle::experimental::DefaultAllocator>(
      paddle::platform::CPUPlace());
  pten::DenseTensor dense_x(alloc.get(),
                            pten::DenseTensorMeta(pten::DataType::FLOAT32,
                                                  pten::make_ddim({3, 10}),
                                                  pten::DataLayout::NCHW));
  auto* dense_x_data =
      dense_x.mutable_data<float>(paddle::platform::CPUPlace());

  pten::DenseTensor dense_y(alloc.get(),
                            pten::DenseTensorMeta(pten::DataType::FLOAT32,
                                                  pten::make_ddim({10}),
                                                  pten::DataLayout::NCHW));
  auto* dense_y_data =
      dense_y.mutable_data<float>(paddle::platform::CPUPlace());

  float sum[3][10] = {0.0};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 10; ++j) {
      dense_x_data[i * 10 + j] = (i * 10 + j) * 1.0;
      sum[i][j] = (i * 10 + j) * 1.0 + j * 2.0;
    }
  }
  for (size_t i = 0; i < 10; ++i) {
    dense_y_data[i] = i * 2.0;
  }

  // 2. test API
  pten::CPUContext dev_ctx;
  dev_ctx.SetAllocator(paddle::memory::allocation::AllocatorFacade::Instance()
                           .GetAllocator(paddle::platform::CPUPlace())
                           .get());
  dev_ctx.Init();
  auto dense_out = pten::Add<float>(dev_ctx, dense_x, dense_y);

  // 3. check result
  ASSERT_EQ(dense_out.dims().size(), 2);
  ASSERT_EQ(dense_out.dims()[0], 3);
  ASSERT_EQ(dense_out.dtype(), pten::DataType::FLOAT32);
  ASSERT_EQ(dense_out.layout(), pten::DataLayout::NCHW);

  auto expect_result = sum;
  auto actual_result0 = dense_out.data<float>()[0];
  auto actual_result1 = dense_out.data<float>()[1];
  auto actual_result2 = dense_out.data<float>()[10];
  ASSERT_NEAR(expect_result[0][0], actual_result0, 1e-6f);
  ASSERT_NEAR(expect_result[0][1], actual_result1, 1e-6f);
  ASSERT_NEAR(expect_result[1][0], actual_result2, 1e-6f);
}

TEST(DEV_API, subtract) {
  // 1. create tensor
  const auto alloc = std::make_unique<paddle::experimental::DefaultAllocator>(
      paddle::platform::CPUPlace());
  pten::DenseTensor dense_x(alloc.get(),
                            pten::DenseTensorMeta(pten::DataType::FLOAT32,
                                                  pten::make_ddim({3, 10}),
                                                  pten::DataLayout::NCHW));
  auto* dense_x_data =
      dense_x.mutable_data<float>(paddle::platform::CPUPlace());

  pten::DenseTensor dense_y(alloc.get(),
                            pten::DenseTensorMeta(pten::DataType::FLOAT32,
                                                  pten::make_ddim({10}),
                                                  pten::DataLayout::NCHW));
  auto* dense_y_data =
      dense_y.mutable_data<float>(paddle::platform::CPUPlace());

  float sub[3][10] = {0.0};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 10; ++j) {
      dense_x_data[i * 10 + j] = (i * 10 + j) * 1.0;
      sub[i][j] = (i * 10 + j) * 1.0 - j * 2.0;
    }
  }
  for (size_t i = 0; i < 10; ++i) {
    dense_y_data[i] = i * 2.0;
  }

  // 2. test API
  pten::CPUContext dev_ctx;
  dev_ctx.SetAllocator(paddle::memory::allocation::AllocatorFacade::Instance()
                           .GetAllocator(paddle::platform::CPUPlace())
                           .get());
  dev_ctx.Init();
  auto dense_out = pten::Subtract<float>(dev_ctx, dense_x, dense_y);

  // 3. check result
  ASSERT_EQ(dense_out.dims().size(), 2);
  ASSERT_EQ(dense_out.dims()[0], 3);
  ASSERT_EQ(dense_out.dtype(), pten::DataType::FLOAT32);
  ASSERT_EQ(dense_out.meta().layout, pten::DataLayout::NCHW);

  auto expect_result = sub;
  auto actual_result0 = dense_out.data<float>()[0];
  auto actual_result1 = dense_out.data<float>()[1];
  auto actual_result2 = dense_out.data<float>()[10];
  ASSERT_NEAR(expect_result[0][0], actual_result0, 1e-6f);
  ASSERT_NEAR(expect_result[0][1], actual_result1, 1e-6f);
  ASSERT_NEAR(expect_result[1][0], actual_result2, 1e-6f);
}

TEST(DEV_API, divide) {
  // 1. create tensor
  const auto alloc = std::make_unique<paddle::experimental::DefaultAllocator>(
      paddle::platform::CPUPlace());
  pten::DenseTensor dense_x(alloc.get(),
                            pten::DenseTensorMeta(pten::DataType::FLOAT32,
                                                  pten::make_ddim({3, 10}),
                                                  pten::DataLayout::NCHW));
  auto* dense_x_data =
      dense_x.mutable_data<float>(paddle::platform::CPUPlace());

  pten::DenseTensor dense_y(alloc.get(),
                            pten::DenseTensorMeta(pten::DataType::FLOAT32,
                                                  pten::make_ddim({10}),
                                                  pten::DataLayout::NCHW));
  auto* dense_y_data =
      dense_y.mutable_data<float>(paddle::platform::CPUPlace());

  float div[3][10] = {0.0};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 10; ++j) {
      dense_x_data[i * 10 + j] = (i * 10 + j) * 1.0;
      div[i][j] = (i * 10 + j) * 1.0 / (j * 2.0 + 1);
    }
  }
  for (size_t i = 0; i < 10; ++i) {
    dense_y_data[i] = i * 2.0 + 1;
  }

  // 2. test API
  pten::CPUContext dev_ctx;
  dev_ctx.SetAllocator(paddle::memory::allocation::AllocatorFacade::Instance()
                           .GetAllocator(paddle::platform::CPUPlace())
                           .get());
  dev_ctx.Init();
  auto dense_out = pten::Divide<float>(dev_ctx, dense_x, dense_y);

  // 3. check result
  ASSERT_EQ(dense_out.dims().size(), 2);
  ASSERT_EQ(dense_out.dims()[0], 3);
  ASSERT_EQ(dense_out.dtype(), pten::DataType::FLOAT32);
  ASSERT_EQ(dense_out.layout(), pten::DataLayout::NCHW);

  auto expect_result = div;
  auto actual_result0 = dense_out.data<float>()[0];
  auto actual_result1 = dense_out.data<float>()[1];
  auto actual_result2 = dense_out.data<float>()[10];
  ASSERT_NEAR(expect_result[0][0], actual_result0, 1e-6f);
  ASSERT_NEAR(expect_result[0][1], actual_result1, 1e-6f);
  ASSERT_NEAR(expect_result[1][0], actual_result2, 1e-6f);
}

TEST(DEV_API, multiply) {
  // 1. create tensor
  const auto alloc = std::make_unique<paddle::experimental::DefaultAllocator>(
      paddle::platform::CPUPlace());
  pten::DenseTensor dense_x(alloc.get(),
                            pten::DenseTensorMeta(pten::DataType::FLOAT32,
                                                  pten::make_ddim({3, 10}),
                                                  pten::DataLayout::NCHW));
  auto* dense_x_data =
      dense_x.mutable_data<float>(paddle::platform::CPUPlace());

  pten::DenseTensor dense_y(alloc.get(),
                            pten::DenseTensorMeta(pten::DataType::FLOAT32,
                                                  pten::make_ddim({10}),
                                                  pten::DataLayout::NCHW));
  auto* dense_y_data =
      dense_y.mutable_data<float>(paddle::platform::CPUPlace());

  float mul[3][10] = {0.0};
  for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 10; ++j) {
      dense_x_data[i * 10 + j] = (i * 10 + j) * 1.0;
      mul[i][j] = (i * 10 + j) * 1.0 * j * 2.0;
    }
  }
  for (size_t i = 0; i < 10; ++i) {
    dense_y_data[i] = i * 2.0;
  }

  // 2. test API
  pten::CPUContext dev_ctx;
  dev_ctx.SetAllocator(paddle::memory::allocation::AllocatorFacade::Instance()
                           .GetAllocator(paddle::platform::CPUPlace())
                           .get());
  dev_ctx.Init();
  auto dense_out = pten::Multiply<float>(dev_ctx, dense_x, dense_y);

  // 3. check result
  ASSERT_EQ(dense_out.dims().size(), 2);
  ASSERT_EQ(dense_out.dims()[0], 3);
  ASSERT_EQ(dense_out.dtype(), pten::DataType::FLOAT32);
  ASSERT_EQ(dense_out.layout(), pten::DataLayout::NCHW);

  auto expect_result = mul;
  auto actual_result0 = dense_out.data<float>()[0];
  auto actual_result1 = dense_out.data<float>()[1];
  auto actual_result2 = dense_out.data<float>()[10];
  ASSERT_NEAR(expect_result[0][0], actual_result0, 1e-6f);
  ASSERT_NEAR(expect_result[0][1], actual_result1, 1e-6f);
  ASSERT_NEAR(expect_result[1][0], actual_result2, 1e-6f);
}
}  // namespace tests
}  // namespace pten
