#coding=utf-8

# Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import sys
import argparse
import topic_model_compress

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--model_dir', action='store', default='../model/news',
                        dest='model_dir', help='the directory of model file')
    parser.add_argument('--conf', action='store', default='lda.conf',
                        dest='conf', help='the configure file')
    parser.add_argument('--compress', action='store',
                        default='topic_compress', dest='compress_method',
                        help='Compress methods: topic_compress and alias_compress')
    parser.add_argument('--threshold', action='store', type=float, default=0.01,
                        dest='threshold', help='the threshold to decide whether to delete')
    parser.add_argument('--output_file', action='store', default='./word_topic_compress.model',
                        dest='output_file', help='the output file of the compressed model')
    paras = parser.parse_args()

    compressor = topic_model_compress.TopicModelCompress(paras.model_dir, paras.conf)
    if paras.compress_method == 'topic_compress':
        compressor.topic_compress(paras.threshold, paras.output_file)
    elif paras.compress_method == 'alias_compress':
        compressor.alias_compress(paras.threshold, paras.output_file)

