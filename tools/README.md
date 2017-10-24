# 主题去重与模型压缩 ([**English**][4])

## 主题去重
主题模型的训练结果往往会存在主题间的冗余。我们提供了两种指标来度量两个主题是否冗余，并合并相互冗余的多个主题。

    $ sh run_topic_model_merge.sh

更详细介绍及参数解释请参照[主题去重工具][1]

## 快速主题去重
针对大规模的主题模型，计算主题两两之间的相似度会非常耗时。我们设计了基于simhash的快速主题去重算法：首先基于simhash找出相近的主题集，在不同主题集上再分别合并冗余主题。

    $ sh run_topic_model_fastmerge.sh

更详细介绍及参数解释请参照[快速主题去重工具][2]
## 主题模型压缩
很多工业界场景对内存消耗有严格的限制，如果主题模型过大，则需要考虑对其进行压缩。我们提供两种压缩方法：主题维度压缩和采样维度压缩。

    $ sh run_topic_model_compress.sh

更详细介绍及参数解释请参照[主题模型压缩工具][3]

[1]:    https://github.com/baidu/Familia/wiki/%E4%B8%BB%E9%A2%98%E6%A8%A1%E5%9E%8B%E5%8E%BB%E9%87%8D%E5%B7%A5%E5%85%B7
[2]:    https://github.com/baidu/Familia/wiki/%E4%B8%BB%E9%A2%98%E6%A8%A1%E5%9E%8B%E5%BF%AB%E9%80%9F%E5%8E%BB%E9%87%8D%E5%B7%A5%E5%85%B7
[3]:    https://github.com/baidu/Familia/wiki/%E4%B8%BB%E9%A2%98%E6%A8%A1%E5%9E%8B%E5%8E%8B%E7%BC%A9%E5%B7%A5%E5%85%B7
[4]:    https://github.com/baidu/Familia/blob/master/tools/README.EN.md