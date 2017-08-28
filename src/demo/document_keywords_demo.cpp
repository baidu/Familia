// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "familia/inference_engine.h"
#include "familia/semantic_matching.h"
#include "familia/tokenizer.h"

#include <gflags/gflags.h>
#include <iostream>
#include <iomanip>
#include <set>

using std::string;
using std::vector;
using std::ifstream;
using std::cin;
using std::cout;
using std::endl;
using std::set;

DEFINE_string(model_dir, "./", "model directory");
DEFINE_string(conf_file, "lda.conf", "model configuration");
DEFINE_string(emb_file, "./", "Topical Word Embedding (TWE) file");
DEFINE_string(model_type, "LDA", "LDA or TWE");
DEFINE_int32(top_k, 20, "the nearest k words");

namespace familia {

bool compare(const WordAndDis& a, const WordAndDis& b) {
    return a.distance > b.distance;
}

// 主题模型语义匹配计算Demo类
class DocKeywordsDemo {
public:
    DocKeywordsDemo() : _engine(FLAGS_model_dir, FLAGS_conf_file),
                        _twe(FLAGS_model_dir, FLAGS_emb_file) {
        // 初始化分词器, 加载主题模型词表
        _tokenizer = new SimpleTokenizer(FLAGS_model_dir + "/vocab_info.txt");
    }

    ~DocKeywordsDemo() = default;

    // 计算document内每个词与其之间的相关性
    // 可选的指标包括:
    // 1. document主题分布生成词的likelihood, 值越大越相关
    // 2. 基于TWE模型的相关性计算
    void cal_doc_words_similarity(const string& document) {
        // 分词
        vector<string> d_tokens;
        _tokenizer->tokenize(document, d_tokens);
        print_tokens("Doc Tokens", d_tokens);

        // 对长文本进行主题推断，获取主题分布
        LDADoc doc;
        _engine.infer(d_tokens, doc);
        vector<Topic> doc_topic_dist;
        doc.sparse_topic_dist(doc_topic_dist);

        // 计算文档内每个词与文档的相关性
        vector<WordAndDis> items;
        set<string> words;
        for (const auto& word : d_tokens) {
            if (words.find(word) != words.end()) {
                continue;
            }
            words.insert(word);
            vector<string> single_token;
            single_token.push_back(word);
            WordAndDis wd;
            wd.word = word;
            if (FLAGS_model_type == "LDA") {
                wd.distance = SemanticMatching::likelihood_based_similarity(single_token,
                                                                            doc_topic_dist,
                                                                            _engine.get_model());
            } else {
                wd.distance = SemanticMatching::twe_based_similarity(single_token,
                                                                     doc_topic_dist,
                                                                     _twe);
            }
            items.push_back(wd);
        }

        // 排序
        std::sort(items.begin(), items.end(), compare);

        // 打印结果
        cout << "Word                    Similarity          " << endl;
        cout << "--------------------------------------------" << endl;
        int size = items.size();
        for (int i = 0; i < FLAGS_top_k; i++) {
            if (i >= size) {
                break;
            }
            cout << std::left << std::setw(24) << items[i].word << "\t"
                << items[i].distance << endl;
        }
    }

    // 打印分词结果
    void print_tokens(const string& title, const vector<string>& tokens) {
        cout << title << ": ";
        for (size_t i = 0; i < tokens.size(); ++i) {
            cout << tokens[i] << " ";
        }
        cout << endl;
    }

private:
    InferenceEngine _engine;
    // Topic Word Embedding模型
    TopicalWordEmbedding _twe;
    // 分词器
    Tokenizer* _tokenizer;
};
} // namespace familia

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    google::SetVersionString("1.0.0.0");
    string usage = string("Usage: ./query_doc_sim_demo --model_dir=\"PATH/TO/MODEL\" ") + 
                   string("--emb_file=\"webpage_twe_lda.model\" ") +
                   string("--model_type=\"LDA\" ") +
                   string("--top_k=\"20\" ");
    google::SetUsageMessage(usage);
    google::ParseCommandLineFlags(&argc, &argv, true);

    familia::DocKeywordsDemo doc_keywords_demo;
    // 计算短文本与长文本的相似度
    string doc;
    while (true) {
        cout << "请输入:" << endl;
        getline(cin, doc);
        if (doc.size() == 0) {
            LOG(ERROR) << "Empty input!";
            continue;
        }
        doc_keywords_demo.cal_doc_words_similarity(doc);
    }

    return 0;
}
