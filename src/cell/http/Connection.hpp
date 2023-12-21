// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_CONNECTION_HPP
#define CELL_CONNECTION_HPP

namespace cell::http {

enum class Connection { Close, KeepAlive };

}

#endif  // CELL_CONNECTION_HPP
