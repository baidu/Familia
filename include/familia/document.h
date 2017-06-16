// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: chenzeyu01@baidu.com

#ifndef FAMILIA_DOCUMENT_H
#define FAMILIA_DOCUMENT_H

#include <algorithm>
#include <string>
#include <vector>

namespace familia {

// 主题的基本数据结构，包含id以及对应的概率
struct Topic {
    int tid; // topic id
    double prob; // topic probability
    bool operator<(const Topic& t) const {
        return prob > t.prob; // 优先按照主题概率从大到小排序
    }
};

// LDA文档存储基本单元，包含词id以及对应的主题id
struct Token {
    int topic;
    int id;
};

// SentenceLDA文档存储基本单元，包含句子的词id以及对应的主题id
struct Sentence {
    int topic;
    std::vector<int> tokens;
};

// LDA模型inference结果存储结构
class LDADoc {
public:
    LDADoc() = default;

    explicit LDADoc(int num_topics) {
        init(num_topics);
    }

    // 根据主题数初始化文档结构
    void init(int num_topics);

    // 添加新的单词
    void add_token(const Token& token);

    inline Token& token(size_t index) {
        return _tokens[index];
    }

    // 对文档中第index个单词的主题置为new_topic, 并更新相应的文档主题分布
    void set_topic(int index, int new_topic);

    // 配置文档先验参数alpha
    inline void set_alpha(float alpha) {
        _alpha = alpha;
    }

    // 返回文档中词的数量
    inline size_t size() const {
        return _tokens.size();
    }

    inline size_t topic_sum(int topic_id) const {
        return _topic_sum[topic_id];
    }

    // 返回稀疏格式的文档主题分布, 默认按照主题概率从大到小的排序
    // NOTE: 这一接口返回结果为了稀疏化，忽略了先验参数的作用
    void sparse_topic_dist(std::vector<Topic>& topic_dist, bool sort = true) const;

    // 返回稠密格式的文档主题分布, 考虑了先验参数的结果
    void dense_topic_dist(std::vector<float>& dense_dist) const;

    // 对每轮采样结果进行累积, 以得到一个更逼近真实后验的分布
    void accumulate_topic_sum();

protected:
    // 主题数
    int _num_topics;
    // 累积的采样轮数
    int _num_accum;
    // 文档先验参数alpha
    float _alpha;
    // inference 结果存储结构
    std::vector<Token> _tokens;
    // 文档在一轮采样中的topic sum
    std::vector<int> _topic_sum;
    // topic sum在多轮采样中的累积结果 
    std::vector<int> _accum_topic_sum;
};

// Sentence LDA Document
// 继承自LDADoc，新增了add_sentence接口
class SLDADoc : public LDADoc {
public:
    SLDADoc() = default;

    void init(int num_topics);

    // 新增句子
    void add_sentence(const Sentence& sent);

    // 对文档中第index个句子的主题置为new_topic, 并更新相应的文档主题分布
    void set_topic(int index, int new_topic);

    // 返回文档句子数量
    inline size_t size() const {
        return _sentences.size();
    }
    
    inline Sentence& sent(size_t index) {
        return _sentences[index];
    }

private:
    // 文档为句子的集合，每个句子有一个对应主题
    std::vector<Sentence> _sentences;
};
} // namespace familia
#endif  // FAMILIA_DOCUMENT_H
