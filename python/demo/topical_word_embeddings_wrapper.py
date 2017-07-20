#coding=utf-8

# Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Author: lianrongzhong@baidu.com

import sys
import topical_word_embeddings

class TopicalWordEmbeddingsWrapper:
    """Topical Word Embeddings Wrapper

    对Topical Word Embeddings进行包装，简化函数接口

    Attributes:
        _twe: 指向TopicalWordEmbeddings的对象指针
    """

    def __init__(self, model_dir, emb_file):
        """初始化TWE对象

        Args:
            model_dir: 模型目录路径
            emb_file: topical word embeddings模型文件
        """
        self._twe = topical_word_embeddings.init_twe(model_dir, emb_file)

    def __del__(self):
        """销毁TWE对象"""
        if topical_word_embeddings is not None:
            topical_word_embeddings.destroy_twe(self._twe)

    def nearest_words(self, word, k = 10):
        """寻求与目标词最相关的词语组

        对模型中的所有词语(不包哈目标词)进行检索，通过计算cosine相似度，返回最相关的k个词语

        Args:
            word: 目标词
            k: 设置返回最邻近词的个数，默认为10个

        Returns:
            返回一个list对象，长度为k，list中每个元素为tuple，包含了返回词以及该词与目标词的cosine相关性，
            并按照相关性从高到低排序。例如输入"篮球"目标词返回前10个结果：
            [(篮球队，0.833797),
             (排球， 0.833721)
             ...,
             (篮球圈, 0.752021)]

             如果输入目标词不在词典中，则返回None。
        """
        return topical_word_embeddings.nearest_words(self._twe, word, k)

    def nearest_words_around_topic(self, topic_id, k = 10):
        """寻求与目标主题最相关的词语组

        对模型中的所有词语进行检索，通过计算cosine相似度，返回最相关的k个词语

        Args:
            topic_id: 目标主题ID
            k: 设置返回最邻近词的个数，默认为10个

        Returns:
            返回一个list对象，长度为k，list中每个元素为tuple，包含了返回词以及该词与目标主题的cosine相关性，
            并按照相关性从高到低排序。例如输入主题ID"356",返回前10个结果：
            [(绿色，0.908283),
             (低碳， 0.857772)
             ...,
             (环保型, 0.658591)]

             如果输入目标主题超出范围，则返回None。
        """
        return topical_word_embeddings.nearest_words_around_topic(self._twe, topic_id, k)

