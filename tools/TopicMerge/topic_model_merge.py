#coding=utf-8

# Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Author: lianrongzhong@baidu.com

import sys
from collections import defaultdict as ddict
import operator

class TopicModelMerge(object):
    """
    对模型中重复度较高的主题进行聚合,采用Jaccard Similarity或Weighted Jaccard Similarity判断两个主题是否冗余,
    具体计算公式参考github相关文档。

    Attributes:
        _num_topics: 模型主题个数
        _word_topic_file: 主题模型文件
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
        self._word_topic_file = model_dir + '/' + parameters["word_topic_file"].strip('"')


    def conv_topic_word(self):
        """
        将词-主题格式转换为主题-词格式。
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


    def select_topk(self, topic_word, topic_sum, topk):
        """
        从每个主题中选取概率最大的k个词

        Args:
            topic_word: 主题-词表格
            topic_sum: 每个主题下的词总数
            topk: 每个主题选取词的个数

        Returns:
            topk_items: 存放每个主题前K个词以及对应的概率
        """
        topk_items = [[] for _ in xrange(self._num_topics)]
        for topic_id in xrange(self._num_topics):
            # 根据每个词的数目进行排序
            topic_word[topic_id].sort(key=operator.itemgetter(1), reverse=True)
            topk_items[topic_id] = [(item[0], float(item[1]) / topic_sum[topic_id]) for item in topic_word[topic_id][:topk]]
        return topk_items


    def disjoint_set(self, overlap_pair):
        """
        并查集算法

        Args:
            overlap_pair: list类型，每个元素为一个tuple，包含两个冗余的主题ID，例如：
                          [(1, 2), (2, 3), (4, 5)]
        Returns:
            results: list类型，每个元素为一个set，包含若干个冗余的主题ID，例如：
                          [set(1, 2, 3), set(4, 5)]
        """
        d = ddict(list)
        for index, (tid1, tid2) in enumerate(overlap_pair):
            d[tid1].append(index)
            d[tid2].append(index)

        sets = []
        while len(d):
            indexs = set(d.popitem()[1])
            temp_set = set()
            while len(indexs):
                temp_set |= indexs
                temp_list = list()
                for index in indexs:
                    for tid in overlap_pair[index]:
                        for ind in d.pop(tid, []):
                            temp_list += [ind]
                indexs = set(temp_list) - temp_set
            sets += [temp_set]

        results = []
        for indexs in sets:
            temp_list = []
            for index in indexs:
                for tid in overlap_pair[index]:
                    temp_list += [tid]
            results.append(set(temp_list))
        return results


    def reduce_topic(self, topk, Jac_thresh, Jac_opt, output_file):
        """
        根据Jaccard Similarity对冗余主题进行压缩

        Args:
            topk: 每个主题取的词个数
            Jac_thresh: 阈值，Jaccard Similarity高于该值则当成冗余主题对
            Jac_opt: 0 表示选用Jaccard Similarity，1表示选用Weighted Jaccard Similarity
            output_file: 去重模型保存文件

        Returns:
            None
        """
        topic_word, topic_sum = self.conv_topic_word()
        topk_items = self.select_topk(topic_word, topic_sum, topk)
        # 获取每个主题前K个词(不包含概率)
        topk_words = []
        for topic_id in xrange(self._num_topics):
            topk_words.append(set([item[0] for item in topk_items[topic_id]]))

        overlap_pair = []
        for tid1 in xrange(self._num_topics - 1):
            for tid2 in xrange(tid1 + 1, self._num_topics):
                if not Jac_opt:             # Jaccard Similarity
                    overlap = float(len(topk_words[tid1] & topk_words[tid2])) / len(topk_words[tid1] | topk_words[tid2])
                else:                       # Weighted Jaccard Similarity
                    overlap_set = topk_words[tid1] & topk_words[tid2]
                    min_value = {}
                    for word in overlap_set:
                        min_value[word] = 1.0
                    w_union = 0.0
                    for word, prob in topk_items[tid1]:
                        w_union += prob
                        if word in overlap_set and min_value[word] > prob:
                            min_value[word] = prob
                    for word, prob in topk_items[tid2]:
                        w_union += prob
                        if word in overlap_set and min_value[word] > prob:
                            min_value[word] = prob
                    w_inter = 0.0
                    for prob in min_value.itervalues():
                        w_inter += prob
                    overlap = w_inter / (w_union - w_inter)
                # 添加高于阈值的主题对
                if overlap >= Jac_thresh:
                    overlap_pair.append((tid1, tid2))
        # 使用并查集算法合并主题对
        dis_sets = self.disjoint_set(overlap_pair)
        redundant_topic_cnt = 0
        for item in dis_sets:
            redundant_topic_cnt += len(item)
        print("Merge {} redundant topics into {} topics (sets).".format(
                redundant_topic_cnt, len(dis_sets)))
        # 对属于同一类的主题进行合并
        for index in xrange(len(dis_sets)):
            current_list = list(dis_sets[index])
            for topic_id in current_list[1:]:
                topic_word[current_list[0]] += topic_word[topic_id]
                topic_word[topic_id] = []

        # 转换成word-topic形式
        word_topic = ddict(list)
        new_topic_index = -1
        for topic_id in xrange(self._num_topics):
            if topic_word[topic_id] == []:
                continue
            new_topic_index += 1
            word_dict = ddict(int)
            for word_id, cnt in topic_word[topic_id]:
                word_dict[word_id] += cnt
            for word_id, cnt in word_dict.items():
                word_topic[word_id].append("{}:{}".format(new_topic_index, cnt))
        print("Now, in total we have {} refined topics.".format(new_topic_index + 1))
        # 输出模型到文件
        out_file = open(output_file, 'w')
        for word_id in word_topic.keys():
            out_file.writelines("{} {}\n".format(word_id, ' '.join(word_topic[word_id])))
        out_file.close()

