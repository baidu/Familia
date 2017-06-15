// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: chenzeyu01@baidu.com

#ifndef FAMILIA_VOCAB_H
#define FAMILIA_VOCAB_H

#include <unordered_map>

namespace familia {
// OOV: out of vocabulary, 表示单词不在词表中
constexpr int OOV = -1;

// 主题模型词表数据结构
// 主要负责明文单词到词id之间的映射, 若单词不在词表中，则范围OOV(-1)
class Vocab {    
public:
    Vocab() = default;
    // 范围给定明文单词的词id
    int get_id(const std::string& word) const; 

    // 加载词表
    void load(const std::string& vocab_file);

    // 返回词表大小
    size_t size() const;
  
    // no copying alowed
    Vocab(const Vocab&) = delete;
    Vocab& operator=(const Vocab&) = delete;
private:
    // 明文到id的映射
    std::unordered_map<std::string, int> _term2id;
};
} // familia
#endif // FAMILIA_VOCAB_H
