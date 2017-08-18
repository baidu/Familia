// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "familia/semantic_matching.h"
#include "familia/util.h"

using std::vector;
using std::string;

namespace familia {

// 根据topic id返回对应的topic embedding
Embedding& TopicalWordEmbedding::topic_emb(int topic_id) {
    CHECK_GE(topic_id, 0) << "Topic id out of range";
    CHECK_LT(topic_id, _num_topics) << "Topic id out of range";

    return _topic_emb[topic_id];
}

Embedding& TopicalWordEmbedding::word_emb(const string& term) {
    CHECK(contains_word(term)) << term << " out of vocabulary!";
    return _word_emb[term];
}

bool TopicalWordEmbedding::contains_word(const string& term) const {
    return _word_emb.find(term) == _word_emb.end() ? false : true;
}

int TopicalWordEmbedding::num_topics() const {
    return _num_topics;
}

int TopicalWordEmbedding::load_emb(const string& emb_file) {
    LOG(INFO) << "Loading Topical Word Embedding (TWE)...";
    FILE* fin_emb = fopen(emb_file.c_str(), "rb");
    CHECK(fin_emb) << "Error to open embedding file!";

    fscanf(fin_emb, "%d%d%d\n", &_vocab_size, &_num_topics, &_emb_size);

    LOG(INFO) << "#word = " << _vocab_size
              << " #topic = " << _num_topics
              << " #emb_size = " << _emb_size;

    const int MAX_TOKEN_LENGTH = 50;
    char term[MAX_TOKEN_LENGTH];
    Embedding emb(_emb_size, 0);
    int total_num = _vocab_size + _num_topics;
    // TWE模型存储格式：
    // 单词明文 \空格 二进制embedding \n
    // 0~vocab_size-1行为word embedding...
    // _topic_#ID \空格 二进制embedding \n
    // 随后num_topics行为topic embedding
    for (int i = 0; i < total_num; ++i) {
        if (i % 100000 == 0) {
            LOG(INFO) << "Loading embedding #id = " << i;
        }
        fread(term, sizeof(char), MAX_TOKEN_LENGTH, fin_emb);
        if (i < _vocab_size) { 
            // 加载word embedding
            _word_emb[term] = Embedding(_emb_size, 0);
            fread(_word_emb[term].data(), sizeof(Embedding::value_type), _emb_size, fin_emb);
            // fgetc(fin_emb); // 跳过\n
        } else { 
            // 加载topic embedding
            fread(emb.data(), sizeof(Embedding::value_type), _emb_size, fin_emb);
            _topic_emb.push_back(emb);
        }
    }
    fclose(fin_emb);
    LOG(INFO) << "Load Topical Word Embedding (TWE) successully!";

    return 0;
}

void TopicalWordEmbedding::nearest_words(const string& word,
                                         std::vector<WordAndDis>& items) {
    Embedding& target_word_emb = word_emb(word);
    int num_k = items.size();
    for (const auto& it : _word_emb) {
        if (it.first == word) {
            continue;
        }
        float dist = SemanticMatching::cosine_similarity(target_word_emb, it.second);
        for (int i = 0; i < num_k; i++) {
            if (dist > items[i].distance) {
                for (int j = num_k - 1; j > i; j--) {
                    items[j] = items[j - 1];
                }
                items[i].word = it.first;
                items[i].distance = dist;
                break;
            }
        }
    }
}

void TopicalWordEmbedding::nearest_words_around_topic(int topic_id,
                                                      std::vector<WordAndDis>& items) {
    Embedding& target_topic_emb = topic_emb(topic_id);
    int num_k = items.size();
    for (const auto& it : _word_emb) {
        float dist = SemanticMatching::cosine_similarity(target_topic_emb, it.second);
        for (int i = 0; i < num_k; i++) {
            if (dist > items[i].distance) {
                for (int j = num_k - 1; j > i; j--) {
                    items[j] = items[j - 1];
                }
                items[i].word = it.first;
                items[i].distance = dist;
                break;
            }
        }
    }
}
} // namespace familia
