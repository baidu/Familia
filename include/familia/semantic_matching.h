// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: jiangjiajun@baidu.com

#ifndef FAMILIA_SEMANTIC_MATCHING_H
#define FAMILIA_SEMANTIC_MATCHING_H

#include "familia/model.h"
#include "familia/document.h"

#include <memory>
#include <cmath>
#include <vector>
#include <unordered_map>

namespace familia {

constexpr double EPS = 1e-06; // epsilon

typedef std::vector<float> Embedding;
typedef std::vector<float> Distribution;

// 存储候选词以及对应距离
struct WordAndDis {
    std::string word;
    float distance;
};

// Topical Word Embedding (TWE) 模型类
// 包括了模型的加载及embedding的获取
class TopicalWordEmbedding {
public:
    TopicalWordEmbedding(const std::string& work_dir, 
                         const std::string& emb_file) {
        const std::string emb_path = work_dir + "/"  + emb_file; 
        CHECK_EQ(load_emb(emb_path), 0) << "Failed to load Topical Word Embedding!";
    }

    ~TopicalWordEmbedding() = default;

    // 加载Topical Word Embedding
    int load_emb(const std::string& emb_file);

    // 根据topic id返回topic的embedding
    Embedding& topic_emb(int topic_id);

    // 根据明文返回词的embedding
    Embedding& word_emb(const std::string& term);

    // 返回距离词最近的K个词
    void nearest_words(const std::string& word,
                       std::vector<WordAndDis>& candidates);

    // 返回离主题最近的K个词
    void nearest_words_around_topic(int topic_index,
                                    std::vector<WordAndDis>& candidates);

    // 检查当前词是否在TWE模型中
    bool contains_word(const std::string& term) const;

    // 返回主题数
    int num_topics() const;

private:
    // word embedding
    std::unordered_map<std::string, Embedding> _word_emb;
    // topic embedding
    std::vector<Embedding> _topic_emb;
    // num of topics
    int _num_topics;
    // TWE模型embeeding size
    int _emb_size;
    // TWE中word embedding的词表大小
    int _vocab_size;
};

// 语义匹配计算指标类
class SemanticMatching {
public:
    // 计算向量的长度，传入的是embedding
    // NOTE: 可用SSE进行向量运算加速，此处为了代码可读性不进行优化
    static float l2_norm(const Embedding& vec) {
        float result = 0.0;
        for (size_t i = 0; i < vec.size(); ++i) {
            result += vec[i] * vec[i];
        }

        return sqrt(result);
    }

    // 计算两个embedding的余弦相似度
    static float cosine_similarity(const Embedding& vec1, const Embedding& vec2) {
        float result = 0.0;
        float norm1 = l2_norm(vec1);
        float norm2 = l2_norm(vec2);

        // NOTE: 可用SSE进行向量运算加速，此处为了代码可读性不进行优化
        for (size_t i = 0; i < vec1.size(); ++i) {
            result += vec1[i] * vec2[i];
        }
        result = result / norm1 / norm2;
        return result;
    }

    // 使用短文本到长文本之间的似然值表示之间的相似度
    static float likelihood_based_similarity(const std::vector<std::string>& terms, 
                                             const std::vector<Topic>& doc_topic_dist,
                                             std::shared_ptr<TopicModel> model) {
        int num_of_term_in_vocab = 0;
        float result = 0.0;
               
        for (size_t i = 0; i < terms.size(); ++i) {
            int term_id = model->term_id(terms[i]);
            if (term_id == OOV) {
                continue;
            }
            
            // 统计在词表中的单词
            num_of_term_in_vocab += 1;
            for (size_t j = 0; j < doc_topic_dist.size(); ++j) {
                int topic_id = doc_topic_dist[j].tid;
                float prob = doc_topic_dist[j].prob; 
                result += model->word_topic(term_id, topic_id) * 1.0 / 
                          model->topic_sum(topic_id) * prob;
            }
        }

        if (num_of_term_in_vocab == 0) {
            return result;
        }

        return result / num_of_term_in_vocab;
    }

    
    // 基于Topical Word Embedding (TWE) 计算短文本与长文本的相似度
    // 输入短文本明文分词结果，长文本主题分布，TWE模型，返回长文本与短文本语义相似度
    static float twe_based_similarity(const std::vector<std::string>& terms,
                                      const std::vector<Topic>& doc_topic_dist,
                                      TopicalWordEmbedding& twe) {
        int short_text_length = terms.size();
        float result = 0.0;

        for (size_t i = 0; i < terms.size(); ++i) {
            if (!twe.contains_word(terms[i])) {
                short_text_length--;
                continue;
            }
            Embedding& word_emb = twe.word_emb(terms[i]);
            for (const auto& topic : doc_topic_dist) {
                Embedding& topic_emb = twe.topic_emb(topic.tid);
                result += cosine_similarity(word_emb, topic_emb) * topic.prob;
            }
        }

        if (short_text_length == 0) { // 如果短文本中的词均不在词表中
            return 0.0;
        }

        return result / short_text_length; // 针对短文本长度进行归一化
    }

    // Kullback Leibler Divergence
    // D(P||Q) = \sum_i {P(i) ln \frac {P(i)}{Q(i)}
    // REQUIRE: 传入的两个参数维度须一致
    static float kullback_leibler_divergence(Distribution& dist1, Distribution& dist2) {
        CHECK_EQ(dist1.size(), dist2.size());
        float result = 0.0;
        for (size_t i = 0; i < dist1.size(); ++i) {
            dist2[i] = dist2[i] < EPS ? EPS : dist2[i];
            result += dist1[i] * log(dist1[i] / dist2[i]);
        }

        return result;
    }

    // Jensen-Shannon Divergence
    // REQUIRE: 传入的两个参数维度须一致
    static float jensen_shannon_divergence(Distribution& dist1, Distribution& dist2) {
        CHECK_EQ(dist1.size(), dist2.size());
        // 检测分布值小于epsilon的情况
        for (size_t i = 0; i < dist1.size(); ++i) {
            dist1[i] = dist1[i] < EPS ? EPS : dist1[i];
            dist2[i] = dist2[i] < EPS ? EPS : dist2[i];
        }

        Distribution mean(dist1.size(), 0);

        for (size_t i = 0; i < dist1.size(); ++i) {
            mean[i] = (dist1[i] + dist2[i]) * 0.5;
        }

        float jsd = kullback_leibler_divergence(dist1, mean) * 0.5 + 
                    kullback_leibler_divergence(dist2, mean) * 0.5;
        return jsd;
    }

    // Hellinger Distance
    // REQUIRE: 传入的两个参数维度须一致
    static float hellinger_distance(Distribution& dist1, Distribution& dist2) {
        CHECK_EQ(dist1.size(), dist2.size());
        
        // NOTE: 可用SSE进行向量运算加速，此处为了代码可读性不进行优化
        float result = 0.0;
        for (size_t i = 0; i < dist1.size(); ++i) {
            float tmp = sqrt(dist1[i]) - sqrt(dist2[i]);
            result += tmp * tmp;
        }

        // 1/√2 = 0.7071067812
        result = sqrt(result) * 0.7071067812;
        return result;
    }
};
} // namespace familia
#endif  // FAMILIA_SEMANTIC_MATCHING_H
