dirs = .
incs = -I . -I ..
srcs = $(foreach dir, $(dirs), $(wildcard $(dir)/*.cpp))
objs = $(patsubst %.cpp, %.o, $(srcs))

CXXFLAGS = -std=c++14
LDFLAGS = -lpthread

.PHONY: all
all = example.out

$(all): $(objs)
	$(CXX) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@ $(incs)

.PHONY: clean
clean:
	@rm $(objs)
	@rm $(all)
