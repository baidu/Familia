#!/bin/bash

python TopicFastMerge/fastmerge_runner.py --model_dir=../model/news --conf=lda.conf --k=30 --threshold=0.4 --output_file=news_lda.fastmerge.model
