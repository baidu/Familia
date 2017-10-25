# Topic Redundancy Reduction and Model Compression 

## Topic Redundancy Reduction
The topics given by the topic model contain much redundancy sometimes, and we offer two metrics to reduce topic redundancy. What's more, the tool supports merging similar topics. 

    $ sh run_topic_model_merge.sh

Details can be found under [Topic Redundancy Reduction Tools][1]. 

## Fast Topic Redundancy Reduction
For large datasets, it can be time-consuming to calculate the similarity between each two topics. We design a fast topic redundancy reduction algorithm based on simhash: it finds out all the similar topics, and then merges the similar topics on each topic set. 

    $ sh run_topic_model_fastmerge.sh

Details (such as the parameters) can be found under [Fast Topic Redundancy Reduction Tools][2]. 

## Topic Model Compression
The memory available is usually limited in some industry scenarios. We provide tools to compress the topic model, which includes: topic dimensionality compression and sampling dimensionality compression. 

    $ sh run_topic_model_compress.sh

Details can be found under [Topic Model Compression Tools][3]. 

[1]:    https://github.com/baidu/Familia/wiki/%E4%B8%BB%E9%A2%98%E6%A8%A1%E5%9E%8B%E5%8E%BB%E9%87%8D%E5%B7%A5%E5%85%B7
[2]:    https://github.com/baidu/Familia/wiki/%E4%B8%BB%E9%A2%98%E6%A8%A1%E5%9E%8B%E5%BF%AB%E9%80%9F%E5%8E%BB%E9%87%8D%E5%B7%A5%E5%85%B7
[3]:    https://github.com/baidu/Familia/wiki/%E4%B8%BB%E9%A2%98%E6%A8%A1%E5%9E%8B%E5%8E%8B%E7%BC%A9%E5%B7%A5%E5%85%B7
