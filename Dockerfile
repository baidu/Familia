FROM python:3.6-alpine

ADD . /familia
WORKDIR /familia/

ENV PYTHON_VERSION=python3.6m

RUN apk add make gcc g++ && \
  sh build.sh && \
  rm -f *_demo.sh *_demo && \
  (cd model; sh download_model.sh; rm -f *.tar.gz) && \
  (cd python; rm -rf demo *_demo.sh) && \
  pip install -r python/requirements.txt

ENV LD_LIBRARY_PATH=/familia/third_party/lib:$LD_LIBRARY_PATH

EXPOSE 5000

CMD ["python", "python/app.py"]
