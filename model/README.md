# 模型说明

# 新闻主题模型

使用百度新闻2016全年千万级新闻数据训练, 词表数300000, 主题数2000。
模型包括LDA、SentenceLDA(SLDA), Topical Word Embedding (TWE), 其中模型均存放于model/news目录中

    * lda.conf      # LDA模型inference所需配置文件
    * slda.conf     # Sentence-LDA模型inference所需配置文件
    * news_lda.model     # LDA模型参数
    * news_slda.model    # Sentence-LDA模型参数
    * news_twe_lda.model # TWE模型参数, 包括词向量与主题向量
    * vocab_info.txt        # 词表文件，中文编码采用UTF-8
