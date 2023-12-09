// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "WeakStringMap.hpp"

#include "StringSlice.hpp"
#include "cell/log/Log.hpp"

namespace cell {
uint64_t WeakStringMap::AddKeyValuePair(StringSlice k, StringSlice v) noexcept {
  CELL_LOG_DEBUG("Adding KV Pair <'%s', '%s'>", k.GetConstCharPtr(), v.GetConstCharPtr());
  const auto key_pos = SearchKey(k);

  if (key_pos == kKeyDoesNotExist) {
    table_.emplace_back(k, v);
    return table_.size() - 1;
  }

  table_[key_pos].second.Clear();
  table_[key_pos].second.AppendStringSlice(v);
  return key_pos;
}

uint64_t WeakStringMap::AppendToValue(StringSlice k, StringSlice v) noexcept {
  const auto key_pos = SearchKey(k);

  if (key_pos == kKeyDoesNotExist) {
    table_.emplace_back(k, v);
    return table_.size() - 1;
  }

  table_[key_pos].second.AppendStringSlice(v);
  return key_pos;
}

uint64_t WeakStringMap::SearchKey(cell::StringSlice k) const noexcept {
  for (uint64_t i = 0; i < table_.size(); ++i) {
    if (table_[i].first.Compare(k)) {
      return i;
    }
  }

  return kKeyDoesNotExist;
}

uint64_t WeakStringMap::SearchKeyIgnoreCase(StringSlice k) const noexcept {
  for (uint64_t i = 0; i < table_.size(); ++i) {
    if (table_[i].first.CompareIgnoreCase(k)) {
      return i;
    }
  }

  return kKeyDoesNotExist;
}
StringSlice WeakStringMap::GetKeyAtIndex(uint64_t index) const noexcept {
  CELL_ASSERT(index < table_.size());

  return table_[index].second.SubSlice();
}

}  // namespace cell