#!/bin/bash
# 下载主题模型文件

if [ ! -d news ]; then
    wget http://familia.bj.bcebos.com/models/news.v1.tar.gz
    tar -xzf news.v1.tar.gz
fi
