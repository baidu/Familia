#!/bin/bash
export LD_LIBRARY_PATH=./third_party/lib:$LD_LIBRARY_PATH

cd model
sh download_model.sh
cd ..

./topic_word_demo --work_dir="./model/news" --emb_file="news_twe_lda.model" --topic_words_file="topic_words.lda.txt"
