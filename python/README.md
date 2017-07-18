**Familia Python接口**

## 代码编译
第三方依赖除了Familia C++代码所需要的库之外，还需要依赖python2.7，默认系统自带（若没有python2.7，请自行安装）。兼容Linux和Mac操作系统。
默认情况下在Familia目录执行以下脚本会自动获取依赖并编译产生inference_engine.so和topical_word_embeddings.so。

	$ sh build.sh # 包含获取并安装第三方依赖的过程

## Python接口
将原先C++代码封装成两个python类：InferenceEngineWrapper 和 TopicalWordEmbeddingsWrapper.
其中，InferenceEngineWrappr提供了与主题模型相关的接口:

	- lda_infer                     # LDA主题模型推断 
	- slda_infer                    # SentenceLDA主题模型推断
	- cal_doc_distance              # 计算长文本与长文本之间的距离
	- cal_query_doc_similarity      # 计算短文本跟长文本之间的相关性

TopicalWordEmbeddingsWrapper则提供了与TWE模型相关的接口：

	- nearest_words                 # 寻求与目标词最相关的词语组
	- nearest_words_around_topic    # 寻求与目标主题最相关的词语组

具体使用方法可参照当前文件夹中的demo示例。

## 注意事项
Familia Python接口依赖python2.7，如果系统默认python为python3，运行demo案例需要切换成python2.
