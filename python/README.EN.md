# Familia Python Interface

## Code Compilation 
To compile the code, you need install Python (support python2 and python3, Mac and Linux system). 
Run the following script in the Familia directory and it will download all the dependent packages and product fimilia.so.   

$ sh build.sh

## Python Interface 
We provide two python classes (familia_wrapper.py) based on the original C++ file: InferenceEngineWrapper and TopicalWordEmbeddingsWrapper.
InferenceEngineWrappr provides the following interfaces for topic model: 

	- lda_infer                     # LDA inference  
	- slda_infer                    # SentenceLDA inference
	- cal_doc_distance              # distance calculation between long-long documents
	- cal_query_doc_similarity      # distance calculation between short-long documents

TopicalWordEmbeddingsWrapper provides the following interfaces for TWE model：

	- nearest_words                 # find the nearest words for a given word
	- nearest_words_around_topic    # find the nearest words for a given topic 

Demos can be found under the following link (https://github.com/baidu/Familia/wiki/Python-Demo%E4%BD%BF%E7%94%A8%E6%96%87%E6%A1%A3)
