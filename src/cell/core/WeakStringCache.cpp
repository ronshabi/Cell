// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#include "WeakStringCache.hpp"

#include "StringSlice.hpp"
#include "cell/log/Log.hpp"

namespace cell {
uint64_t WeakStringCache::AddKeyValuePair(const String& k, const String& v) noexcept {
  CELL_LOG_DEBUG("Adding KV Pair <'%s', '%s'>", k.GetCString(), v.GetCString());
  const auto key_pos = SearchKey(k.SubSlice());

  if (key_pos == kKeyDoesNotExist) {
    table_.emplace_back(k, v);
    return table_.size() - 1;
  }

  table_[key_pos].second.Clear();
  table_[key_pos].second.AppendStringSlice(v.SubSlice());
  return key_pos;
}

uint64_t WeakStringCache::AppendToValue(StringSlice k, StringSlice v) noexcept {
  const auto key_pos = SearchKey(k);

  if (key_pos == kKeyDoesNotExist) {
    table_.emplace_back(k, v);
    return table_.size() - 1;
  }

  table_[key_pos].second.AppendStringSlice(v);
  return key_pos;
}

uint64_t WeakStringCache::SearchKey(cell::StringSlice k) const noexcept {
  for (uint64_t i = 0; i < table_.size(); ++i) {
    if (table_[i].first.Compare(k)) {
      return i;
    }
  }

  return kKeyDoesNotExist;
}

uint64_t WeakStringCache::SearchKeyIgnoreCase(StringSlice k) const noexcept {
  for (uint64_t i = 0; i < table_.size(); ++i) {
    if (table_[i].first.CompareIgnoreCase(k)) {
      return i;
    }
  }

  return kKeyDoesNotExist;
}
StringSlice WeakStringCache::GetKeyAtIndex(uint64_t index) const noexcept {
  CELL_ASSERT(index < table_.size());

  return table_[index].second.SubSlice();
}

}  // namespace cell