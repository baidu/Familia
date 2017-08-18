#coding=utf-8

# Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import sys
from familia_wrapper import TopicalWordEmbeddingsWrapper

if sys.version_info < (3,0):
    input = raw_input

def print_result(result_list):
    # 分隔符
    print("Word\t\t\tCosine Distance")
    print("-" * 40)
    # 判断list是否为空，为空则返回
    if result_list is None:
        return
    for (word, distance) in result_list:
        print("{}\t\t\t{}".format(word, distance))

if __name__ == "__main__":
    if len(sys.argv) < 3:
        sys.stderr.write("Usage: python {} {} {}\n".format(
            sys.argv[0], "model_dir", "emb_file"))
        exit(-1)
    model_dir = sys.argv[1]
    emb_file = sys.argv[2]
    # 创建topical_word_embeddings_wrapper对象
    twe_wrapper = TopicalWordEmbeddingsWrapper(model_dir, emb_file)
    while True:
        word = input("Enter word: ")
        result_list = twe_wrapper.nearest_words(word.strip(), 10)
        print_result(result_list)
