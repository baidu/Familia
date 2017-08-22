#coding=utf-8

# Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import sys
from collections import defaultdict as ddict

class TopicModelCompress(object):
    """TopicModelCompress

    主题模型压缩,提供了两种压缩方式：主题维度压缩(topic_compress)和采样维度压缩(alias_compress)。

    Attributes:
        _num_topics: 模型主题个数
        _beta: beta参数
        _word_topic_file: 主题模型文件
        _num_vocabs: 词表大小
    """

    def __init__(self, model_dir, conf_file):
        """
        通过传进来配置文件初始化参数

        Args:
            model_dir: 模型目录
            conf_file: 模型配置文件
        """
        parameters = self.config_parser(model_dir + '/' + conf_file)
        self._num_topics = int(parameters["num_topics"])
        self._beta = float(parameters["beta"])
        self._word_topic_file = model_dir + '/' + parameters["word_topic_file"].strip('"')
        # 初始化词表大小为0，后续统计
        self._num_vocabs = 0


    def conv_topic_word(self):
        """
        将词-主题格式转换成主题-词格式。
        词-主题表格存放每行格式为：词ID 主题ID:个数 ...  主题ID:个数
        转换成主题-词存储，格式为：主题ID 词ID:个数 ... 词ID:个数

        Args:
            None

        Returns:
            topic_word: 转换后的主题-词模型
            topic_sum: 统计每个主题下词的总数
        """
        topic_word = [[] for _ in xrange(self._num_topics)]
        topic_sum = [0] * self._num_topics
        with open(self._word_topic_file) as f:
            for line in f:
                cols = line.strip().split()
                word_id = int(cols[0])
                self._num_vocabs += 1
                for index in range(1, len(cols)):
                    topic_id, cnt = [int(item) for item in cols[index].split(':')]
                    topic_word[topic_id].append((word_id, cnt))
                    topic_sum[topic_id] += cnt
        return topic_word, topic_sum


    def config_parser(self, config):
        """
        读取配置文件，解析参数。

        Args:
            config: 模型配置

        Returns:
            parameters: 模型配置词典
        """
        parameters = {}
        with open(config) as f:
            for line in f:
                if line.startswith('#'):
                    continue
                cols = line.strip().split(':')
                if len(cols) != 2:
                    continue
                parameters[cols[0].strip()] = cols[1].strip()
        return parameters

    def topic_compress(self, compress_thresh, output_file):
        """
        主题维度压缩，考虑每个主题下每个词的占比，删除一些占比较低的词

        Args:
            compress_thresh: 阈值，低于该阈值的词删除
            output_file: 输出压缩后模型文件

        Returns:
            None
        """
        topic_word, topic_sum = self.conv_topic_word()
        # 遍历每个主题，删除低于阈值的词语
        for topic_id in range(self._num_topics):
            threshold = compress_thresh * topic_sum[topic_id]
            topic_word[topic_id] = filter(lambda item: item[1] >= threshold, topic_word[topic_id])

        # 重新转换成词-主题格式
        word_topic = ddict(list)
        for topic_id in range(self._num_topics):
            for word_id, cnt in topic_word[topic_id]:
                word_topic[word_id].append("{}:{}".format(topic_id, cnt))

        # 保存压缩后模型
        out_file = open(output_file, 'w')
        for word_id in word_topic.keys():
            out_file.writelines("{} {}\n".format(word_id, ' '.join(word_topic[word_id])))
        out_file.close()


    def alias_compress(self, compress_thresh, output_file):
        """
        采样维度压缩，与alias方法保持一致在采样维度上的压缩。
        计算每个词在每个主题下的占比，即(count+beta)／(topic_id_sum+V*beta),其中V是指词表大小，beta是主题模型的超参
        删除占比较低的主题条目

        Args：
            compress_thresh: 阈值参数，低于该值被删除
            output_file: 压缩后模型文件

        Returns:
            None
        """
        _, topic_sum = self.conv_topic_word()
        beta_sum = self._beta * self._num_vocabs
        word_topic = ddict(list)
        out_file = open(output_file, 'w')
        with open(self._word_topic_file) as f:
            # 逐行处理每个词语
            for line in f:
                cols = line.strip().split()
                word_id = int(cols[0])
                proportion = 0.0
                word_topic_row = []
                for item in cols[1:]:
                    topic_id, cnt = [int(a) for a in item.split(':')]
                    # 计算在对应主题下占比
                    item_proportion = (cnt + self._beta) / (topic_sum[topic_id] + beta_sum)
                    proportion += item_proportion
                    word_topic_row.append((topic_id, cnt, item_proportion))
                threshold = proportion * compress_thresh
                # 过滤低于阈值的主题条目
                word_topic_row = filter(lambda item: item[2] >= threshold, word_topic_row)

                # 输出压缩后词语
                if word_topic_row:
                    topic_dist = ' '.join([str(topic_id)+':'+str(cnt) for topic_id, cnt, _ in word_topic_row])
                    out_file.writelines("{} {}\n".format(word_id, topic_dist))
        out_file.close()
