# Familia Python接口

## 代码编译
第三方依赖除了Familia C++代码所需要的库之外，还需要依赖python，默认使用当前系统python（支持python2和python3），兼容Linux和Mac操作系统。
默认情况下在Familia目录执行以下脚本会自动获取依赖并编译产生familia.so。

	$ sh build.sh # 包含获取并安装第三方依赖的过程

## Python接口
将原先C++代码封装成两个python类（familia_wrapper.py）：InferenceEngineWrapper 和 TopicalWordEmbeddingsWrapper.
其中，InferenceEngineWrappr提供了与主题模型相关的接口:

	- lda_infer                     # LDA主题模型推断 
	- slda_infer                    # SentenceLDA主题模型推断
	- cal_doc_distance              # 计算长文本与长文本之间的距离
	- cal_query_doc_similarity      # 计算短文本跟长文本之间的相关性

TopicalWordEmbeddingsWrapper则提供了与TWE模型相关的接口：

	- nearest_words                 # 寻求与目标词最相关的词语组
	- nearest_words_around_topic    # 寻求与目标主题最相关的词语组

具体使用方法可参照[使用文档](https://github.com/baidu/Familia/wiki/Python-Demo%E4%BD%BF%E7%94%A8%E6%96%87%E6%A1%A3)
