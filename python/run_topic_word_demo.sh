#!/bin/bash
export LD_LIBRARY_PATH=../third_party/lib:$LD_LIBRARY_PATH

cd ../model
sh download_model.sh
cd ../python

python topic_word_demo.py ../model/news news_twe_lda.model
