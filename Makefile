#

CXX			= clang++ -std=c++11

LINK_FLAGS	+= -framework OpenCL

test: test.cpp
	$(CXX) $(LINK_FLAGS) $< -o $@
