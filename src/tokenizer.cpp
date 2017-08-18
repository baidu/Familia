// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "familia/tokenizer.h"

#include <iostream>
#include <string>
#include <vector>

namespace familia {

void SimpleTokenizer::tokenize(const std::string& text, std::vector<std::string>& result) const {
    result.clear();
    std::string word;
    std::string found_word;
    int text_len = text.size();
    for (int i = 0; i < text_len; ++i) {
        word.clear();
        found_word = "";
        // 处理英文字符的分支
        if (is_eng_char(text[i])) {
            // 遍历至字符串末尾\0以保证纯英文串切分
            for (int j = i; j <= text_len; ++j) {
                // 一直寻找英文字符,直到遇到非英文字符串
                if (j < text_len && is_eng_char(text[j])) {
                    // 词表中只包含小写字母单词, 对所有英文字符均转小写
                    word.push_back(tolower(text[j]));
                } else {
                    // 按字符粒度正向匹配
                    if (_vocab.find(word) != _vocab.end()) {
                        result.push_back(word);
                    }
                    i = j - 1;
                    break;
                }
            }
        } else {
            for (int j = i; j < i + _max_word_len && j < text_len; ++j) {
                word.push_back(text[j]);
                if (_vocab.find(word) != _vocab.end()) {
                    found_word = word; 
                }
            }
            if (found_word.size() > 0) {
                result.push_back(found_word);
                i += found_word.size() - 1;
            }
        }
    }
}

bool SimpleTokenizer::contains(const std::string& word) const {
    return _vocab.find(word) != _vocab.end();
}

void SimpleTokenizer::load_vocab(const std::string& vocab_path) {
    LOG(INFO) << "Loading vocabulary file from " << vocab_path;
    std::ifstream fin(vocab_path, std::ios::in);
    CHECK(fin) << "Open vocabulary file failed!";

    std::string line;
    int vocab_size = 0;
    while (getline(fin, line)) {
        std::vector<std::string> fields;
        split(fields, line, '\t');
        CHECK_GE(fields.size(), 2);
        std::string word = fields[1];
        _max_word_len = std::max(static_cast<int>(word.size()), _max_word_len);
        _vocab.insert(word);
        ++vocab_size;
    }
    LOG(INFO) << "Vocabulary load successfully! #vocab_size = " << vocab_size;
}
} // namespace familia
