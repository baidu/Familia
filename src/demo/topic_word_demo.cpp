// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: lianrongzhong@baidu.com

#include "familia/semantic_matching.h"
#include "familia/util.h"

#include <gflags/gflags.h>
#include <iostream>
#include <iomanip>
#include <fstream>

using std::string;
using std::vector;
using std::unordered_map;
using std::make_pair;
using std::cin;
using std::cout;
using std::endl;

DEFINE_string(work_dir, "./", "Working directory");
DEFINE_string(emb_file, "./", "Topical Word Embedding (TWE) file");
DEFINE_string(topic_words_file, "./", "Topic word file");

namespace familia {
// 主题词展示Demo类
class TopicWordDemo {
public:
    TopicWordDemo() : _twe(FLAGS_work_dir, FLAGS_emb_file) {
        // 加载主题模型每个主题的返回词
        load_topic_words(FLAGS_work_dir + "/" + FLAGS_topic_words_file);
    }

    ~TopicWordDemo() = default;

    // 展示同个主题下不同召回方式的结果
    void show_topics(int topic_id)  {
        // 获取TWE模型下每个主题最相关的词
        vector<WordAndDis> items(_top_k);
        _twe.nearest_words_around_topic(topic_id, items);
        print_result(items, _topic_words[topic_id]);
    }

    // 打印结果
    void print_result(const vector<WordAndDis>& items,
                      const vector<string>& words) {
        cout << "TWE result                      LDA result" << endl;
        cout << "---------------------------------------------" << endl;
        for (int i = 0; i < _top_k; i++) {
             cout << std::left << std::setw(30) << items[i].word << "\t\t" << words[i] << endl;
        }
        cout << endl;
    }

    int num_topics() const {
        return _twe.num_topics();
    }

private:
    // 读取主题模型的每个主题下的展现结果
    void load_topic_words(const string& topic_words_file) {
        std::ifstream fin(topic_words_file, std::ios::in);
        CHECK(fin) << "Failed to open topic word file!";
        string line;
        for (int t = 0; t < num_topics(); t++) {
            // 读取每个主题第一行信息，解析出topk
            getline(fin, line);
            vector<string> cols;
            split(cols, line, '\t');
            CHECK_EQ(cols.size(), 2) << "Format of the topic_words file error!";
            int topk = std::stoi(cols[1]);
            // 读取多余行
            getline(fin, line);
            // 读取前k个词
            vector<string> words;
            for (int i = 0; i < topk; i++) {
                string line;
                vector<string> cols;
                getline(fin, line);
                split(cols, line, '\t');
                words.push_back(cols[0]);
            }
            _topic_words[t] = words;
        }
    }

    TopicalWordEmbedding _twe;
    // LDA中每个主题下出现概率最高的词
    unordered_map<int, vector<string>> _topic_words;
    // 每个主题下展示的词的数目, 默认为10
    static constexpr int _top_k = 10;
};
} // namespace familia

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    google::SetVersionString("1.0.0.0");
    string usage = string("Usage: ./topic_distance_demo --work_dir=\"PATH/TO/MODEL\" ") +
                   string("--emb_file=\"webpage_twe_lda.model\" ") +
                   string("--topic_words_file=\"topic_words.txt\" ");
    google::SetUsageMessage(usage);
    google::ParseCommandLineFlags(&argc, &argv, true);

    familia::TopicWordDemo tw_demo;
    
    string line;
    while(true) {
        cout << "请输入主题编号(0-" << tw_demo.num_topics() - 1 << "):\t";
        getline(cin, line);
        int topic_id = std::stoi(line);
        CHECK_GE(topic_id, 0) << "Topic out of range!";
        CHECK_LT(topic_id, tw_demo.num_topics()) << "Topic out of range!";
        // 展示主题下TWE与LDA召回的词
        tw_demo.show_topics(topic_id);
    }
    return 0;
}
