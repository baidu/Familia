ifdef config
include $(config)
endif

ifndef DEPS_PATH
DEPS_PATH = $(shell pwd)/third_party
endif

ifndef PROTOC
PROTOC = ${DEPS_PATH}/bin/protoc
endif

ifndef CXX
	CXX = g++
endif
CXXFLAGS=-pipe \
  		 -W \
  		 -Wall \
  		 -fPIC \
  		 -std=c++11 \
  		 -fno-omit-frame-pointer \
  		 -fpermissive \
  		 -O3 \
  		 -ffast-math

INCPATH=-I./include/ \
		-I./include/familia \
  		-I./third_party/include

LDFLAGS_SO = -L$(DEPS_PATH)/lib -L./build/ -lfamilia -lprotobuf -lglog -lgflags

.PHONY: all
all: familia
	@echo $(SOURCES)
	@echo $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCPATH) build/demo/inference_demo.o  $(LDFLAGS_SO) -o inference_demo
	$(CXX) $(CXXFLAGS) $(INCPATH) build/demo/semantic_matching_demo.o $(LDFLAGS_SO)  -o semantic_matching_demo
	$(CXX) $(CXXFLAGS) $(INCPATH) build/demo/word_distance_demo.o $(LDFLAGS_SO) -o word_distance_demo
	$(CXX) $(CXXFLAGS) $(INCPATH) build/demo/topic_word_demo.o $(LDFLAGS_SO) -o topic_word_demo
	$(CXX) $(CXXFLAGS) $(INCPATH) build/demo/show_topic_demo.o $(LDFLAGS_SO) -o show_topic_demo

include depends.mk

.PHONY: clean
clean:
	rm -rf inference_demo
	rm -rf semantic_matching_demo
	rm -rf word_distance_demo
	rm -rf topic_word_demo
	rm -rf show_topic_demo
	rm -rf build 
	find src -name "*.pb.[ch]*" -delete

# third party dependency
deps: ${GLOGS} ${GFLAGS} ${PROTOBUF}
	@echo "dependency installed!"

familia: build/libfamilia.a

OBJS = $(addprefix build/, vose_alias.o inference_engine.o model.o vocab.o document.o sampler.o config.o util.o semantic_matching.o tokenizer.o \
		                   demo/inference_demo.o \
						   demo/semantic_matching_demo.o \
						   demo/word_distance_demo.o \
						   demo/topic_word_demo.o \
						   demo/show_topic_demo.o)

build/libfamilia.a: include/config.pb.h $(OBJS)
	@echo Target $@;
	ar crv $@ $(filter %.o, $?)

build/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(INCPATH) $(CXXFLAGS) -MM -MT build/$*.o $< >build/$*.d
	$(CXX) $(INCPATH) $(CXXFLAGS) -c $< -o $@ 

# build proto
include/config.pb.h src/config.cpp : proto/config.proto 
	$(PROTOC) --cpp_out=./src --proto_path=./proto $<
	mv src/config.pb.h ./include/familia
	mv src/config.pb.cc ./src/config.cpp
