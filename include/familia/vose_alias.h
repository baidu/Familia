// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: chenzeyu01@baidu.com

#ifndef FAMILIA_VOSE_ALIAS_H
#define FAMILIA_VOSE_ALIAS_H

#include <vector>

#include "familia/util.h"

namespace familia {
// Vose's Alias Method 数值稳定版本实现
// 更多的具体细节可以参考 http://www.keithschwarz.com/darts-dice-coins/
class VoseAlias {
public:
    VoseAlias() = default;

    // 根据输入分布初始化alias table
    void initialize(const std::vector<double>& distribution);

    // 从给定分布中生成采样样本
    int generate() const;

    // 离散分布的维度
    inline size_t size() const {
        return _prob.size();
    }

    // no copying alowed
    VoseAlias(const VoseAlias&) = delete;
    VoseAlias& operator=(const VoseAlias&) = delete;

private:
    // alias table
    std::vector<int> _alias;
    // probability table
    std::vector<double> _prob;
};
} // namespace familia
#endif // FAMILIA_VOSE_ALIAS_H
