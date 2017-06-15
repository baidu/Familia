// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: chenzeyu01@baidu.com

#ifndef FAMILIA_LDA_SAMPLER_H
#define FAMILIA_LDA_SAMPLER_H

#include "familia/document.h"
#include "familia/vose_alias.h"
#include "familia/model.h"
#include "familia/util.h"

#include <memory>

namespace familia {

typedef std::vector<int32_t> TopicIndex;

// 采样器的接口
class Sampler {
public:
    virtual ~Sampler() = default;

    // 对文档进行LDA主题采样
    virtual void sample_doc(LDADoc& doc) = 0;

    // 对文档进行SentenceLDA主题采样
    virtual void sample_doc(SLDADoc& doc) = 0;
};

// 基于Metropolis-Hastings的采样器实现，包含LDA和SentenceLDA两个模型的实现
class MHSampler : public Sampler {
public:
    // 默认MH-Steps为2
    MHSampler(std::shared_ptr<TopicModel> model) : _model(model) {
        construct_alias_table();
    }

    void sample_doc(LDADoc& doc) override;

    void sample_doc(SLDADoc& doc) override;

    // no copying allowed
    MHSampler(const MHSampler&) = delete;
    MHSampler& operator=(const MHSampler&) = delete;

private:
    // 根据LDA模型参数构建alias table
    int construct_alias_table();

    // 对文档中的一个词进行主题采样, 返回采样结果对应的主题ID
    int sample_token(LDADoc& doc, Token& token);

    // 对文档中的一个句子进行主题采样, 返回采样结果对应的主题ID
    int sample_sentence(SLDADoc& doc, Sentence& sent);

    // doc proposal for LDA
    int doc_proposal(LDADoc& doc, Token& token);

    // doc proposal for Sentence-LDA
    int doc_proposal(SLDADoc& doc, Sentence& sent);

    // word proposal for LDA
    int word_proposal(LDADoc& doc, Token& token, int old_topic);

    // word proposal for Sentence-LDA
    int word_proposal(SLDADoc& doc, Sentence& sent, int old_topic);

    // propotional function for LDA model
    float proportional_funtion(LDADoc& doc, Token& token, int new_topic);

    // propotional function for SLDA model
    float proportional_funtion(SLDADoc& doc, Sentence& sent, int new_topic);

    // word proposal distribuiton for LDA and Sentence-LDA
    float word_proposal_distribution(int word_id, int topic);

    // doc proposal distribution for LDA and Sentence-LDA
    float doc_proposal_distribution(LDADoc& doc, int topic);

    // 对当前词id的单词使用Metroplis-Hastings方法proprose一个主题id
    int propose(int word_id);

    // LDA model pointer, shared by sampler and inference engine
    std::shared_ptr<TopicModel> _model;

    // 主题的下标映射
    std::vector<TopicIndex> _topic_indexes;

    // 存放每个单词使用VoseAlias Method构建的alias结果(word-proposal无先验参数部分)
    std::vector<VoseAlias> _alias_tables;

    // 存放每个单词各个主题下概率之和(word-proposal无先验参数部分)
    std::vector<double> _prob_sum;

    // 存放先验参数部分使用VoseAlias Method构建的alias结果(word-proposal先验参数部分)
    VoseAlias _beta_alias;
    
    // 存放先验参数各个主题下概率之和(word-proposal先验参数部分)
    double _beta_prior_sum;

    // metroplis-hastings steps, 默认值为2
    static constexpr int _mh_steps = 2;
};

// 吉布斯采样器，实现了LDA和SentenceLDA两种模型的采样算法
class GibbsSampler : public Sampler {
public:
    GibbsSampler(std::shared_ptr<TopicModel> model) : _model(model) {
    }

    // 对文档输入进行LDA主题采样，主题结果保存在doc中
    void sample_doc(LDADoc& doc) override;

    // 使用Sentence-LDA模型对文档每个句子进行采样, 结果保存在doc中
    // 其中SentenceLDA采样算法考虑了数值计算的精度问题，对公式进行了采样
    void sample_doc(SLDADoc& doc) override;

    // no copying allowed
    GibbsSampler(const GibbsSampler&) = delete;
    GibbsSampler& operator=(const GibbsSampler&) = delete;
private:
    int sample_token(LDADoc& doc, Token& token);

    int sample_sentence(SLDADoc& doc, Sentence& sent);

    std::shared_ptr<TopicModel> _model;
};
} // namespace familia
#endif  // FAMILIA_SAMPLER_H
