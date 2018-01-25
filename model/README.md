## 模型说明([**English**][1])

已开放新闻、网页和小说领域主题模型，其他领域的模型会陆续开放。

### 新闻主题模型

使用百度新闻2016全年千万级新闻数据训练, 词表规模294657, 主题数2000。
模型包括LDA、SentenceLDA(SLDA), Topical Word Embedding (TWE), 其中模型均存放于model/news目录中

    * lda.conf      # LDA模型inference所需配置文件
    * slda.conf     # Sentence-LDA模型inference所需配置文件
    * news_lda.model     # LDA模型参数, 存储格式为libSVM格式
    * news_slda.model    # Sentence-LDA模型参数, 存储格式为libSVM格式
    * news_twe_lda.model # TWE模型参数, 二进制存储, 包括词向量与主题向量
    * vocab_info.txt        # 词表文件, 中文编码采用UTF-8

### 网页主题模型

使用千万级别网页数据训练，词表规模283827，LDA模型主题数为4267，SentenceLDA(SLDA)模型主题数为5000。
模型包括LDA、SentenceLDA(SLDA), Topical Word Embedding (TWE), 其中模型均存放于model/webpage目录中

    * lda.conf      # LDA模型inference所需配置文件
    * slda.conf     # Sentence-LDA模型inference所需配置文件
    * webpage_lda.model     # LDA模型参数, 存储格式为libSVM格式
    * webpage_slda.model    # Sentence-LDA模型参数, 存储格式为libSVM格式
    * webpage_twe_lda.model # TWE模型参数, 二进制存储, 包括词向量与主题向量
    * vocab_info.txt        # 词表文件, 中文编码采用UTF-8

### 小说主题模型

使用百度万级别小说数据训练，词表规模243617，LDA模型主题数为500，SentenceLDA(SLDA)模型主题数为500。
模型包括LDA、SentenceLDA(SLDA), Topical Word Embedding (TWE), 其中模型均存放于model/novel目录中

    * lda.conf      # LDA模型inference所需配置文件
    * slda.conf     # Sentence-LDA模型inference所需配置文件
    * novel_lda.model     # LDA模型参数, 存储格式为libSVM格式
    * novel_slda.model    # Sentence-LDA模型参数, 存储格式为libSVM格式
    * novel_twe_lda.model # TWE模型参数, 二进制存储, 包括词向量与主题向量
    * vocab_info.txt        # 词表文件, 中文编码采用UTF-8
 
### 微博主题模型

使用约一亿微博数据训练，每一条实例由一条微博状态以及其一条评论组成，词表规模175347，SentenceLDA(SLDA)模型主题数为2000。
模型包括SentenceLDA(SLDA), 其中模型均存放于model/weibo目录中

    * slda.conf     # Sentence-LDA模型inference所需配置文件
    * weibo_slda.model    # Sentence-LDA模型参数, 存储格式为libSVM格式
    * vocab_info.txt        # 词表文件, 中文编码采用UTF-8
    
[1]:    https://github.com/baidu/Familia/blob/master/model/README.EN.md
