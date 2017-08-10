# 主题模型去重与压缩

## 主题模型去重
主题模型进行的是无监督的训练，在topic数量设置过大或者过小的情况下，都会出现多个主题间严重的冗余。因此我们提供了两种衡量方式来判断两个主题是否冗余，并合并相互冗余的多个主题。

    $ sh run_topic_model_merge.sh

更详细介绍及参数解释参照[主题模型去重工具][1]

## 主题模型快速去重
针对超大模型的去重，计算主题两两之间的相似度会非常耗时，在此，我们介绍基于simhash的快速去重算法：首先基于simhash找出相近的主题集，在不同主题集上再分别合并冗余主题。

    $ sh run_topic_model_fastmerge.sh

更详细介绍及参数解释参照[主题模型快速去重工具][2]
## 主题模型压缩
针对一些场景对内存空间的限制，如果主题模型过大，则需要对其进行压缩。主题模型压缩工具提供了两种压缩方法：主题维度压缩和采样维度压缩。

    $ sh run_topic_model_compress.sh

更详细介绍及参数介绍参照[主题模型压缩工具][3]

[1]:    https://github.com/baidu/Familia/wiki/%E4%B8%BB%E9%A2%98%E6%A8%A1%E5%9E%8B%E5%8E%BB%E9%87%8D%E5%B7%A5%E5%85%B7
[2]:    https://github.com/baidu/Familia/wiki/%E4%B8%BB%E9%A2%98%E6%A8%A1%E5%9E%8B%E5%BF%AB%E9%80%9F%E5%8E%BB%E9%87%8D%E5%B7%A5%E5%85%B7
[3]:    https://github.com/baidu/Familia/wiki/%E4%B8%BB%E9%A2%98%E6%A8%A1%E5%9E%8B%E5%8E%8B%E7%BC%A9%E5%B7%A5%E5%85%B7
