export LD_LIBRARY_PATH=./third_party/lib:$LD_LIBRARY_PATH

if [ -d news_t1000 ];then
    echo "model file downloaded already"
else
    rm -rf news_t1000
    mkdir news_t1000

    echo "get example model..."
    cd news_t1000
    wget ftp://nj03-rp-m22nlp062.nj03.baidu.com/home/disk0/chenzeyu01/public/infer_data/news_t1000/*
    
    cd ..
    rm -rf input.sample

    echo "get input data..."
    wget ftp://nj03-rp-m22nlp062.nj03.baidu.com/home/disk0/chenzeyu01/public/infer_data/input.sample
fi

echo "running infer program..."

# ./lda-infer model_path,  lda-infer.conf, #burn_in_iter #total_iter
# cat input.merge.sample | ./lda-infer ./news_t1000 lda_infer.conf > infer.result 
#cat example/example.txt | ./inference_demo --work_dir="./news_t1000" --conf_file="model.conf" > infer.result
cat example/input.sample | ./test --work_dir="./news_t1000" --conf_file="model.conf" > infer.result
#head input.sample | ./lda-infer ./news_t1000 lda_infer.conf > infer.result 

echo "infer result store in infer.result"
python scripts/jsd.py infer.result news_t1000/doc_topic.txt 1000
#python tools/jsd.py infer.slda.mh.result infer.slda.gs.result 1000
#python tools/jsd.py infer.result infer.slda.gs.result 1000
echo "new alias_table jsd / new random / fix_random_seed = 0.135918819452 / 0.146278813358 / 0.135296716886"
echo "original time cost 4.22496 sec"
