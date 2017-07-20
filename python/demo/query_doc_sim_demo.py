#coding=utf-8

# Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Author: lianrongzhong@baidu.com

import sys
from familia_wrapper import InferenceEngineWrapper

if __name__ == '__main__':
    if len(sys.argv) < 4:
        sys.stderr.write("Usage:python {} {} {} {}.\n".format(
            sys.argv[0], "model_dir", "conf_file", "emb_file"))
        exit(-1)

    # 获取参数
    model_dir = sys.argv[1]
    conf_file = sys.argv[2]
    emb_file = sys.argv[3]
    # 创建InferenceEngineWrapper对象
    inference_engine_wrapper = InferenceEngineWrapper(model_dir, conf_file, emb_file)
    while True:
        # 输入短文本和长文本
        query = raw_input("Enter Query: ").strip()
        doc = raw_input("Enter Document: ").strip()
        distances = inference_engine_wrapper.cal_query_doc_similarity(query, doc)
        # 打印结果
        print("LDA Similarity = {}".format(distances[0]))
        print("TWE similarity = {}".format(distances[1]))
