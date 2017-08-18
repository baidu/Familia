// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "familia/vose_alias.h"
#include "familia/util.h"

#include <queue>

namespace familia {

void VoseAlias::initialize(const std::vector<double>& distribution) {
    int size = distribution.size();
    _prob.resize(size);
    _alias.resize(size);
    std::vector<double> p(size, 0.0);
    double sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += distribution[i];
    }
    for (int i = 0; i < size; ++i) {
        p[i] = distribution[i] / sum * size; // scale up probability
    }
    
    std::queue<double> large;
    std::queue<double> small;
    for (int i = 0; i < size; ++i) {
        if (p[i] < 1.0)  {
            small.push(i);
        } else {
            large.push(i);
        }
    }
    while (!small.empty() && !large.empty()) {
        int l = small.front();
        int g = large.front();
        small.pop();
        large.pop();
        _prob[l] = p[l];
        _alias[l] = g;
        p[g] = p[g] + p[l] - 1; // a more numerically stable option
        if (p[g] < 1.0) {
            small.push(g);
        } else {
            large.push(g);
        }
    }
    while (!large.empty()) {
        int g = large.front();
        large.pop();
        _prob[g] = 1.0;
    }
    while (!small.empty()) {
        int l = small.front();
        small.pop();
        _prob[l] = 1.0;
    }
}

int VoseAlias::generate() const {
    int dart1 = rand_k(size());
    int dart2 = rand();

    return dart2 > _prob[dart1]? dart1 : _alias[dart1];
}
} // namespace familia
