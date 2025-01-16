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

#include <memory>
#include <utility>

#include "arrow/compute/function_options.h"
#include "arrow/compute/ordering.h"
#include "arrow/result.h"
#include "arrow/type_fwd.h"

namespace arrow {
namespace compute {

class ExecContext;

/// \brief Percentile rank options
class ARROW_EXPORT RankPercentileOptions : public FunctionOptions {
 public:
  explicit RankPercentileOptions(std::vector<SortKey> sort_keys = {},
                                 NullPlacement null_placement = NullPlacement::AtEnd,
                                 double factor = 1.0);
  /// Convenience constructor for array inputs
  explicit RankPercentileOptions(SortOrder order,
                                 NullPlacement null_placement = NullPlacement::AtEnd,
                                 double factor = 1.0)
      : RankPercentileOptions({SortKey("", order)}, null_placement, factor) {}

  static constexpr char const kTypeName[] = "RankPercentileOptions";
  static RankPercentileOptions Defaults() { return RankPercentileOptions(); }

  /// Column key(s) to order by and how to order by these sort keys.
  std::vector<SortKey> sort_keys;
  /// Whether nulls and NaNs are placed at the start or at the end
  NullPlacement null_placement;
  /// Factor to apply to the output.
  /// Use 1.0 for results in (0, 1), 100.0 for percentages, etc.
  double factor;
};

}  // namespace compute
}  // namespace arrow
