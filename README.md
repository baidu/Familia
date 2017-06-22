# Familia

[![Build Status](https://travis-ci.org/baidu/Familia.svg?branch=master)](http://travis-ci.org/baidu/Familia)
[![License](https://img.shields.io/pypi/l/Django.svg)]()

**Familia** 开源项目包含文档主题推断工具、语义匹配计算工具以及基于工业级语料训练的三种主题模型：Latent Dirichlet Allocation(LDA)、SentenceLDA 和Topical Word Embedding(TWE)。 支持用户以“拿来即用”的方式进行文本分类、文本聚类、个性化推荐等多种场景的调研和应用。考虑到主题模型训练成本较高以及开源主题模型资源有限的现状，我们会陆续开放基于工业级语料训练的多个垂直领域的主题模型，以及这些模型在工业界的典型应用方式，助力主题模型技术的科研和落地。

# 应用介绍
    
**Familia**目前包含的三种主题模型对应的论文介绍可以参考[相关论文](https://github.com/baidu/Familia/wiki/%E5%8F%82%E8%80%83%E6%96%87%E7%8C%AE)。

主题模型在工业界的应用范式可以抽象为两大类: 语义表示和语义匹配。

- **语义表示** 
    
    对文档进行主题降维，以获得文档的语义表示，这些表示可以应用于文本分类、文本聚类、CTR预估等下游应用。

- **语义匹配**

    计算文本间的语义匹配度，在代码中我们提供了两种文本类型的相似度计算方式:

    - 短文本-长文本相似度计算，使用场景包括文档关键词抽取、计算搜索引擎查询和网页的相似度等等。
    - 长文本-长文本相似度计算，使用场景包括计算两篇文档的相似度、计算用户画像和新闻的相似度等等。

更详细的内容可以参考**Familia** [Wiki](https://github.com/baidu/Familia/wiki)

# 代码编译
第三方依赖包括`gflags-2.0`，`glogs-0.3.4`，`protobuf-2.5.0`, 同时要求编译器支持C++11, `g++ >= 4.8`, 兼容Linux和Mac操作系统。
默认情况下执行以下脚本会自动获取依赖并安装。
    
    $ sh build.sh # 包含获取并安装第三方依赖的过程

# 模型下载

    $ cd model
    $ sh download_model.sh

* 关于模型的详细配置说明可以参考[模型说明](https://github.com/baidu/Familia/blob/master/model/README.md)

# DEMO
    
**Familia**自带的Demo包含以下功能：
-  **文档主题推断**
   
   对输入文档进行主题推断，以得到文档的主题降维表示。

-  **语义匹配计算**
   
   计算文件间相似度，包括短文本-长文本、长文本-长文本间的相似度计算。

-  **模型内容展现**
    
    对模型的主题词，近邻词进行展现，供用户对模型的主题有直观的理解。

具体的Demo使用说明可以参考[Demo使用说明](https://github.com/baidu/Familia/wiki/Demo%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E)

# 注意事项

* 若出现找不到libglog.so, libgflags.so等动态库错误，请添加third_party至环境变量的`LD_LIBRARY_PATH`中。

    `export LD_LIBRARY_PATH=./third_party/lib:$LD_LIBRARY_PATH`

* 代码中内置简易的FMM分词工具，只针对主题模型中出现的词表进行正向匹配。该工具仅用于Demo示例使用，若对分词和语义准确度有更高要求，建议使用商用分词工具，并使用自定义词表的功能导入主题模型中的词表。

