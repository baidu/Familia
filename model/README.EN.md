## Model Descriptions

We have released topic models focusing in the following areas: News, Webpage and Novel, and models in other areas will be released soon. 

### News Topic Model

We use the whole dataset of Baidu news of 2016. The vocabulary table includes 294,657 words, and the topic size is 2000. The models include LDA, SentenceLDA (SLDA), Topical Word Embedding (TWE), which are stored in model/news. 

    * lda.conf      # configuration for LDA inference
    * slda.conf     # configuration for Sentence-LDA inference
    * news_lda.model     # parameters for LDA (libSVM format)
    * news_slda.model    # parameters for Sentence-LDA (libSVM format)
    * news_twe_lda.model # parameters for TWE (binary file, including word embedding and topic embedding)
    * vocab_info.txt        # vocabulary table, Chinese encoded in UTF8

### Webpage Topic Model

We use the whole dataset of Baidu webpage dataset. The vocabulary table includes 283,827 words, and the topic size is 4267 for LDA and 5000 for SentenceLDA(SLDA). The models include LDA, SentenceLDA(SLDA), Topical Word Embedding (TWE), which are stored in model/webpage. 

    * lda.conf      # configuration for LDA inference
    * slda.conf     # configuration for Sentence-LDA inference
    * webpage_lda.model     # parameters for LDA (libSVM format)
    * webpage_slda.model    # parameters for Sentence-LDA (libSVM format)
    * webpage_twe_lda.model # parameters for TWE (binary file, including word embedding and topic embedding)
    * vocab_info.txt        # vocabulary table, Chinese encoded in UTF8

### Novel Topic Model

We use the whole dataset of Baidu novel dataset. The vocabulary table includes 243,617 words, and the topic size is 500 topics for LDA and 500 for SentenceLDA(SLDA). The models include LDA, SentenceLDA(SLDA), Topical Word Embedding (TWE), which are stored in model/novel. 

    * lda.conf      # configuration for LDA inference
    * slda.conf     # configuration for Sentence-LDA inference
    * novel_lda.model     # parameters for LDA (libSVM format)
    * novel_slda.model    # parameters for Sentence-LDA (libSVM format)
    * novel_twe_lda.model # parameters for TWE (binary file, including word embedding and topic embedding)
    * vocab_info.txt        # vocabulary table, Chinese encoded in UTF8


### Weibo Topic Model

We use the weibo data set containing more than 100 million weibo status-comment pairs. The vocabulary table includes 175,347 words, and the topic size is 2000 for SentenceLDA(SLDA). The weibo SentenceLDA(SLDA) is stored in model/weibo. 

    * slda.conf     # configuration for Sentence-LDA inference
    * weibo_slda.model    # parameters for Sentence-LDA (libSVM format)
    * vocab_info.txt        # vocabulary table, Chinese encoded in UTF8
