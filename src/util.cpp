// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "familia/util.h"

namespace familia {

void split(std::vector<std::string>& result, const std::string& text, char separator) {
    size_t start = 0;
    size_t end = 0;
    if (text.empty()) {
        return;
    }
    while ((end = text.find(separator, start)) != std::string::npos) {
        std::string substr = text.substr(start, end - start);
        result.push_back(std::move(substr));
        start = end + 1;
    }
    // NOTE: 如果输入没有分割字符，则返回原输入
    result.push_back(text.substr(start));
}
} // namespace familia
