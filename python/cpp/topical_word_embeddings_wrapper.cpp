// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: lianrongzhong@baidu.com

#include <python2.7/Python.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "familia/semantic_matching.h"
#include "familia/util.h"

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using namespace familia;

#ifdef Py_RETURN_NONE
#define Py_RETURN_NONE return Py_INCREF(Py_None), Py_None
#endif

// 使用UNUSED宏对不使用的参数进行处理
#define UNUSED(x) (void)(x)

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

// 销毁Topical Word Embedding对象
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
    {"init_twe", (PyCFunction)init_twe, METH_VARARGS, "init_twe"},
    {"destroy_twe", (PyCFunction)destroy_twe, METH_VARARGS, "destroy_twe"},
    {"nearest_words", (PyCFunction)nearest_words, METH_VARARGS, "nearest_words"},
    {"nearest_words_around_topic", (PyCFunction)nearest_words_around_topic,
        METH_VARARGS, "nearest_words_around_topic"},
    {NULL, NULL, 0, NULL}
};

// 模块初始化化
PyMODINIT_FUNC inittopical_word_embeddings(void) {
    Py_InitModule("topical_word_embeddings", Methods);
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
