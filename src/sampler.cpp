// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "familia/sampler.h"

namespace familia {

void MHSampler::sample_doc(LDADoc& doc) {
    for (size_t i = 0; i < doc.size(); ++i) {
        int new_topic = sample_token(doc, doc.token(i));
        doc.set_topic(i, new_topic);
    }
};

void MHSampler::sample_doc(SLDADoc& doc) {
    int new_topic = 0;
    for (size_t i = 0; i < doc.size(); ++i) {
        new_topic = sample_sentence(doc, doc.sent(i));
        doc.set_topic(i, new_topic);
    }
}

int MHSampler::propose(int word_id) {
    // 决定是否要从先验参数的alias table生成一个样本
    double dart = rand() * (_prob_sum[word_id] + _beta_prior_sum);
    int topic = -1;
    if (dart < _prob_sum[word_id]) {
        int idx = _alias_tables[word_id].generate(); // 从alias table中生成一个样本
        topic = _topic_indexes[word_id][idx]; // 找到当前idx对应的真实主题id
    } else { // 命中先验概率部分
        // 先验alias table为稠密分布，无需再做ID映射
        topic = _beta_alias.generate();
    }

    return topic;
}

int MHSampler::sample_token(LDADoc& doc, Token& token) {
    int new_topic = token.topic;
    for (int i = 0; i < _mh_steps; ++i) {
        int doc_proposed_topic = doc_proposal(doc, token);
        new_topic = word_proposal(doc, token, doc_proposed_topic);
    }

    return new_topic;
}

int MHSampler::sample_sentence(SLDADoc& doc, Sentence& sent) {
    int new_topic = sent.topic;
    for (int i = 0; i < _mh_steps; ++i) { 
        int doc_proposed_topic = doc_proposal(doc, sent);
        new_topic = word_proposal(doc, sent, doc_proposed_topic);
    }

    return new_topic;
}

int MHSampler::doc_proposal(LDADoc& doc, Token& token) {
    int old_topic = token.topic;
    int new_topic = old_topic;

    double dart = rand() * (doc.size() + _model->alpha_sum());
    if (dart < doc.size()) {
        int token_index = static_cast<int>(dart);
        new_topic = doc.token(token_index).topic;
    } else {
        // 命中文档先验部分, 则随机进行主题采样
        new_topic = rand_k(_model->num_topics());
    }

    if (new_topic != old_topic) {
        float proposal_old = doc_proposal_distribution(doc, old_topic);
        float proposal_new = doc_proposal_distribution(doc, new_topic);
        float proportion_old = proportional_funtion(doc, token, old_topic);
        float proportion_new = proportional_funtion(doc, token, new_topic);
        double transition_prob = (proportion_new * proposal_old) / (proportion_old * proposal_new);
        double rejection = rand();
        int mask = -(rejection < transition_prob); 
        return (new_topic & mask) | (old_topic & ~mask); // 用位运算避免if分支判断
    }

    return new_topic;
}

int MHSampler::doc_proposal(SLDADoc& doc, Sentence& sent) {
    int old_topic = sent.topic;
    int new_topic = -1;

    double dart = rand() * (doc.size() + _model->alpha_sum());
    if (dart < doc.size()) {
        int token_index = static_cast<int>(dart);
        new_topic = doc.sent(token_index).topic;
    } else {
        // 命中文档先验部分, 则随机进行主题采样
        new_topic = rand_k(_model->num_topics());
    }

    if (new_topic != old_topic) {
        float proportion_old = proportional_funtion(doc, sent, old_topic);
        float proportion_new = proportional_funtion(doc, sent, new_topic);
        float proposal_old = doc_proposal_distribution(doc, old_topic);
        float proposal_new = doc_proposal_distribution(doc, new_topic);
        double transition_prob = (proportion_new * proposal_old) / (proportion_old * proposal_new);
        double rejection = rand();
        int mask = -(rejection < transition_prob);
        return (new_topic & mask) | (old_topic & ~mask);
    }

    return new_topic;
}

int MHSampler::word_proposal(LDADoc& doc, Token& token, int old_topic) {
    int new_topic = propose(token.id); // prpose a new topic from alias table
    if (new_topic != old_topic) {
        float proposal_old = word_proposal_distribution(token.id, old_topic);
        float proposal_new = word_proposal_distribution(token.id, new_topic);
        float proportion_old = proportional_funtion(doc, token, old_topic);
        float proportion_new = proportional_funtion(doc, token, new_topic);
        double transition_prob = (proportion_new * proposal_old) / (proportion_old * proposal_new);
        double rejection = rand();
        int mask = -(rejection < transition_prob);
        return (new_topic & mask) | (old_topic & ~mask);
    }

    return new_topic;
}

// word proposal for Sentence-LDA
int MHSampler::word_proposal(SLDADoc& doc, Sentence& sent, int old_topic) {
    int new_topic = old_topic;
    for (const auto& word_id : sent.tokens) {
        new_topic = propose(word_id); // prpose a new topic from alias table
        if (new_topic != old_topic) {
            float proportion_old = proportional_funtion(doc, sent, old_topic);
            float proportion_new = proportional_funtion(doc, sent, new_topic);
            float proposal_old = word_proposal_distribution(word_id, old_topic);
            float proposal_new = word_proposal_distribution(word_id, new_topic);
            double transition_prob = (proportion_new * proposal_old) / 
                                     (proportion_old * proposal_new);

            double rejection = rand();
            int mask = -(rejection < transition_prob);
            new_topic = (new_topic & mask) | (old_topic & ~mask);
        }
    }

    return new_topic;
}

float MHSampler::proportional_funtion(LDADoc& doc, Token& token, int new_topic) {
    int old_topic = token.topic;
    float dt_alpha = doc.topic_sum(new_topic) + _model->alpha();
    float wt_beta = _model->word_topic(token.id, new_topic) + _model->beta();
    float t_sum_beta_sum = _model->topic_sum(new_topic) + _model->beta_sum();
    if (new_topic == old_topic && wt_beta > 1) {
        if (dt_alpha > 1) {
            dt_alpha -= 1;
        }
        wt_beta -= 1;
        t_sum_beta_sum -= 1;
    }

    return dt_alpha * wt_beta / t_sum_beta_sum;
}

float MHSampler::proportional_funtion(SLDADoc& doc, Sentence& sent, int new_topic) {
    int old_topic = sent.topic;
    float result = doc.topic_sum(new_topic) + _model->alpha();
    if (new_topic == old_topic) {
        result -= 1;
    }
    for (const auto& word_id : sent.tokens) {
        float wt_beta = _model->word_topic(word_id, new_topic) + _model->beta();
        float t_sum_beta_sum = _model->topic_sum(new_topic) + _model->beta_sum();
        if (new_topic == old_topic && wt_beta > 1) {
            wt_beta -= 1;
            t_sum_beta_sum -= 1;
        }

        result *= wt_beta / t_sum_beta_sum;
    }

    return result;
}

float MHSampler::doc_proposal_distribution(LDADoc& doc, int topic) {
    return doc.topic_sum(topic) + _model->alpha();
}

float MHSampler::word_proposal_distribution(int word_id, int topic) {
    float wt_beta = _model->word_topic(word_id, topic) + _model->beta();
    float t_sum_beta_sum = _model->topic_sum(topic) + _model->beta_sum();
    
    return wt_beta / t_sum_beta_sum;
}

int MHSampler::construct_alias_table() {
    size_t vocab_size = _model->vocab_size();
    _topic_indexes = std::vector<TopicIndex>(vocab_size);
    _alias_tables = std::vector<VoseAlias>(vocab_size);
    _prob_sum = std::vector<double>(vocab_size);

    // 构建每个词的alias table (不包含先验部分)
    std::vector<double> dist;
    for (size_t i = 0; i < vocab_size; ++i) {
        dist.clear();
        double prob_sum = 0;
        for (auto& iter : _model->word_topic(i)) {
            int topic_id = iter.first; // topic index
            int word_topic_count = iter.second; // topic count
            size_t topic_sum = _model->topic_sum(topic_id); // topic sum
            
            _topic_indexes[i].push_back(topic_id);
            double q = word_topic_count / (topic_sum + _model->beta_sum());
            dist.push_back(q);
            prob_sum += q;
        }
        _prob_sum[i] = prob_sum;
        if (dist.size() > 0) {
            _alias_tables[i].initialize(dist);
        }
    }

    // 构建先验参数beta的alias table
    _beta_prior_sum = 0;
    std::vector<double> beta_dist(_model->num_topics(), 0);
    for (int i = 0; i < _model->num_topics(); ++i) {
        beta_dist[i] = _model->beta() / (_model->topic_sum(i) + _model->beta_sum());
        _beta_prior_sum += beta_dist[i];
    }
    _beta_alias.initialize(beta_dist);

    return 0;
}

void GibbsSampler::sample_doc(LDADoc& doc) {
    int new_topic = -1;
    for (size_t i = 0; i < doc.size(); ++i) {
        new_topic = sample_token(doc, doc.token(i));
        doc.set_topic(i, new_topic);
    }
}

void GibbsSampler::sample_doc(SLDADoc& doc) {
    int new_topic = -1;
    for (size_t i = 0; i < doc.size(); ++i) {
        new_topic = sample_sentence(doc, doc.sent(i));
        doc.set_topic(i, new_topic);
    }
}

int GibbsSampler::sample_token(LDADoc& doc, Token& token) {
    int old_topic = token.topic;
    int num_topics = _model->num_topics();
    std::vector<float> accum_prob(num_topics, 0.0);
    std::vector<float> prob(num_topics, 0.0);
    float sum = 0.0;
    float dt_alpha = 0.0;
    float wt_beta = 0.0;
    float t_sum_beta_sum = 0.0;
    for (int t = 0; t < num_topics; ++t) {
        dt_alpha = doc.topic_sum(t) + _model->alpha();
        wt_beta = _model->word_topic(token.id, t) + _model->beta();
        t_sum_beta_sum = _model->topic_sum(t) + _model->beta_sum();
        if (t == old_topic && wt_beta > 1) {
            if (dt_alpha > 1) {
                dt_alpha -= 1;
            }
            wt_beta -= 1;
            t_sum_beta_sum -= 1;
        }
        prob[t] = dt_alpha * wt_beta / t_sum_beta_sum;
        sum += prob[t];
        accum_prob[t] = (t == 0 ? prob[t] : accum_prob[t - 1] + prob[t]);
    }
    
    double dart = rand() * sum;
    if (dart <= accum_prob[0]) {
        return 0;
    }
    for (int t = 1; t < num_topics; ++t) {
        if (dart > accum_prob[t - 1] && dart <= accum_prob[t]) {
            return t;
        }
    }

    return num_topics - 1; // 返回最后一个主题id
}

int GibbsSampler::sample_sentence(SLDADoc& doc, Sentence& sent) {
    int old_topic = sent.topic;
    int num_topics = _model->num_topics();
    std::vector<float> accum_prob(num_topics, 0.0);
    std::vector<float> prob(num_topics, 0.0);
    float sum = 0.0;
    float dt_alpha = 0.0;
    float t_sum_beta_sum = 0.0;
    float wt_beta = 0.0;
    // 为了保证数值计算的稳定，以下实现为SentenceLDA的采样近似实现
    for (int t = 0; t < num_topics; ++t) {
        dt_alpha = doc.topic_sum(t) + _model->alpha();
        t_sum_beta_sum = _model->topic_sum(t) + _model->beta_sum();
        if (t == old_topic) {
            if (dt_alpha > 1) {
                dt_alpha -= 1;
            }
            if (t_sum_beta_sum > 1) {
                t_sum_beta_sum -= 1;
            }
        }
        prob[t] = dt_alpha;
        for (size_t i = 0; i < sent.tokens.size(); ++i) {
            int w = sent.tokens[i];
            wt_beta = _model->word_topic(w, t) + _model->beta();
            if (t == old_topic && wt_beta > 1) {
                wt_beta -= 1;
            }
            // NOTE: 若句子长度过长，此处连乘项过多会导致概率过小, 丢失精度
            prob[t] *= wt_beta / t_sum_beta_sum;
        }
        sum += prob[t];
        accum_prob[t] = (t == 0 ? prob[t] : accum_prob[t - 1] + prob[t]);
    }
    double dart = rand() * sum;
    if (dart <= accum_prob[0]) { 
        return 0;
    }
    for (int t = 1; t < num_topics; ++t) {
        if (dart > accum_prob[t - 1] && dart <= accum_prob[t]) {
            return t;
        }
    }

    return num_topics - 1; // 返回最后一个主题id
}
} // namespace familia
