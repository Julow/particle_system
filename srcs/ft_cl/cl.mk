ifeq ($(shell uname),Darwin)
  BASE_FLAGS += -DFT_CL_MAC_OS=1
  LINK_FLAGS += -framework OpenCL
else
  LINK_FLAGS += -lOpenCL
endif
