// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: chenzeyu01@baidu.com

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

DEFINE_string(work_dir, "./", "working directory");
DEFINE_string(conf_file, "lda.conf", "model configuration");
DEFINE_string(emb_file, "./", "Topical Word Embedding (TWE) file");
DEFINE_int32(mode, 0, "0: query-doc similarity  1: doc-doc semantic distance");

namespace familia {

// 主题模型语义匹配计算Demo类
class SemanticMatchingDemo {
public:
    SemanticMatchingDemo() : _engine(FLAGS_work_dir, FLAGS_conf_file) , 
                             _twe(FLAGS_work_dir, FLAGS_emb_file) {
        // 初始化分词器, 加载主题模型词表
        _tokenizer = new SimpleTokenizer(FLAGS_work_dir + "/vocab_info.txt");
    }

    ~SemanticMatchingDemo() = default;

    // 计算query (短文本) 与 content (长文本) 的相似度
    // 可选的指标包括:
    // 1. content主题分布生成query的likelihood, 值越大相似度越高
    // 2. 基于TWE模型的相似度计算 
    void cal_query_content_similarity(const string& query, const string& content) {
        // 分词
        vector<string> q_tokens, c_tokens;
        _tokenizer->tokenize(query, q_tokens);
        _tokenizer->tokenize(content, c_tokens);
        print_tokens("Query Tokens", q_tokens);
        print_tokens("Content Tokens", c_tokens);
       
        // 对长文本content进行主题推断，并获取主题分布
        LDADoc doc;
        _engine.infer(c_tokens, doc);
        vector<Topic> doc_topic_dist;
        doc.sparse_topic_dist(doc_topic_dist);
        
        float lda_sim = SemanticMatching::likelihood_based_similarity(q_tokens, 
                                                                      doc_topic_dist, 
                                                                      _engine.get_model());
        float twe_sim = SemanticMatching::twe_based_similarity(q_tokens, doc_topic_dist, _twe);

        cout << "LDA Similarity = " << lda_sim << endl
             << "TWE Similarity = " << twe_sim << endl;
    }

    // 计算长文本之间的相似度
    // 可选的指标包括常用的分布间距离jensen shannon diveregnce和hellinger distance
    void cal_doc_distance(const string& doc_text1, const string& doc_text2) {
        // 分词
        vector<string> doc1_tokens, doc2_tokens;
        _tokenizer->tokenize(doc_text1, doc1_tokens);
        _tokenizer->tokenize(doc_text2, doc2_tokens);
        print_tokens("Doc1 Tokens", doc1_tokens);
        print_tokens("Doc2 Tokens", doc2_tokens);
    
        // 文档主题推断, 输入分词结果，主题推断结果存放于LDADoc中
        LDADoc doc1, doc2;
        _engine.infer(doc1_tokens, doc1);
        _engine.infer(doc2_tokens, doc2);
    
        // 计算jsd需要传入稠密型分布
        // 获取稠密的文档主题分布
        vector<float> dense_dist1;
        vector<float> dense_dist2;
        doc1.dense_topic_dist(dense_dist1);
        doc2.dense_topic_dist(dense_dist2);
    
        // 计算分布之间的距离, 值越小则表示文档语义相似度越高
        float jsd = SemanticMatching::jensen_shannon_divergence(dense_dist1, dense_dist2);
        float hd = SemanticMatching::hellinger_distance(dense_dist1, dense_dist2);
        cout << "Jensen Shannon Divergence = " << jsd << endl
             << "Hellinger Distance = " << hd << endl;
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

// 语义匹配类型
enum MatchingType {
    QueryDocSim = 0, // 短文本与长文本相似度
    DocDistance = 1  // 文档间距离
};

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    google::SetVersionString("1.0.0.0");
    string usage = string("Usage: ./semantic_matching_demo --work_dir=\"PATH/TO/MODEL\" ") + 
                   string("--conf_file=\"lda.conf\" ") + 
                   string("--emb_file=\"webpage_twe_lda.model\" ");
    google::SetUsageMessage(usage);
    google::ParseCommandLineFlags(&argc, &argv, true);

    familia::SemanticMatchingDemo sm_demo;
    // 计算短文本与长文本的相似度
    if (FLAGS_mode == MatchingType::QueryDocSim) {
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
            sm_demo.cal_query_content_similarity(query, doc);
        }
    } else if (FLAGS_mode == MatchingType::DocDistance) {
        string doc1;
        string doc2;
        while (true) {
            cout << "请输入文档1:" << endl;
            getline(cin, doc1);
            if (doc1.size() == 0) {
                LOG(ERROR) << "Empty input!";
                continue;
            }
            cout << "请输入文档2:" << endl;
            getline(cin, doc2);
            if (doc2.size() == 0) {
                LOG(ERROR) << "Empty input!";
                continue;
            }
            sm_demo.cal_doc_distance(doc1, doc2);
        }
    }

    return 0;
}
