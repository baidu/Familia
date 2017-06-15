#!/bin/bash
# 下载主题模型文件

if [ ! -d news ]; then
    wget http://familia.bj.bcebos.com/models/news.tar.gz
    tar -xzf news.tar.gz
fi
