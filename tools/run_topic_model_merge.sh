#!/bin/bash

python TopicMerge/merge_runner.py --model_dir=../model/news --conf=lda.conf --k=30 --Jac_opt=0 --threshold=0.1 --output_file=news_lda.merge.model
