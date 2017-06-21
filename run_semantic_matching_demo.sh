#!/bin/bash
export LD_LIBRARY_PATH=./third_party/lib:$LD_LIBRARY_PATH

cd model
sh download_model.sh
cd ..

# mode = 0 为计算短文本与长文本的主题语义相似度
# mode = 1 为计算长文本与长文本的主题语义相似度
./semantic_matching_demo --model_dir="./model/news" --conf_file="lda.conf" --emb_file="news_twe_lda.model" --mode=1
