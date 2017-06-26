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

DEFINE_string(model_dir, "./", "model directory");
DEFINE_string(conf_file, "lda.conf", "model configuration");

namespace familia {

// 主题模型语义匹配计算Demo类
class DocDistanceDemo {
public:
    DocDistanceDemo() : _engine(FLAGS_model_dir, FLAGS_conf_file) {
        // 初始化分词器, 加载主题模型词表
        _tokenizer = new SimpleTokenizer(FLAGS_model_dir + "/vocab_info.txt");
    }

    ~DocDistanceDemo() = default;

    // 计算长文本之间的相似度
    // 可选的指标包括常用的分布间距离Jensen-Shannon Diveregnce和Hellinger Distance
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
        cout << "Jensen-Shannon Divergence = " << jsd << endl
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
    // 分词器
    Tokenizer* _tokenizer;
};
} // namespace familia

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    google::SetVersionString("1.0.0.0");
    string usage = string("Usage: ./doc_distance_demo --model_dir=\"PATH/TO/MODEL\" ") + 
                   string("--conf_file=\"lda.conf\" ");
    google::SetUsageMessage(usage);
    google::ParseCommandLineFlags(&argc, &argv, true);

    familia::DocDistanceDemo doc_dis_demo;
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
        doc_dis_demo.cal_doc_distance(doc1, doc2);
    }

    return 0;
}
