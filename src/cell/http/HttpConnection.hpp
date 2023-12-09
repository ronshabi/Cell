// SPDX-FileCopyrightText: (c) 2023 Ron Shabi <ron@ronsh.net>
// SPDX-License-Identifier: Apache-2.0

#ifndef CELL_HTTPCONNECTION_HPP
#define CELL_HTTPCONNECTION_HPP

namespace cell::http {

enum class HttpConnection { Close, KeepAlive };

}

#endif  // CELL_HTTPCONNECTION_HPP
