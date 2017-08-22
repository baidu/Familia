#!/bin/bash

python TopicCompress/compress_runner.py --model_dir=../model/news --conf=lda.conf --compress=topic_compress --threshold=0.001 --output_file=news_lda.compress.model
