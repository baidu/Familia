#coding=utf-8

# Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import sys
from collections import defaultdict as ddict
import operator

class TopicModelFastMerge(object):
    """
    针对超大模型的主题去重，如果计算两两主题之间的相似度会非常耗时，我们引入simhash的快速去重算法。
    主要分为两步：基于Simhash的主题预分析以及基于Weighted Jaccard Similarity的Cluster内主题相关性分析。

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
            topic_word[topic_id].sort(key=operator.itemgetter(1), reverse=True)
            topk_items[topic_id] = [(item[0], float(item[1]) / topic_sum[topic_id]) for item in topic_word[topic_id][:topk]]
        return topk_items


    def string_hash(self, word_str, hashbits):
        """
        将一个字符串映射成一个哈希码

        Args:
            word_str: 输入字符串
            hashbits: 哈希码位数

        Returns:
            hash_code: 哈希码
        """
        if word_str == "":
            return 0
        hash_code = ord(word_str[0]) << 7
        m = 1000003
        mask = 2 ** hashbits - 1
        for ch in word_str:
            hash_code = ((hash_code * m) ^ ord(ch)) & mask
        hash_code ^= len(word_str)
        if hash_code == -1:
            hash_code = -2
        return hash_code


    def word_id_hash(self, hashbits):
        """
        为词表中的所有词生成哈希码

        Args:
            hashbits: 哈希码位数

        Returns:
            word_hash: 词表哈希词典
        """
        word_hash = ddict(int)
        with open(self._word_topic_file) as f:
            for line in f:
                word_id = line.strip().split(' ', 1)[0]
                word_hash[word_id] = self.string_hash(word_id, hashbits)
        return word_hash


    def topic_simhash(self, topic_word, topic_sum, hashbits):
        """
        对于该主题下的每个单词，计算word_id字符串对应的hash值，
        每个word_id也有相应的权值weight，为topic_count/topic_sum。
        若该word_id字符串对应的hash值为1001，加权后的值为weight -weight -weight weight，
        即hash某一位值为‘1’则为‘weight’，hash某一位值为‘0’则为‘-weight’。

        Args:
            topic_word: 主题-词格式的模型
            topic_sum: 每个主题下词的总数
            hashbits: 哈希码位数

        Returns:
            tpc_simhash: 主题哈希词典
        """
        word_hash = self.word_id_hash(hashbits)
        tpc_simhash = ddict(int)
        for tid in xrange(self._num_topics):
            topic_value = [0.0] * hashbits
            masks = [1 << i for i in xrange(hashbits)]
            for item in topic_word[tid]:
                w_hash = word_hash[str(item[0])]
                weight = float(item[1]) / topic_sum[tid]
                for index in xrange(hashbits):
                    if w_hash & masks[index]:
                        topic_value[index] += weight
                    else:
                        topic_value[index] -= weight
            simhash = 0
            for index in xrange(hashbits):
                if topic_value[index] >= 0:
                    simhash |= masks[index]
            tpc_simhash[tid] = simhash
        return tpc_simhash


    def topic_cluster(self, tpc_simhash, hashbits):
        """
        将相同哈希码的主题聚成一个类别

        Args:
            tpc_simhash: 主题哈希词典
            hashbits: 哈希码位数

        Returns:
            dis_sets: list类型，每个元素为一个set，存放属于同一类的主题ID
        """
        overlap_pair = []
        for tid1 in xrange(self._num_topics - 1):
            for tid2 in xrange(tid1 + 1, self._num_topics):
                topic_sim = (tpc_simhash[tid1] ^ tpc_simhash[tid2]) & ((1 << hashbits) -1)
                if topic_sim == 0:
                    overlap_pair.append((tid1, tid2))
        # 通过并查集合并主题对
        dis_sets = self.disjoint_set(overlap_pair)
        candidate_topic_cnt = 0
        for item in dis_sets:
            candidate_topic_cnt += len(item)
        print("There are {} candidate topic pairs with the same Simhash value in {} clusters.".format(
                candidate_topic_cnt, len(dis_sets)))
        return dis_sets


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


    def reduce_topic(self, topk, Jac_thresh, hashbits, output_file):
        """
        删除冗余主题，主要分为两步：
            1.基于Simhash的主题预分析;
            2.基于Weighted Jaccard Similarity的Cluster内主题相关性分析。

        Args:
            topk: 每个主题取的词个数
            Jac_thresh: 阈值，Jaccard Similarity高于该值则当成冗余主题对
            hashbits: 哈希码位数
            output_file: 去重模型保存文件

        Returns:
            None
        """
        topic_word, topic_sum = self.conv_topic_word()
        topk_items = self.select_topk(topic_word, topic_sum, topk)
        tpc_simhash = self.topic_simhash(topic_word, topic_sum, hashbits)
        # 获取主题聚类
        topic_clusters = self.topic_cluster(tpc_simhash, hashbits)
        topk_words = []
        for topic_id in xrange(self._num_topics):
            topk_words.append(set([item[0] for item in topk_items[topic_id]]))

        # 对主题聚类内进行相相关分析
        overlap_pair = []
        for index in xrange(len(topic_clusters)):
            cluster = list(topic_clusters[index])
            length = len(cluster)
            for index1 in xrange(length - 1):
                for index2 in xrange(index1 + 1, length):
                    tid1 = cluster[index1]
                    tid2 = cluster[index2]
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
        print("Merge {} redundant topic pairs into {} topics (sets).".format(
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

