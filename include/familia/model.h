// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: chenzeyu01@baidu.com

#ifndef FAMILIA_MODEL_H
#define FAMILIA_MODEL_H

#include <vector>
#include <unordered_map>
#include <stdio.h>
#include <limits>

#include "familia/config.pb.h"
#include "familia/util.h"
#include "familia/vocab.h"

namespace familia {

// 主题计数，key为topic id，value是计数值
typedef std::pair<int, int> TopicCount;
// 多个主题计数构成主题分布 TopicDist = Topic Distribution
typedef std::vector<TopicCount> TopicDist;

// 主题模型模型存储结构，包含词表和word topic count两分布
// 其中LDA和SentenceLDA使用同样的模型存储格式
class TopicModel {
public:
    TopicModel() = delete;

    TopicModel(const std::string& work_dir, const ModelConfig& config);

    inline int term_id(const std::string& term) const {
        return _vocab.get_id(term);
    }

    // 加载word topic count以及词表文件
    void load_model(const std::string& word_topic_path, const std::string& vocab_path);
    
    // 返回模型中某个词在某个主题下的参数值，由于模型采用稀疏存储，若找不到则返回0
    int word_topic(int word_id, int topic_id) const {
        // 二分查找
        auto it = std::lower_bound(_word_topic[word_id].begin(), 
                                   _word_topic[word_id].end(), 
                                   std::make_pair(topic_id, std::numeric_limits<int>::min()));
        if (it != _word_topic[word_id].end() && it->first == topic_id) {
            return it->second;
        } else {
            return 0;
        }
    }

    // 返回某个词的主题分布
    TopicDist& word_topic(int term_id) {
        return _word_topic.at(term_id);
    }

    // 返回指定topic id的topic sum参数
    uint64_t topic_sum(int topic_id) const;

    // 返回topic sum参数向量
    std::vector<uint64_t>& topic_sum() {
        return _topic_sum;
    }

    inline int num_topics() const {
        return _num_topics;
    }

    inline size_t vocab_size() const {
        return _vocab.size();
    }

    inline float alpha() const {
        return _alpha;
    }

    inline float alpha_sum() const {
        return _alpha_sum;
    }

    inline float beta() const {
        return _beta;
    }

    inline float beta_sum() const {
        return _beta_sum;
    }

    inline ModelType type() const {
        return _type;
    }

private:
    // 加载word topic参数
    void load_word_topic(const std::string& word_topic_path);
    // word topic 模型参数
    std::vector<TopicDist> _word_topic;
    // word topic对应的每一维主题的计数总和
    std::vector<uint64_t> _topic_sum;
    // 模型对应的词表数据结构
    Vocab _vocab;
    // 主题数
    int _num_topics;
    // 主题模型超参数
    float _alpha;
    float _alpha_sum;
    float _beta;
    float _beta_sum;
    // 模型类型
    ModelType _type;
};
} // namespace familia
#endif  // FAMILIA_MODEL_H
