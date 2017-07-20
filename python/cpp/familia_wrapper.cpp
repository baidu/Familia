// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: lianrongzhong@baidu.com

#include <Python.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>

#include "familia/inference_engine.h"
#include "familia/tokenizer.h"
#include "familia/util.h"
#include "familia/semantic_matching.h"

using std::string;
using std::endl;
using std::cout;
using std::cin;
using std::vector;
using namespace familia;

#ifdef Py_RETURN_NONE
#define Py_RETURN_NONE return Py_INCREF(Py_None), Py_None
#endif

// 使用UNUSED宏对不使用的参数进行处理
#define UNUSED(x) (void)(x)

// 创建InferenceEngine对象
static PyObject* init_inference_engine(PyObject* self, PyObject* args) {
    UNUSED(self);
    char* model_dir = NULL;
    char* conf = NULL;
    int sampler_type = 0;
    //获取InferenceEngine初始化对应参数
    if (!PyArg_ParseTuple(args, "ssi", &model_dir, &conf, &sampler_type)) {
        LOG(ERROR) << "Failed to parse Inference Engine parameters.";
        return NULL;
    }

    InferenceEngine* engine;
    // sampler_type : 0表示使用GibbsSampling采样方法，1表示使用MetropolisHastings采样方法
    if (sampler_type == 1) {
        engine = new InferenceEngine(model_dir, conf, SamplerType::MetropolisHastings);
    }
    else {
        engine = new InferenceEngine(model_dir, conf, SamplerType::GibbsSampling);
    }
    if (engine == NULL) {
        LOG(ERROR) << "Failed to new Inference Engine.";
        return NULL;
    }
    return PyLong_FromUnsignedLong((unsigned long)engine);
}

// 销毁InferenceEngine对象
static PyObject* destroy_inference_engine(PyObject* self, PyObject* args) {
    UNUSED(self);
    unsigned long infer_ptr = 0;
    //获取inference engine对象指针
    if (!PyArg_ParseTuple(args, "k", &infer_ptr)) {
        LOG(ERROR) << "Failed to parse InferenceEngine pointer.";
        return NULL;
    }
    //转换成InferenceEngine指针
    InferenceEngine* inference_engine = (InferenceEngine*)(infer_ptr);
    delete(inference_engine);
    Py_RETURN_NONE;
}

// 创建Tokenizer对象用来分词
static PyObject* init_tokenizer(PyObject* self, PyObject* args) {
    UNUSED(self);
    char* vocab_file = NULL;
    if (!PyArg_ParseTuple(args, "s", &vocab_file)) {
        LOG(ERROR) << "Failed to parse tokenizer parameters.";
        return NULL;
    }

    Tokenizer* tokenizer = new SimpleTokenizer(vocab_file);
    if (tokenizer == NULL) {
        LOG(ERROR) << "Failed to new Tokenizer.";
        return NULL;
    }
    return PyLong_FromUnsignedLong((unsigned long)tokenizer);
}

// 销毁Tokenizer对象
static PyObject* destroy_tokenizer(PyObject* self, PyObject* args) {
    UNUSED(self);
    unsigned long tokenizer_ptr = 0;
    if (!PyArg_ParseTuple(args, "k", &tokenizer_ptr)) {
        LOG(ERROR) << "Failed to parse Tokenizer pointer.";
        return NULL;
    }
    Tokenizer* tokenizer = (Tokenizer*)(tokenizer_ptr);
    delete(tokenizer);
    Py_RETURN_NONE;
}

// 创建Topical Word Embeddings对象
static PyObject* init_twe(PyObject* self, PyObject* args) {
    UNUSED(self);
    char* model_dir = NULL;
    char* emb_file = NULL;
    if (!PyArg_ParseTuple(args, "ss", &model_dir, &emb_file)) {
        LOG(ERROR) << "Failed to parse twe parameters.";
        return NULL;
    }
    TopicalWordEmbedding* twe = new TopicalWordEmbedding(model_dir, emb_file);
    if (twe == NULL) {
        LOG(ERROR) << "Failed to new TopicalWordEmbedding.";
        return NULL;
    }
    return PyLong_FromUnsignedLong((unsigned long)twe);
}

// 销毁Topical Word Embeddings对象
static PyObject* destroy_twe(PyObject* self, PyObject* args) {
    UNUSED(self);
    unsigned long twe_ptr = 0;
    if (!PyArg_ParseTuple(args, "k", &twe_ptr)) {
        LOG(ERROR) << "Failed to parse TopicalWordEmbedding pointer.";
        return NULL;
    }
    TopicalWordEmbedding* twe = (TopicalWordEmbedding*)(twe_ptr);
    delete(twe);
    Py_RETURN_NONE;
}

// 分词
static PyObject* tokenize(PyObject* self, PyObject* args) {
    UNUSED(self);
    unsigned long tokenizer_ptr = 0;
    char* input = NULL;
    if (!PyArg_ParseTuple(args, "ks", &tokenizer_ptr, &input)) {
        LOG(ERROR) << "Failed to parse tokenize parameters.";
        return NULL;
    }
    string str = input;
    vector<string> input_vec;
    Tokenizer* tokenizer = (Tokenizer*)(tokenizer_ptr);
    // 分词结果保存在vector中
    tokenizer->tokenize(str, input_vec);
    // 转成python的list返回
    PyObject* py_list = PyList_New(0);
    if (py_list != NULL) {
        for (size_t i = 0; i < input_vec.size(); ++i) {
            PyObject* word = Py_BuildValue("s", input_vec[i].c_str());
            PyList_Append(py_list, word);
            Py_CLEAR(word);
        }
    }
    return py_list;
}

// 使用LDA模型对输入的分词文本进行infer
static PyObject* lda_infer(PyObject* self, PyObject* args) {
    UNUSED(self);
    unsigned long infer_ptr = 0;
    char* input = NULL;
    if (!PyArg_ParseTuple(args, "ks", &infer_ptr, &input)) {
        LOG(ERROR) << "Failed to parse lda_infer parameters.";
        return NULL;
    }
    string str = input;
    vector<string> input_vec;
    // 词与词之间使用空格隔开
    split(input_vec, str, ' ');

    LDADoc doc;
    InferenceEngine* inference_engine = (InferenceEngine*)(infer_ptr);
    inference_engine->infer(input_vec, doc);
    vector<Topic> topics;
    // 使用稀疏结果保存主题分布便于展现
    doc.sparse_topic_dist(topics);
    //infer后的结果封装成python的list并返回
    PyObject* py_list = PyList_New(0);
    if (py_list != NULL) {
        for (size_t i = 0; i < topics.size(); ++i) {
            PyObject* topic = Py_BuildValue("(if)", topics[i].tid, topics[i].prob);
            PyList_Append(py_list, topic);
            Py_CLEAR(topic);
        }
    }
    return py_list;
}

// 使用SentenceLDA模型对输入的分词文本进行infer
static PyObject* slda_infer(PyObject* self, PyObject* args) {
    UNUSED(self);
    unsigned long infer_ptr = 0;
    char* input = NULL;
    if (!PyArg_ParseTuple(args, "ks", &infer_ptr, &input)) {
        LOG(ERROR) << "Failed to parse slda_infer parameters.";
        return NULL;
    }
    string str = input;
    vector<string> input_vec;
    // sentenceLDA 需要保存句子结构
    vector<vector<string> > sentences;
    // 句子与句子之间用制表符隔开
    split(input_vec, str, '\t');
    for (size_t i = 0; i < input_vec.size(); ++i) {
        vector<string> sent;
        // 句子中词与词之间用空格隔开
        split(sent, input_vec[i], ' ');
        sentences.push_back(sent);
    }

    SLDADoc doc;
    InferenceEngine* inference_engine = (InferenceEngine*)(infer_ptr);
    inference_engine->infer(sentences, doc);
    vector<Topic> topics;
    doc.sparse_topic_dist(topics);
    //infer后的结果封装成python的list并返回
    PyObject* py_list = PyList_New(0);
    if (py_list != NULL) {
        for (size_t i = 0; i < topics.size(); ++i) {
            PyObject* topic = Py_BuildValue("(if)", topics[i].tid, topics[i].prob);
            PyList_Append(py_list, topic);
            Py_CLEAR(topic);
        }
    }
    return py_list;
}

// 计算长文本与长文本之间的距离
static PyObject* cal_doc_distance(PyObject* self, PyObject* args) {
    UNUSED(self);
    unsigned long infer_ptr = 0;
    unsigned long tokenizer_ptr = 0;
    char* doc_text1 = NULL;
    char* doc_text2 = NULL;
    if (!PyArg_ParseTuple(args, "kkss", &infer_ptr, &tokenizer_ptr, &doc_text1, &doc_text2)) {
        LOG(ERROR) << "Failed to parse cal_doc_distance parameters.";
        return NULL;
    }

    InferenceEngine* inference_engine = (InferenceEngine*)(infer_ptr);
    Tokenizer* tokenizer = (Tokenizer*)(tokenizer_ptr);
    vector<string> doc1_tokens;
    vector<string> doc2_tokens;
    tokenizer->tokenize(doc_text1, doc1_tokens);
    tokenizer->tokenize(doc_text2, doc2_tokens);

    // 文档主题推断, 输入分词结果，主题推断结果存放于LDADoc中
    LDADoc doc1, doc2;
    inference_engine->infer(doc1_tokens, doc1);
    inference_engine->infer(doc2_tokens, doc2);

    // 计算jsd需要传入稠密型分布
    // 获取稠密的文档主题分布
    vector<float> dense_dist1;
    vector<float> dense_dist2;
    doc1.dense_topic_dist(dense_dist1);
    doc2.dense_topic_dist(dense_dist2);

    // 计算分布之间的距离, 值越小则表示文档语义相似度越高
    float jsd = SemanticMatching::jensen_shannon_divergence(dense_dist1, dense_dist2);
    float hd = SemanticMatching::hellinger_distance(dense_dist1, dense_dist2);

    //将jsd,hd封装成py_list返回
    PyObject* py_list = PyList_New(0);
    if (py_list != NULL) {
        PyObject* jsd_value = Py_BuildValue("f", jsd);
        PyObject* hd_value = Py_BuildValue("f", hd);
        PyList_Append(py_list, jsd_value);
        PyList_Append(py_list, hd_value);
        Py_CLEAR(jsd_value);
        Py_CLEAR(hd_value);
    }
    return py_list;
}

// 计算短文本与长文本之间的相似度
static PyObject* cal_query_doc_similarity(PyObject* self, PyObject* args) {
    UNUSED(self);
    unsigned long infer_ptr = 0;
    unsigned long tokenizer_ptr = 0;
    unsigned long twe_ptr = 0;
    char* query = NULL;
    char* document = NULL;
    if (!PyArg_ParseTuple(args, "kkkss", &infer_ptr, &tokenizer_ptr, &twe_ptr,
                &query, &document)) {
        LOG(ERROR) << "Failed to parse cal_query_doc_similarity parameters.";
        return NULL;
    }

    InferenceEngine* inference_engine = (InferenceEngine*)(infer_ptr);
    Tokenizer* tokenizer = (Tokenizer*)(tokenizer_ptr);
    TopicalWordEmbedding* twe = (TopicalWordEmbedding*)(twe_ptr);
    vector<string> q_tokens;
    vector<string> doc_tokens;
    tokenizer->tokenize(query, q_tokens);
    tokenizer->tokenize(document, doc_tokens);

    // 对长文本进行主题推断，获取主题分布
    LDADoc doc;
    inference_engine->infer(doc_tokens, doc);
    vector<Topic> doc_topic_dist;
    doc.sparse_topic_dist(doc_topic_dist);

    // 计算在LDA跟TWE模型上的相关性
    float lda_sim = SemanticMatching::likelihood_based_similarity(q_tokens,
                                                                  doc_topic_dist,
                                                                  inference_engine->get_model());
    float twe_sim = SemanticMatching::twe_based_similarity(q_tokens, doc_topic_dist, *twe);

    // 将LDA跟TWE的结果封装成py_list返回
    PyObject* py_list = PyList_New(0);
    if (py_list != NULL) {
        PyObject* lda_sim_value = Py_BuildValue("f", lda_sim);
        PyObject* twe_sim_value = Py_BuildValue("f", twe_sim);
        PyList_Append(py_list, lda_sim_value);
        PyList_Append(py_list, twe_sim_value);
        Py_CLEAR(lda_sim_value);
        Py_CLEAR(twe_sim_value);
    }
    return py_list;
}

// 返回与目标词最相关的K个词
static PyObject* nearest_words(PyObject* self, PyObject* args) {
    UNUSED(self);
    unsigned long twe_ptr = 0;
    char* word = NULL;
    int k = 0;
    if (!PyArg_ParseTuple(args, "ksi", &twe_ptr, &word, &k)) {
        LOG(ERROR) << "Failed to parse find_nearest_words parameters.";
        return NULL;
    }

    // 检查词典是否包含目标词
    TopicalWordEmbedding* twe = (TopicalWordEmbedding*)(twe_ptr);
    if (!twe->contains_word(word)) {
        LOG(INFO) << word << " is out of vocabulary.";
        Py_RETURN_NONE;
    }

    // 查询最邻近的词
    vector<WordAndDis> items(k);
    twe->nearest_words(word, items);

    // 将结果封装成list返回
    PyObject* py_list = PyList_New(0);
    if (py_list != NULL) {
        for (size_t i = 0; i < items.size(); ++i) {
            PyObject* item = Py_BuildValue("(sf)", items[i].word.c_str(), items[i].distance);
            PyList_Append(py_list, item);
            Py_CLEAR(item);
        }
    }
    return py_list;
}

// 返回对应主题下最邻近的词
static PyObject* nearest_words_around_topic(PyObject* self, PyObject* args) {
    UNUSED(self);
    unsigned long twe_ptr = 0;
    int topic_id = 0;
    int k = 0;
    if (!PyArg_ParseTuple(args, "kii", &twe_ptr, &topic_id, &k)) {
        LOG(ERROR) << "Failed to parse nearest_words_around_topic parameters.";
        return NULL;
    }

    // 判断主题ID是否在合法范围内
    TopicalWordEmbedding* twe = (TopicalWordEmbedding*)(twe_ptr);
    if (0 > topic_id || topic_id >= twe->num_topics()) {
        LOG(INFO) << "Topic_id " << topic_id << " is iilegal.";
        Py_RETURN_NONE;
    }

    // 查询该主题下最邻近的词
    vector<WordAndDis> items(k);
    twe->nearest_words_around_topic(topic_id, items);

    // 将结果封装成list返回
    PyObject* py_list = PyList_New(0);
    if (py_list != NULL) {
        for (size_t i = 0; i < items.size(); ++i) {
            PyObject* item = Py_BuildValue("(sf)", items[i].word.c_str(), items[i].distance);
            PyList_Append(py_list, item);
            Py_CLEAR(item);
        }
    }
    return py_list;
}

// 定义各个函数
static PyMethodDef Methods[] = {
    {"init_inference_engine", (PyCFunction)init_inference_engine, METH_VARARGS, "init_inference_engine"},
    {"destroy_inference_engine", (PyCFunction)destroy_inference_engine, METH_VARARGS, "destroy_inference_engine"},
    {"init_tokenizer", (PyCFunction)init_tokenizer, METH_VARARGS, "init_tokenizer"},
    {"destroy_tokenizer", (PyCFunction)destroy_tokenizer, METH_VARARGS, "destroy_tokenizer"},
    {"init_twe", (PyCFunction)init_twe, METH_VARARGS, "init_twe"},
    {"destroy_twe", (PyCFunction)destroy_twe, METH_VARARGS, "destroy_twe"},
    {"tokenize", (PyCFunction)tokenize, METH_VARARGS, "tokenize"},
    {"lda_infer", (PyCFunction)lda_infer, METH_VARARGS, "lda_infer"},
    {"slda_infer", (PyCFunction)slda_infer, METH_VARARGS, "slda_infer"},
    {"cal_doc_distance", (PyCFunction)cal_doc_distance, METH_VARARGS, "cal_doc_distance"},
    {"cal_query_doc_similarity", (PyCFunction)cal_query_doc_similarity,
        METH_VARARGS, "cal_query_doc_similarity"},
    {"nearest_words", (PyCFunction)nearest_words, METH_VARARGS, "nearest_words"},
    {"nearest_words_around_topic", (PyCFunction)nearest_words_around_topic,
        METH_VARARGS, "nearest_words_around_topic"},
    {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "familia",
    NULL,
    -1,
    Methods
};

PyMODINIT_FUNC PyInit_familia(void) {
    return PyModule_Create(&moduledef);
}
#else

// 模块初始化
PyMODINIT_FUNC initfamilia(void)
{
    Py_InitModule("familia", Methods);
}
#endif
/* vim: set ts=4 sw=4 sts=4 tw=100 */
