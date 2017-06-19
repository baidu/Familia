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
#include <algorithm>
#include <vector>

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::unordered_map;

DEFINE_string(vocabulary_path, "./", "the path of the vocabulary file");
DEFINE_string(item_topic_table_path, "./", "the path of the item_topic_table file");
DEFINE_int32(top_k, 20, "the nearest k words in a topic");
DEFINE_int32(num_topics, 2000, "the number of the topic");

const double EPS = 1e-8;

namespace familia {

struct WordCount {
    int32_t word_id;
    int32_t count;
    WordCount(int32_t id, int32_t cnt) : word_id(id), count(cnt) {}
};

struct WordCountCmpGreater {
    bool operator()(const WordCount& w1, const WordCount& w2) {
        return w1.count > w2.count;
    }
} word_count_cmp_greater;

// 加载词典信息
void load_vocabulary(
    const string& vocabulary_path,
    unordered_map<int32_t, string>& vocabulary) {
    std::ifstream infile(vocabulary_path);
    string line;

    while(std::getline(infile, line)) {
        // 词典每行格式: factor_name term word_id tf df
        vector<string> items;
        split(items, line, '\t');
        // 需要保证至少有前三列
        CHECK_GE(items.size(), 3);
        string word = items[1];
        string id = items[2];
        // 加入词典
        vocabulary.insert({std::stoi(id), word});
    }
    infile.close();
}

// 加载模型信息
void load_item_topic_table(
    const string& item_topic_table_path,
    vector<int64_t>& topic_sum_table,
    vector<vector<WordCount> >& topic_words) {
    std::ifstream infile(item_topic_table_path);
    string line;

    while(std::getline(infile, line)) {
        vector<string> items;
        split(items, line, ' ');
        CHECK_GE(items.size(), 2);

        int32_t word_id = std::stoi(items[0]);
        for (size_t i = 1; i < items.size(); i++) {
            vector<string> temps;
            split(temps, items[i], ':');
            CHECK_EQ(temps.size(), 2);
            int32_t topic_index = std::stoi(temps[0]);
            int32_t count = std::stoi(temps[1]);
            // 统计每个主题下词的总数
            topic_sum_table[topic_index] += count;
            WordCount current_word_count(word_id, count);
            topic_words[topic_index].push_back(current_word_count);
        }
    }
    infile.close();
}

// 展示指定主题下前k个词
class ShowTopicDemo {
public:
    ShowTopicDemo(
        int32_t num_topics,
        const string& vocabulary_path,
        const string& item_topic_table_path) {
        _num_topics = num_topics;
        _topic_sum_table.resize(_num_topics);
        _topic_words.resize(_num_topics);
        load_vocabulary(vocabulary_path, _vocabulary);
        load_item_topic_table(item_topic_table_path, _topic_sum_table, _topic_words);
        // 排序
        for (int32_t i = 0; i < num_topics; i++) {
            std::sort(_topic_words[i].begin(), _topic_words[i].end(), word_count_cmp_greater);
        }
    }


    ~ShowTopicDemo() = default;

    // 打印指定主题下的前k个词
    void show_topics(int topic_id, int k) {
        cout << "--------------------------------------------" << endl;
        if (topic_id >= 0 && topic_id < _num_topics) {
            if (k > (int)_topic_words[topic_id].size()) {
                k = _topic_words[topic_id].size();
            }
            for (int32_t i = 0; i < k; i++) {
                cout << _vocabulary[_topic_words[topic_id][i].word_id] << "\t"
                    << (float)(_topic_words[topic_id][i].count) / (_topic_sum_table[topic_id] + EPS)
                    << endl;
            }
        } else {
            cout << topic_id << " is illegal" << endl;
        }
    }
private:
    int32_t _num_topics;
    vector<int64_t> _topic_sum_table;
    unordered_map<int32_t, string> _vocabulary;
    vector<vector<WordCount> > _topic_words;
};
} // namespace familia

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    google::SetVersionString("1.0.0.0");
    string usage = string("Usage: ./show_topic_demo --vocabulary_path=\"PATH/TO/VOCABULARY\" ") +
                   string("--item_topic_table_path=\"PATH/TO/ITEM_TOPIC_TABLE\" ") +
                   string("--topic_num=\"2000\" ") +
                   string("--top_k=\"20\" ");
    google::SetUsageMessage(usage);
    google::ParseCommandLineFlags(&argc, &argv, true);

    familia::ShowTopicDemo st_demo(FLAGS_num_topics,
            FLAGS_vocabulary_path,
            FLAGS_item_topic_table_path);

    string topic_index;
    while (true) {
        cout << "请输入主题编号(0-" << FLAGS_num_topics - 1 << "):\t";
        getline(cin, topic_index);
        st_demo.show_topics(std::stoi(topic_index), FLAGS_top_k);
    }

    return 0;
}
