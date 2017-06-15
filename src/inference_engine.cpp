// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: chenzeyu01@baidu.com

#include "familia/inference_engine.h"

#include <algorithm>
#include <fstream>
#include <stdlib.h>
#include <memory>

namespace familia {

InferenceEngine::InferenceEngine(const std::string& work_dir,
                                 const std::string& conf_file,
                                 SamplerType type) {
    LOG(INFO) << "Inference Engine initializing...";
    // 读取模型配置和模型
    ModelConfig config;
    load_prototxt(work_dir + "/" + conf_file, config);
    _model = std::make_shared<TopicModel>(work_dir, config);

    // 根据配置初始化采样器
    if (type == SamplerType::GibbsSampling) {
        _sampler = std::unique_ptr<Sampler>(new GibbsSampler(_model));
    } else if (type == SamplerType::MetropolisHastings) {
        _sampler = std::unique_ptr<Sampler>(new MHSampler(_model));
    }

    LOG(INFO) << "InferenceEngine initialize successfully!";
}

int InferenceEngine::infer(const std::vector<std::string>& input, LDADoc& doc) {
    fix_random_seed(); // 固定随机数种子, 保证同样输入下推断的的主题分布稳定
    doc.init(_model->num_topics());
    for (const auto& token : input) {
        int id = _model->term_id(token);
        if (id != OOV) {
            int init_topic = rand_k(_model->num_topics());
            doc.add_token({init_topic, id});
        }
    }

    lda_infer(doc, 20, 50);

    return 0;
}

int InferenceEngine::infer(const std::vector<std::vector<std::string>>& input, SLDADoc& doc) {
    fix_random_seed(); // 固定随机数种子, 保证同样输入下推断的的主题分布稳定
    doc.init(_model->num_topics());
    std::vector<int> words;
    int init_topic;
    for (const auto& sent : input) {
        for (const auto& token : sent) {
            int id = _model->term_id(token);
            if (id != OOV) {
                words.push_back(id);
            }
        }
        // 随机初始化
        init_topic = rand_k(_model->num_topics());
        doc.add_sentence({init_topic, words});
        words.clear();
    }

    slda_infer(doc, 20, 50);

    return 0;
}

void InferenceEngine::lda_infer(LDADoc& doc, int burn_in_iter, int total_iter) const {
    CHECK_GE(burn_in_iter, 0);
    CHECK_GT(total_iter, 0);
    CHECK_GT(total_iter, burn_in_iter);

    for (int iter = 0; iter < total_iter; ++iter) {
        _sampler->sample_doc(doc);
        if (iter >= burn_in_iter) { 
            // 经过burn-in阶段后, 对每轮采样的结果进行累积，以得到更平滑的分布
            doc.accumulate_topic_sum();
        }
    }
}

void InferenceEngine::slda_infer(SLDADoc& doc, int burn_in_iter, int total_iter) const {
    CHECK_GE(burn_in_iter, 0);
    CHECK_GT(total_iter, 0);
    CHECK_GT(total_iter, burn_in_iter);

    for (int iter = 0; iter < total_iter; ++iter) {
        _sampler->sample_doc(doc);
        if (iter >= burn_in_iter) {
            // 经过burn-in阶段后，对每轮采样的结果进行累积，以得到更平滑的分布
            doc.accumulate_topic_sum();
        }
    }
}
} // namespace familia
