// Copyright (c) 2017, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FAMILIA_INFERENCE_ENGINE_H
#define FAMILIA_INFERENCE_ENGINE_H

#include <memory>

#include "familia/util.h"
#include "familia/config.pb.h"
#include "familia/vocab.h"
#include "familia/model.h"
#include "familia/sampler.h"
#include "familia/document.h"

namespace familia {

// 采样器类型
enum class SamplerType {
    GibbsSampling = 0,
    MetropolisHastings = 1
};

// Inference Engine 支持LDA 和Sentence-LDA两种模型的主题推断, 两种模型使用相同的存储格式
// 同时包含吉布斯采样和Metroplis-Hastings两种采样算法
class InferenceEngine {
public:
    ~InferenceEngine() = default;

    // 默认使用 Metroplis-Hastings 采样算法
    InferenceEngine(const std::string& model_dir, 
                    const std::string& conf_file, 
                    SamplerType type = SamplerType::MetropolisHastings);
    
    // 对input的输入进行LDA主题推断，输出结果存放在doc中
    // 其中input是分词后字符串的集合
    int infer(const std::vector<std::string>& input, LDADoc& doc);

    // 对input的输入进行SentenceLDA主题推断，输出结果存放在doc中
    // 其中input是句子的集合
    int infer(const std::vector<std::vector<std::string>>& input, SLDADoc& doc);
    
    // REQUIRE: 总轮数需要大于burn-in迭代轮数, 其中总轮数越大，得到的文档主题分布越平滑
    void lda_infer(LDADoc& doc, int burn_in_iter, int total_iter) const;
    
    // REQUIRE: 总轮数需要大于burn-in迭代轮数, 其中总轮数越大，得到的文档主题分布越平滑
    void slda_infer(SLDADoc& doc, int burn_in_iter, int total_iter) const;

    // 返回模型指针以便获取模型参数
    inline std::shared_ptr<TopicModel> get_model() {
        return _model;
    }

    // 返回模型类型, 指明为LDA还是SetennceLDA
    ModelType model_type() {
        return _model->type();
    }

private:
    // 模型结构指针
    std::shared_ptr<TopicModel> _model;
    // 采样器指针, 作用域仅在InferenceEngine
    std::unique_ptr<Sampler> _sampler;
};
} // namespace familia
#endif  // FAMILIA_INFERENCE_ENGINE_H
