// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "arrow/array/builder_binary.h"
#include "arrow/array/data.h"
#include "arrow/buffer.h"
#include "arrow/buffer_builder.h"
#include "arrow/compute/kernel.h"
#include "arrow/datum.h"
#include "arrow/result.h"
#include "arrow/scalar.h"
#include "arrow/status.h"
#include "arrow/type.h"
#include "arrow/type_traits.h"
#include "arrow/util/bit_block_counter.h"
#include "arrow/util/bit_util.h"
#include "arrow/util/bitmap_generate.h"
#include "arrow/util/bitmap_reader.h"
#include "arrow/util/bitmap_writer.h"
#include "arrow/util/checked_cast.h"
#include "arrow/util/decimal.h"
#include "arrow/util/logging.h"
#include "arrow/util/macros.h"
#include "arrow/util/visibility.h"

namespace arrow {
namespace compute {
namespace internal {

// ----------------------------------------------------------------------
// Input and output value type definitions

template <typename Type, typename Enable = void>
struct GetViewType;

template <typename Type>
struct GetViewType<Type, enable_if_has_c_type<Type>> {
  using T = typename Type::c_type;
  using PhysicalType = T;

  static T LogicalValue(PhysicalType value) { return value; }
};

template <typename Type>
struct GetViewType<Type, enable_if_t<is_base_binary_type<Type>::value ||
                                     is_fixed_size_binary_type<Type>::value ||
                                     is_binary_view_like_type<Type>::value>> {
  using T = std::string_view;
  using PhysicalType = T;

  static T LogicalValue(PhysicalType value) { return value; }
};

template <>
struct GetViewType<Decimal128Type> {
  using T = Decimal128;
  using PhysicalType = std::string_view;

  static T LogicalValue(PhysicalType value) {
    return Decimal128(reinterpret_cast<const uint8_t*>(value.data()));
  }

  static T LogicalValue(T value) { return value; }
};

template <>
struct GetViewType<Decimal256Type> {
  using T = Decimal256;
  using PhysicalType = std::string_view;

  static T LogicalValue(PhysicalType value) {
    return Decimal256(reinterpret_cast<const uint8_t*>(value.data()));
  }

  static T LogicalValue(T value) { return value; }
};

std::vector<const Array*> GetArrayPointers(const ArrayVector& arrays) {
  std::vector<const Array*> pointers(arrays.size());
  std::transform(arrays.begin(), arrays.end(), pointers.begin(),
                 [&](const std::shared_ptr<Array>& array) { return array.get(); });
  return pointers;
}

// END of DispatchBest helpers
// ----------------------------------------------------------------------
}  // namespace internal
}  // namespace compute
}  // namespace arrow
