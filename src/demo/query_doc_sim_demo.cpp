// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "familia/inference_engine.h"
#include "familia/semantic_matching.h"
#include "familia/tokenizer.h"

#include <gflags/gflags.h>
#include <iostream>

using std::string;
using std::vector;
using std::ifstream;
using std::cin;
using std::cout;
using std::endl;

DEFINE_string(model_dir, "./", "model directory");
DEFINE_string(conf_file, "lda.conf", "model configuration");
DEFINE_string(emb_file, "./", "Topical Word Embedding (TWE) file");

namespace familia {

// 主题模型语义匹配计算Demo类
class QueryDocSimDemo {
public:
    QueryDocSimDemo() : _engine(FLAGS_model_dir, FLAGS_conf_file),
                        _twe(FLAGS_model_dir, FLAGS_emb_file) {
        // 初始化分词器, 加载主题模型词表
        _tokenizer = new SimpleTokenizer(FLAGS_model_dir + "/vocab_info.txt");
    }

    ~QueryDocSimDemo() = default;

    // 计算query (短文本) 与 document (长文本) 的相似度
    // 可选的指标包括:
    // 1. document主题分布生成query的likelihood, 值越大相似度越高
    // 2. 基于TWE模型的相似度计算 
    void cal_query_doc_similarity(const string& query, const string& document) {
        // 分词
        vector<string> q_tokens, d_tokens;
        _tokenizer->tokenize(query, q_tokens);
        _tokenizer->tokenize(document, d_tokens);
        print_tokens("Query Tokens", q_tokens);
        print_tokens("Doc Tokens", d_tokens);
       
        // 对长文本进行主题推断，获取主题分布
        LDADoc doc;
        _engine.infer(d_tokens, doc);
        vector<Topic> doc_topic_dist;
        doc.sparse_topic_dist(doc_topic_dist);
        
        float lda_sim = SemanticMatching::likelihood_based_similarity(q_tokens, 
                                                                      doc_topic_dist, 
                                                                      _engine.get_model());
        float twe_sim = SemanticMatching::twe_based_similarity(q_tokens, doc_topic_dist, _twe);

        cout << "LDA Similarity = " << lda_sim << endl
             << "TWE Similarity = " << twe_sim << endl;
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
                   string("--emb_file=\"webpage_twe_lda.model\" ");
    google::SetUsageMessage(usage);
    google::ParseCommandLineFlags(&argc, &argv, true);

    familia::QueryDocSimDemo qd_sim_demo;
    // 计算短文本与长文本的相似度
    string query;
    string doc;
    while (true) {
        cout << "请输入短文本:" << endl;
        getline(cin, query);
        if (query.size() == 0) {
            LOG(ERROR) << "Empty input!";
            continue;
        }
        cout << "请输入长文本:" << endl;
        getline(cin, doc);
        if (doc.size() == 0) {
            LOG(ERROR) << "Empty input!";
            continue;
        }
        qd_sim_demo.cal_query_doc_similarity(query, doc);
    }

    return 0;
}
