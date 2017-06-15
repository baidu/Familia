// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: chenzeyu01@baidu.com

#include "familia/document.h"
#include "familia/util.h"

using std::vector;
using std::string;

namespace familia {

// -------------LDA Begin---------------
void LDADoc::init(int num_topics) {
    _num_topics = num_topics;
    _tokens.clear();
    _topic_sum.resize(_num_topics, 0);
    _accum_topic_sum.resize(_num_topics, 0);
}

void LDADoc::add_token(const Token& token) {
    CHECK_GE(token.topic, 0) << "Topic " << token.topic << " out of range!";
    CHECK_LT(token.topic, _num_topics) << "Topic " << token.topic << " out of range!";
    _tokens.push_back(token);
    _topic_sum[token.topic]++;
}

void LDADoc::set_topic(int index, int new_topic) {
    CHECK_GE(new_topic, 0) << "Topic " << new_topic << " out of range!";
    CHECK_LT(new_topic, _num_topics) << "Topic " << new_topic << " out of range!";
    int old_topic = _tokens[index].topic;
    if (new_topic == old_topic) {
        return;
    }
    _tokens[index].topic = new_topic;
    _topic_sum[old_topic]--;
    _topic_sum[new_topic]++;
}

void LDADoc::topic_dist(vector<Topic>& topic_dist, bool sort) const {
    topic_dist.clear();
    size_t sum = 0;
    for (int i = 0; i < _num_topics; ++i) {
        sum += _accum_topic_sum[i];
    }
    if (sum == 0) { 
        return; // 返回空结果
    }
    for (int i = 0; i < _num_topics; ++i) {
        // 跳过0的的项，得到稀疏主题分布
        if (_accum_topic_sum[i] == 0) {
            continue;
        }
        topic_dist.push_back({i, _accum_topic_sum[i] * 1.0 / sum});
    }
    if (sort) {
        std::sort(topic_dist.begin(), topic_dist.end());
    }
}

void LDADoc::dense_topic_dist(vector<float>& dense_dist) const {
    dense_dist.clear();
    dense_dist.resize(_num_topics, 0.0);
    size_t sum = 0;
    for (int i = 0; i < _num_topics; ++i) {
        sum += _accum_topic_sum[i];
    }
    if (sum == 0) {
        return; // 返回0向量
    }
    for (int i = 0; i < _num_topics; ++i) {
        dense_dist[i] = _accum_topic_sum[i] * 1.0 / sum;
    }
}

void LDADoc::accumulate_topic_sum() {
    for (int i = 0; i < _num_topics; ++i) {
        _accum_topic_sum[i] += _topic_sum[i];
    }
}
// -------------LDA End---------------

// --------Sentence-LDA Begin---------
void SLDADoc::init(int num_topics) {
    _num_topics = num_topics;
    _sentences.clear();
    _topic_sum.resize(_num_topics, 0);
    _accum_topic_sum.resize(_num_topics, 0);
}

void SLDADoc::add_sentence(const Sentence& sent) {
    CHECK_GE(sent.topic, 0) << "Topic " << sent.topic << " out of range!";
    CHECK_LT(sent.topic, _num_topics) << "Topic " << sent.topic << " out of range!";
    _sentences.push_back(sent);
    _topic_sum[sent.topic]++;
}

void SLDADoc::set_topic(int index, int new_topic) {
    CHECK_GE(new_topic, 0) << "Topic " << new_topic << " out of range!";
    CHECK_LT(new_topic, _num_topics) << "Topic " << new_topic << " out of range!";
    int old_topic = _sentences[index].topic;
    if (new_topic == old_topic) {
        return;
    }
    _sentences[index].topic = new_topic;
    _topic_sum[old_topic]--;
    _topic_sum[new_topic]++;
}
// --------Sentence-LDA End---------
} // namespace familia
