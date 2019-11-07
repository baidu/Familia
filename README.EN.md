<a href="http://github.com/baidu/Familia">
	<img style="vertical-align: top;" src="https://raw.githubusercontent.com/wiki/baidu/Familia/img/logo.png?raw=true" alt="logo" height="140px">
</a>

[![Build Status][image-1]][1]
[![License][image-2]]()

**Familia** is an open-source project, which implements three popular topic models based on the large-scale industrial data. They are Latent Dirichlet Allocation(LDA)、SentenceLDA and Topical Word Embedding(TWE). In addition, **Familia** offers several tools including lda-infer and lda-query-doc-sim. **Familia** could be easily applied to many tasks, such as document classification, document clustering and personalized recommendation. Due to the high cost of model training, we will continue to release well-trained topic models based on the various types of large-scale data.  

## Introduction
The details of topic models implemented by **Familia** can be referred to [papers on topic models][3].

Generally, the applications adopting topic models are categorized into two parts: Semantic Representation and Semantic Matching.

- **Semantic Representation**

    Topic models are able to mine hidden dimensions (topics) from document collection and generate semantic representations of documents. These generated semantic representations can be used as features for document classification, document content analysis, and CTR     prediction.

- **Semantic Matching**

    We offer two methods to compute semantic similarity between documents:
    -	Semantic similarity between short-long documents, which can be applied to keyword extraction and computing query-document semantic  similarity.
    -	Semantic similarity between long-long documents, which can be applied to computing semantic similarity between user profile and news article.

More details can be referred to [**Familia Wiki**][4].

## Compilation
The required third parties include `gflags-2.0`，`glogs-0.3.4`，`protobuf-2.5.0`. The complier should support `C++11`, `g++ >= 4.8` and be compatible with linux and mac. The deps could be obtained and installed automatically by running the following script.

	$ sh build.sh

## Download
	$ cd model
	$ sh download_model.sh

More details can be referred to [Models][5].

## Demo
**Familia** demo includes the following functions:
-	**Semantic Representation**
   utilize topic models to infer the topic distribution of the input document.
   
-	**Semantic Matching**
	compute semantic similarity between short-long or long-long documents.

-	**Topic Show**
	demonstrate top words under each topic for users’ better understanding.
  
More details can be referred to [Demos][6].

## Tips
* If libglog.so, libgflags.so and other dynamic libraries could not be found, please add third\_party to the environmental parameter `LD_LIBRARY_PATH`.

	`export LD_LIBRARY_PATH=./third_party/lib:$LD_LIBRARY_PATH`

## Contact
[Github Issues][7]

{familia} at baidu.com

## Citation

The following article describes the Familia project and industrial cases powered by topic modeling. It bundles and translates the Chinese documentation of the website. We recommend citing this article as default.

Di Jiang, Yuanfeng Song, Rongzhong Lian, Siqi Bao, Jinhua Peng, Huang He and Hua Wu. 2018. [Familia: A Configurable Topic Modeling Framework for Industrial Text Engineering][8]. arXiv preprint arXiv:1808.03733.

	@article{jiang2018familia,
  	  author = {Di Jiang and Yuanfeng Song and Rongzhong Lian and Siqi Bao and Jinhua Peng and Huang He and Hua Wu},
  	  title = {{Familia: A Configurable Topic Modeling Framework for Industrial Text Engineering}},
  	  journal = {arXiv preprint arXiv:1808.03733},
  	  year = {2018}
	}

Further Reading: [Federated Topic Modeling][11]

## Copyright and License

Familia is provided under the [BSD-3-Clause License][9].

[1]:	http://travis-ci.org/baidu/Familia
[3]:	https://github.com/baidu/Familia/wiki/%E5%8F%82%E8%80%83%E6%96%87%E7%8C%AE
[4]:	https://github.com/baidu/Familia/wiki
[5]:	https://github.com/baidu/Familia/blob/master/model/README.md
[6]:	https://github.com/baidu/Familia/wiki/Demo%E4%BD%BF%E7%94%A8%E6%96%87%E6%A1%A3
[7]:	https://github.com/baidu/Familia/issues
[8]:	https://arxiv.org/abs/1808.03733v2
[9]:	LICENSE
[11]:   https://github.com/baidu/Familia/blob/master/papers/FTM.pdf

[image-1]:	https://travis-ci.org/baidu/Familia.svg?branch=master
[image-2]:	https://img.shields.io/pypi/l/Django.svg
 



