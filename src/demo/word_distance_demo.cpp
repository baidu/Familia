// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "familia/semantic_matching.h"

#include <gflags/gflags.h>
#include <iostream>
#include <iomanip>

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;

DEFINE_string(model_dir, "./", "model directory");
DEFINE_string(emb_file, "./", "Topical Word Embedding (TWE) file");
DEFINE_int32(top_k, 20, "the nearest k words");

namespace familia {
// 寻找距离词最邻近的K个词Demo类
class WordDistanceDemo {
public:
    WordDistanceDemo() : _twe(FLAGS_model_dir, FLAGS_emb_file) {
    }

    ~WordDistanceDemo() = default;

    // 获取词距离最近的k个词，并打印出来
    void find_nearest_words(const string& word, int k) {
        vector<WordAndDis> items(k);
        // 如果词不存在模型词典，则返回
        if (!_twe.contains_word(word)) {
            cout << word << " is out of vocabulary." << endl;
            return;
        }

        _twe.nearest_words(word, items);
        print_result(items);
    }

    // 打印结果
    void print_result(const std::vector<WordAndDis>& items) {
        cout << "Word                       Cosine Distance  " << endl;
        cout << "--------------------------------------------" << endl;
        for (const auto& item : items) {
            cout << std::left << std::setw(24) << item.word << "\t" 
                 << item.distance << endl;
        }
        cout << endl;
    }

private:
    // Topic Word Embedding模型
    TopicalWordEmbedding _twe;
};
} // namespace familia

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    google::SetVersionString("1.0.0.0");
    string usage = string("Usage: ./word_distance_demo --model_dir=\"PATH/TO/MODEL\" ") +
                   string("--emb_file=\"webpage_twe_lda.model\" ") +
                   string("--top_k=\"20\" ");
    google::SetUsageMessage(usage);
    google::ParseCommandLineFlags(&argc, &argv, true);

    familia::WordDistanceDemo wd_demo;

    string word;
    while (true) {
        cout << "请输入词语:\t";
        getline(cin, word);
        wd_demo.find_nearest_words(word, FLAGS_top_k);
    }

    return 0;
}
