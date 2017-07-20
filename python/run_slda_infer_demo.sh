#!/bin/bash
export LD_LIBRARY_PATH=../third_party/lib:$LD_LIBRARY_PATH

cd ../model
sh download_model.sh
cd ../python

python ./demo/slda_infer_demo.py ../model/news slda.conf
