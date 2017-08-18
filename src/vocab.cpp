// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "familia/vocab.h"
#include "familia/util.h"

#include <fstream>
#include <vector>

namespace familia {

int Vocab::get_id(const std::string& term) const {
    auto it = _term2id.find(term);
    return it == _term2id.end() ? OOV : it->second;
}

size_t Vocab::size() const {
    return _term2id.size();
}

void Vocab::load(const std::string& vocab_file) {
    _term2id.clear();
    std::ifstream fin(vocab_file, std::ios::in);
    CHECK(fin) << "Failed to open vocab file!";

    std::string line;
    std::vector<std::string> term_id;
    while (getline(fin, line)) {
        term_id.clear();
        split(term_id, line, '\t');
        CHECK_EQ(term_id.size(), 5) << "Vocabulary file [" << vocab_file << "] format error!";
        std::string term = term_id[1];
        int id = std::stoi(term_id[2]);
        if (_term2id.find(term) != _term2id.end()) {
            LOG(ERROR) << "Duplicate word [" << term << "] in vocab file";
            continue;
        }
        _term2id[term] = id;
    }
    fin.close();

    LOG(INFO) << "Load vocabulary success! #vocabulary size = " << size();
}
} // namespace familia
