#coding=utf-8

# Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Author: lianrongzhong@baidu.com

import sys
import inference_engine

class InferenceEngineWrapper:
    """InferenceEngineWrapper

    对InferenceEngine进行封装，避免直接操作tokenizer等对象指针

    Attributes:
        _inference_engine: InferenceEngine对象指针，用来访问infer接口
        _tokenizer: Tokenizer对象指针，用来分词
        _emb_file: TopicalWordEmbeddings模型文件
        _twe: TopicalWordEmbeddings对象指针，如果_emb_file为空则不需创建
    """

    def __init__(self, model_dir, conf_file, emb_file = None):
        """
        初始化InferenceEngine,Tokenizer,TopicalWordEmbeddings对象指针

        Args:
            model_dir: 模型目录
            conf_file: 模型配置文件
            emb_file: TWE模型文件，可选参数
        """
        self._emb_file = emb_file
        self._inference_engine = inference_engine.init_inference_engine(model_dir, conf_file, 1)
        self._tokenizer = inference_engine.init_tokenizer(model_dir + "/vocab_info.txt")
        if self._emb_file is None:
            return
        self._twe = inference_engine.init_twe(model_dir, emb_file)

    def __del__(self):
        """销毁各个对象指针"""
        inference_engine.destroy_inference_engine(self._inference_engine)
        inference_engine.destroy_tokenizer(self._tokenizer)
        if self._emb_file is not None:
            inference_engine.destroy_twe(self._twe)

    def tokenize(self, input_text):
        """分词

        简易的FMM分词工具，只针对主题模型中出现的词表进行正向匹配

        Args:
            input_text: 输入文本，编码需要跟词表编码保持一致

        Returns:
            返回一个list对象，其中每个元素为分词后的结果。
        """
        return inference_engine.tokenize(self._tokenizer, input_text)

    def lda_infer(self, words):
        """LDA模型推断

        使用LDA模型对输入文本进行推断，得到其主题分布

        Args:
            words: 分词后的list结果

        Returns:
            返回一个list对象，存放输入文本对应的稀疏主题分布，list中每个元素为tuple
            每个tuple包含一个主题ID以及该主题对应的概率，按照概率从大到小排序。
            例如：
            [(15, 0.5), (10, 0.25), (1999, 0.25)]
        """
        seg_text = ' '.join(words)
        return inference_engine.lda_infer(self._inference_engine, seg_text)

    def slda_infer(self, sentences):
        """SentenceLDA模型推断

        使用SentenceLDA模型对输入文本进行推断，得到其主题分布

        Args:
            words: 分词后的list结果

        Returns:
            返回一个list对象，存放输入文本对应的稀疏主题分布，list中每个元素为tuple
            每个tuple包含一个主题ID以及该主题对应的概率，按照概率从大到小排序。
            例如：
            [(15, 0.5), (10, 0.25), (1999, 0.25)]
        """
        seg_text = ""
        for sent in sentences:
            for word in sent:
                seg_text += word + ' '
            seg_text = seg_text.strip() + '\t'
        seg_text = seg_text.strip()
        return inference_engine.slda_infer(self._inference_engine, seg_text)

    def cal_doc_distance(self, doc1, doc2):
        """计算长文本与长文本之间的距离

        计算两个长文本的主题分布之间的距离，包括jensen_shannon_divergence和hellinger_distance

        Args:
            doc1: 输入文档1，无需分词
            doc2: 输入文档2，无需分词

        Returns:
            返回一个list对象，其中有两个float元素，第一个表示jensen_shannon_divergence距离，
            第二个表示hellinger_distance距离。例如：
            [0.187232, 0.23431]
        """
        return inference_engine.cal_doc_distance(self._inference_engine,
                                                 self._tokenizer,
                                                 doc1,
                                                 doc2)

    def cal_query_doc_similarity(self, query, doc):
        """计算短文本与长文本之间的相关性

        使用LDA模型和TWE模型分别衡量短文本跟长文本之间的相关性

        Args:
            query: 输入短文本，无需分词
            doc: 输入长文本，无需分词

        Returns:
            返回一个list对象，其中有两个float元素，第一个表示根据LDA模型得到的相关性，
            第二个表示通过TWE模型衡量得到的相关性。例如：
            [0.397232, 0.45431]
        """
        # 如果没有加载TWE模型无法进行计算
        if self._emb_file is None:
            sys.stderr.write("Need to load Topical Word Embeddings file.\n")
            return
        return inference_engine.cal_query_doc_similarity(self._inference_engine,
                                                         self._tokenizer,
                                                         self._twe,
                                                         query,
                                                         doc)

