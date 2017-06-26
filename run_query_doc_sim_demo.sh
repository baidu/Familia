#!/bin/bash
export LD_LIBRARY_PATH=./third_party/lib:$LD_LIBRARY_PATH

cd model
sh download_model.sh
cd ..

./query_doc_sim_demo --model_dir="./model/news" --conf_file="lda.conf" --emb_file="news_twe_lda.model"
