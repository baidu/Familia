# -*- coding: utf-8 -*-
import multiprocessing
import os
import re
import traceback
from collections import defaultdict

from sanic import Sanic
from sanic.exceptions import NotFound
from sanic.log import logger
from sanic.response import json
from sanic_openapi import swagger_blueprint, doc

from familia_wrapper import InferenceEngineWrapper, TopicalWordEmbeddingsWrapper

app = Sanic("Familia", strict_slashes=True)
app.blueprint(swagger_blueprint)
app.config.API_TITLE = 'Familia API'
app.config.API_DESCRIPTION = 'A Toolkit for Industrial Topic Modeling'
app.config.API_PRODUCES_CONTENT_TYPES = ['application/json']

RE_BACKSPACES = re.compile("\b+")

model_name = os.environ.get("MODEL_NAME", 'news').lower()
n_workers = int(os.environ.get('WORKERS', multiprocessing.cpu_count()))

model_dir = f"/familia/model/{model_name}"
emb_file = f"{model_name}_twe_lda.model"

inference_engine_lda = InferenceEngineWrapper(model_dir, 'lda.conf', emb_file)
inference_engine_slda = InferenceEngineWrapper(model_dir, 'slda.conf')
twe = TopicalWordEmbeddingsWrapper(model_dir, emb_file)


def read_topic_words_from_file(topic_words_file_name='topic_words.lda.txt'):
    logger.info(f"reading topic_words from file: {topic_words_file_name}")
    topic_words = defaultdict(list)
    file_path = os.path.join(model_dir, topic_words_file_name)
    if not os.path.exists(file_path):
        logger.warn(f"topic_words file not found: {file_path}")
        return topic_words
    with open(file_path, 'r') as f:
        line = f.readline()
        while line:
            pos = line.find('=')
            line = line[pos + 2:]
            topic_id, num = line.strip().split('\t')
            topic_id, num = int(topic_id), int(num)
            f.readline()
            items = list()
            for i in range(num):
                data = f.readline()
                word, score = data.strip().split('\t')
                items.append([word, float(score)])
            topic_words[topic_id] = items
            line = f.readline()
    return topic_words


lda_topic_words = read_topic_words_from_file()


def get_param(request, param_name, default_value=None, is_list=False):
    param_value = (request.form.getlist(param_name) if is_list else request.form.getlist(param_name)) or \
                  request.args.get(param_name) or \
                  default_value
    if param_value is None:
        return param_value
    value_type = type(param_value)
    if is_list:
        return param_value if value_type == list else [param_value]
    return param_value[0] if value_type == list else param_value


def strip_to_none(text: str):
    if text is None:
        return None
    text = text.strip()
    text = re.sub(RE_BACKSPACES, '', text)
    if len(text) == 0:
        return None
    if text == 'None':
        return None
    return text


def response(success: bool = True, data=None, message=None):
    data = {'success': success, 'message': message, 'data': data}
    data = {k: v for k, v in data.items() if v is not None}
    try:
        return json(data, ensure_ascii=False)
    except Exception as err:
        logger.error(err, exc_info=True)
        msg = traceback.format_exc()
        data = {'success': success, 'message': msg}
        return json(data, ensure_ascii=False)


def error_response(message='Invalid request'):
    return response(success=False, message=message)


def handle_404(request, exception):
    return api_index(request)


def handle_exception(request, exception):
    return error_response(str(exception))


@app.route('/')
@doc.description("ping")
def api_index(request):
    message = f"Familia API is running, check out the api doc at http://{request.host}/swagger/"
    return response(message=message)


@app.exception(NotFound)
async def ignore_404s(request, exception):
    message = f"Yep, I totally found the page: {request.url}"
    return response(message=message)


@app.route('/tokenize', methods=["POST"])
@doc.summary("分词")
@doc.description("简易的FMM分词工具，只针对主题模型中出现的词表进行正向匹配")
@doc.consumes(doc.String(name='text', description="文本"), required=True)
@doc.response(200, None, description="""返回一个list对象，其中每个元素为分词后的结果。""")
async def api_tokenize(request):
    try:
        text = get_param(request, 'text')
        if text is None:
            return error_response()
        result = inference_engine_lda.tokenize(text)
        return response(data=result)
    except Exception as err:
        logger.error(err, exc_info=True)
        return error_response(str(err))


@app.route('/lda', methods=["POST"])
@doc.summary("LDA模型推断")
@doc.description("使用LDA模型对输入文本进行推断，得到其主题分布")
@doc.consumes(doc.Integer(name='n', description="top n，默认10"), required=False)
@doc.consumes(doc.String(name='text', description="文本"), required=True)
@doc.response(200, None, description="""返回一个list对象，存放输入文本对应的稀疏主题分布
list中每个元素为tuple
每个tuple包含一个主题ID以及该主题对应的概率，按照概率从大到小排序。
例如：[(15, 0.5), (10, 0.25), (1999, 0.25)]""")
async def api_lda(request):
    try:
        text = get_param(request, 'text')
        n = int(get_param(request, 'n', 10))
        if text is None:
            return error_response()
        words = inference_engine_lda.tokenize(text)
        result = inference_engine_lda.lda_infer(words)
        result = result[:n]
        result = [
            {
                'topic_id': topic_id,
                'score': score,
                'topic_words': twe.nearest_words_around_topic(topic_id),
                'topic_words_poly': lda_topic_words.get(topic_id),
            } for topic_id, score in result
        ]
        return response(data=result)
    except Exception as err:
        traceback.print_exc()
        logger.error(err, exc_info=True)
        return error_response(str(err))


@app.route('/slda', methods=["POST"])
@doc.summary("SentenceLDA模型推断")
@doc.description("使用SentenceLDA模型对输入文本进行推断，得到其主题分布")
@doc.consumes(doc.Integer(name='n', description="top n，默认10"), required=False)
@doc.consumes(doc.String(name='sep', description="多段文本之间的分割符，默认\\n分割"), required=False)
@doc.consumes(doc.String(name='text', description="多段文本"), required=True)
@doc.response(200, None, description="""返回一个list对象，存放输入文本对应的稀疏主题分布
list中每个元素为tuple
每个tuple包含一个主题ID以及该主题对应的概率，按照概率从大到小排序。
例如：[(15, 0.5), (10, 0.25), (1999, 0.25)]""")
async def api_slda(request):
    try:
        text = str(get_param(request, 'text'))
        sep = get_param(request, 'sep')
        n = int(get_param(request, 'n', 10))
        if text is None:
            return error_response('Invalid request')
        sentences = text.splitlines() if sep is None else text.split(sep=sep)
        sentences = map(inference_engine_slda.tokenize, sentences)
        result = inference_engine_slda.slda_infer(sentences)
        result = result[:n]
        result = [
            {
                'topic_id': topic_id,
                'score': score,
                'topic_words': twe.nearest_words_around_topic(topic_id),
                'topic_words_poly': lda_topic_words.get(topic_id)
            } for topic_id, score in result
        ]
        return response(data=result)
    except Exception as err:
        logger.error(err, exc_info=True)
        return error_response(str(err))


@app.route('/distance', methods=["POST"])
@doc.summary("计算长文本与长文本之间的距离")
@doc.description("计算两个长文本的主题分布之间的距离，包括jensen_shannon_divergence和hellinger_distance")
@doc.consumes(doc.String(name='b', description="文本b"), required=True)
@doc.consumes(doc.String(name='a', description="文本a"), required=True)
@doc.response(200, None, description="""返回一个list对象，其中有两个float元素
第一个表示jensen_shannon_divergence距离
第二个表示hellinger_distance距离
例如：[0.187232, 0.23431]""")
async def api_distance(request):
    try:
        a = get_param(request, 'a')
        b = get_param(request, 'b')
        if a is None or b is None:
            return error_response('Invalid request')
        words_a = inference_engine_lda.tokenize(a)
        words_b = inference_engine_lda.tokenize(b)
        result = inference_engine_lda.cal_doc_distance(words_a, words_b)
        return response(data=result)
    except Exception as err:
        logger.error(err, exc_info=True)
        return error_response(str(err))


@app.route('/similarity/keywords', methods=["POST"])
@doc.summary("关键词计算")
@doc.description("使用LDA/TWE模型计算候选关键词与文档的相关性")
@doc.consumes(doc.Boolean(name='use_twe', description="是否使用TWE模型, 默认不使用"), required=False)
@doc.consumes(doc.String(name='text', description="文本"), required=True)
@doc.consumes(doc.String(name='keywords', description="关键词列表, 空格分割"), required=True)
@doc.response(200, None, description="""返回一个list对象，每个元素为关键词以及其与文档相关性。""")
async def api_similarity_keywords(request):
    try:
        keywords = get_param(request, 'keywords')
        text = get_param(request, 'text')
        use_twe = get_param(request, 'use_twe')
        if keywords is None or len(keywords) == 0 or text is None:
            return error_response('Invalid request')
        text = ' '.join(inference_engine_lda.tokenize(text))
        if use_twe:
            result = inference_engine_lda.cal_keywords_twe_similarity(keywords, text)
        else:
            result = inference_engine_lda.cal_keywords_similarity(keywords, text)
        return response(data=result)
    except Exception as err:
        logger.error(err, exc_info=True)
        return error_response(str(err))


@app.route('/similarity/query', methods=["POST"])
@doc.summary("计算短文本与长文本之间的相关性")
@doc.description("使用LDA模型和TWE模型分别衡量短文本跟长文本之间的相关性")
@doc.consumes(doc.String(name='text', description="长文本"), required=True)
@doc.consumes(doc.String(name='query', description="短文本"), required=True)
@doc.response(200, None, description="""返回一个list对象，其中有两个float元素
第一个表示根据LDA模型得到的相关性
第二个表示通过TWE模型衡量得到的相关性
例如：[0.397232, 0.45431]""")
async def api_similarity_query(request):
    try:
        query = get_param(request, 'query')
        text = get_param(request, 'text')
        if query is None or text is None:
            return error_response('Invalid request')
        words_query = inference_engine_lda.tokenize(query)
        words_text = inference_engine_lda.tokenize(text)
        result = inference_engine_lda.cal_query_doc_similarity(words_query, words_text)
        return response(data=result)
    except Exception as err:
        logger.error(err, exc_info=True)
        return error_response(str(err))


@app.route('/nearest-words', methods=["POST"])
@doc.summary("寻求与目标主题最相关的词")
@doc.description("对模型中的所有词语进行检索，通过计算cosine相似度，返回最相关的n个词语")
@doc.consumes(doc.Integer(name='n', description="top n，默认10"), required=False)
@doc.consumes(doc.Integer(name='topic_id', description="目标主题，可以为多个（一个和多个的返回结果结构不相同）, word与topic_id二选一"), required=False)
@doc.consumes(doc.String(name='word', description="目标词，可以为多个（一个和多个的返回结果结构不相同）, word与topic二选一"), required=False)
@doc.response(200, None, description="""返回一个list对象，长度为n
list中每个元素为tuple，包含了返回词以及该词与目标词的cosine相关性，并按照相关性从高到低排序
例如输入"篮球"目标词返回前10个结果：
[(篮球队，0.833797),
 (排球， 0.833721)
 ...,
 (篮球圈, 0.752021)]
 如果输入目标词不在词典中，则返回None。""")
async def nearest_words(request):
    try:
        word = get_param(request, 'word', is_list=True)
        topic_id = get_param(request, 'topic_id', is_list=True)
        n = int(get_param(request, 'n', 10))
        if word:
            result = {w: twe.nearest_words(w, n) for w in word}
            return response(data=result)
        if topic_id:
            result = {int(_id): twe.nearest_words_around_topic(int(_id), n) for _id in topic_id}
            return response(data=result)
        return error_response()
    except Exception as err:
        logger.error(err, exc_info=True)
        return error_response(str(err))


if __name__ == '__main__':
    logger.info(f"running familia api with {n_workers} workers")
    app.run(host='0.0.0.0', port=5000, workers=n_workers)
