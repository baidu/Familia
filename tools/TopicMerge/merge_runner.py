#coding=utf-8

# Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Author: lianrongzhong@baidu.com

import sys
import argparse
import topic_model_merge

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--model_dir', action='store', default='../model/news',
                        dest='model_dir', help='the directory of model file')
    parser.add_argument('--conf', action='store', default='lda.conf',
                        dest='conf', help='the configure file')
    parser.add_argument('--k', action='store', default=30, type=int,
                        dest='topk', help='the number of selected words for each topic')
    parser.add_argument('--Jac_opt', action='store', type=int,
                        default=0, dest='Jac_opt',
                        help='0 stands for Jaccard Similarity, 1 stands for Weighted Jaccard Similarity')
    parser.add_argument('--threshold', action='store', type=float, default=0.4,
                        dest='threshold', help='the threshold to tell whether two topics are redundant')
    parser.add_argument('--output_file', action='store', default='./word_topic_merge.model',
                        dest='output_file', help='the output file of the merged model')
    paras = parser.parse_args()

    merger = topic_model_merge.TopicModelMerge(paras.model_dir, paras.conf)
    merger.reduce_topic(paras.topk, paras.threshold, paras.Jac_opt, paras.output_file)

