<a href="http://github.com/baidu/Familia">
	<img style="vertical-align: top;" src="https://raw.githubusercontent.com/wiki/baidu/Familia/img/logo.png?raw=true" alt="logo" height="140px">
</a>

[![Build Status][image-1]][1]
[![License][image-2]]()

**Familia** 开源项目包含文档主题推断工具、语义匹配计算工具以及基于工业级语料训练的三种主题模型：Latent Dirichlet Allocation(LDA)、SentenceLDA 和Topical Word Embedding(TWE)。 支持用户以“拿来即用”的方式进行文本分类、文本聚类、个性化推荐等多种场景的调研和应用。考虑到主题模型训练成本较高以及开源主题模型资源有限的现状，我们会陆续开放基于工业级语料训练的多个垂直领域的主题模型，以及这些模型在工业界的典型应用方式，助力主题模型技术的科研和落地。

## 应用介绍
**Familia**目前包含的主题模型的对应论文介绍可以参考[相关论文][3]。

主题模型在工业界的应用范式可以抽象为两大类: 语义表示和语义匹配。

- **语义表示 (Semantic Representation)**
    对文档进行主题降维，获得文档的语义表示，这些语义表示可以应用于文本分类、文本内容分析、CTR预估等下游应用。

- **语义匹配 (Semantic Matching)**

	计算文本间的语义匹配度，我们提供两种文本类型的相似度计算方式:

	- 短文本-长文本相似度计算，使用场景包括文档关键词抽取、计算搜索引擎查询和网页的相似度等等。
	- 长文本-长文本相似度计算，使用场景包括计算两篇文档的相似度、计算用户画像和新闻的相似度等等。

更详细的内容及工业界应用案例可以参考[**Familia Wiki**][4]

## 代码编译
第三方依赖包括`gflags-2.0`，`glogs-0.3.4`，`protobuf-2.5.0`, 同时要求编译器支持C++11, `g++ >= 4.8`, 兼容Linux和Mac操作系统。
默认情况下执行以下脚本会自动获取依赖并安装。

	$ sh build.sh  # 包含获取并安装第三方依赖的过程

## 模型下载

	$ cd model
	$ sh download_model.sh

* 关于模型的详细配置说明可以参考[模型说明][5]

我们会陆续开放不同领域的多种主题模型，来满足更多不同的场景需求。

## Demo
**Familia**自带的Demo包含以下功能：
-  **语义表示计算**
   利用主题模型对输入文档进行主题推断，以得到文档的主题降维表示。

-  **语义匹配计算**
   计算文本之间的相似度，包括短文本-长文本、长文本-长文本间的相似度计算。

-  **模型内容展现**
    对模型的主题词，近邻词进行展现，方便用户对模型的主题有直观的理解。

具体的Demo使用说明可以参考[使用文档][6]

## 注意事项

* 若出现找不到libglog.so, libgflags.so等动态库错误，请添加third\_party至环境变量的`LD_LIBRARY_PATH`中。

	`export LD_LIBRARY_PATH=./third_party/lib:$LD_LIBRARY_PATH`

* 代码中内置简易的FMM分词工具，只针对主题模型中出现的词表进行正向匹配。若对分词和语义准确度有更高要求，建议使用商用分词工具，并使用自定义词表的功能导入主题模型中的词表。

## 问题咨询

欢迎提交任何问题和Bug Report至[Github Issues][7].
或者发送咨询邮件至{ familia } at baidu.com

## Citation

The following article describes the Familia project and industrial cases powered by topic modeling. It bundles and translates the Chinese documentation of the website. We recommend citing this article as default.

Di Jiang, Zeyu Chen, Rongzhong Lian, Siqi Bao and Chen Li. 2017. [Familia: An Open-Source Toolkit for Industrial Topic Modeling][8]. arXiv preprint arXiv:1707.09823.

	@article{jiang2017familia,
	  author = {Di Jiang and Zeyu Chen and Rongzhong Lian and Siqi Bao and Chen Li},
	  title = {{Familia: An Open-Source Toolkit for Industrial Topic Modeling}},
	  journal = {arXiv preprint arXiv:1707.09823},
	  year = {2017}
	}

## Copyright and License

Familia is provided under the [BSD-3-Clause License][9].

[1]:	http://travis-ci.org/baidu/Familia
[3]:	https://github.com/baidu/Familia/wiki/%E5%8F%82%E8%80%83%E6%96%87%E7%8C%AE
[4]:	https://github.com/baidu/Familia/wiki
[5]:	https://github.com/baidu/Familia/blob/master/model/README.md
[6]:	https://github.com/baidu/Familia/wiki/Demo%E4%BD%BF%E7%94%A8%E6%96%87%E6%A1%A3
[7]:	https://github.com/baidu/Familia/issues
[8]:	https://arxiv.org/pdf/1707.09823.pdf
[9]:	LICENSE

[image-1]:	https://travis-ci.org/baidu/Familia.svg?branch=master
[image-2]:	https://img.shields.io/pypi/l/Django.svg
