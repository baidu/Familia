// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// 
// Author: chenzeyu01@baidu.com

#ifndef FAMILIA_TOKENIZER_H
#define FAMILIA_TOKENIZER_H

#include <algorithm>
#include <fstream>
#include <string> 
#include <unordered_set>

#include "familia/util.h"

namespace familia {

// 分词器基类
class Tokenizer {
public:
    Tokenizer() = default;

    virtual ~Tokenizer() = default;

    virtual void tokenize(const std::string& text, std::vector<std::string>& result) const = 0;
};

// 简单版本FMM分词器，仅用于主题模型应用Demo，非真实业务应用场景使用
// NOTE: 该分词器只识别主题模型中词表的单词
class SimpleTokenizer : public Tokenizer {
public:
    SimpleTokenizer(const std::string& vocab_path) : _max_word_len(1) {
        load_vocab(vocab_path);
    }

    ~SimpleTokenizer() = default;

    // 对输入text字符串进行简单分词，结果存放在result中
    void tokenize(const std::string& text, std::vector<std::string>& result) const override; 

    // 检查word是否在词表中
    bool contains(const std::string& word) const;
private:
    // 加载分词词典, 与主题模型共享一套词典
    void load_vocab(const std::string& vocab_path);

    // 检查字符是否为英文字符
    static bool is_eng_char(char c) {
        // 'A' - 'Z' & 'a' - 'z'
       return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }

    // 返回对应字符的小写字符，如无对应小写字符则返回原字符
    static char tolower(char c) {
        if (c >= 'A' && c <= 'Z') {
            return 'a' + (c - 'A');
        } else {
            return c;
        }
    }
    
    // 词表中单词最大长度
    int _max_word_len;
    // 词典数据结构
    std::unordered_set<std::string> _vocab;
};
} // namespace familia
#endif // FAMILIA_TOKENIZER_H
