// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: chenzeyu01@baidu.com

#include "familia/model.h"

#include <fstream>

namespace familia {

TopicModel::TopicModel(const std::string& model_dir, const ModelConfig& config) {
    _num_topics = config.num_topics();
    _beta = config.beta();
    _alpha = config.alpha();
    _alpha_sum = _alpha * _num_topics;
    _topic_sum = std::vector<uint64_t>(_num_topics, 0);
    _type = config.type();

    // 加载模型
    load_model(model_dir + "/" + config.word_topic_file(), model_dir + "/" + config.vocab_file());
}

uint64_t TopicModel::topic_sum(int topic_id) const {
    return _topic_sum.at(topic_id); 
}

void TopicModel::load_model(const std::string& word_topic_path,
                            const std::string& vocab_path) {
    LOG(INFO) << "Loading model: " << word_topic_path;
    LOG(INFO) << "Loading vocab: " << vocab_path;

    // loading vocabulary
    _vocab.load(vocab_path);

    _beta_sum = _beta * _vocab.size();
    _word_topic = std::vector<TopicDist>(_vocab.size());
   
    load_word_topic(word_topic_path);

    LOG(INFO) << "Model Info: #num_topics = " << num_topics() << " #vocab_size = " << vocab_size()
              << " alpha = " << alpha() << " beta = " << beta();
}

void TopicModel::load_word_topic(const std::string& word_topic_path) {
    LOG(INFO) << "Loading word topic from " << word_topic_path;
    std::ifstream fin(word_topic_path.c_str(), std::ios::in);
    CHECK(fin) << "Failed to open word topic file!";

    std::string line;
    while (getline(fin, line)) {
        std::vector<std::string> fields;
        split(fields, line, ' ');

        CHECK_GT(fields.size(), 0) << "Model file format error!";

        int term_id = std::stoi(fields[0]);

        CHECK_LT(term_id, vocab_size()) << "Term id out of range!";
        CHECK_GE(term_id, 0) << "Term id out of range!";

        for (size_t i = 1; i < fields.size(); ++i) {
            std::vector<std::string> topic_count;
            split(topic_count, fields[i], ':');
            CHECK_EQ(topic_count.size(), 2) << "Topic count format error!";

            int topic_id = std::stoi(topic_count[0]);
            CHECK_GE(topic_id, 0) << "Topic out of range!";
            CHECK_LT(topic_id, _num_topics) << "Topic out of range!";

            int count = std::stoi(topic_count[1]);
            CHECK_GT(count, 0) << "Topic count error!";
            
            _word_topic[term_id].emplace_back(topic_id, count);
            _topic_sum[topic_id] += count;
        }
        // 按照主题下标进行排序
        std::sort(_word_topic[term_id].begin(), _word_topic[term_id].end());
    }

    fin.close();
    LOG(INFO) << "Word topic load successfully!";
}
} // namespace familia
