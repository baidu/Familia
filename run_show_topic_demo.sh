#!/bin/bash
export LD_LIBRARY_PATH=./third_party/lib:$LD_LIBRARY_PATH

cd model
sh download_model.sh
cd ..

./show_topic_demo --vocabulary_path="./model/news/vocab_info.txt" --item_topic_table_path="./model/news/news_lda.model" --top_k=10 --num_topics=2000
