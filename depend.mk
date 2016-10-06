INCLUDE_FLAGS += -I$(O_DIR)/_public
MAINS += particle_system
OBJ_DIR_TREE += $(O_DIR)/srcs/main/ $(O_DIR)/srcs/ $(O_DIR)/_public/ft/ \
	$(O_DIR)/_public/ $(O_DIR)/
O_FILES += $(O_DIR)/srcs/main/ClContextProxy.o \
	$(O_DIR)/srcs/main/GlfwWindowProxy.o $(O_DIR)/srcs/main/main.o
PUBLIC_LINKS += $(O_DIR)/_public/ft/cl.h $(O_DIR)/_public/ft/gl.h

particle_system: $(O_FILES)

# module ft::cl
ifeq ($(shell uname),Darwin)
  BASE_FLAGS += -DFT_CL_MAC_OS=1
  LINK_FLAGS += -framework OpenCL
else
  LINK_FLAGS += -lOpenCL
endif

# module ft::gl
ifeq ($(shell uname),Darwin)
  BASE_FLAGS += -DMAC_OS_MODE=1
  LINK_FLAGS += -lglfw3 -framework OpenGL
else
  LINK_FLAGS += -lglfw -lGL -lGLEW
endif

# module particle_system
$(O_DIR)/srcs/main/ClContextProxy.o: srcs/main/ClContextProxy.cpp \
	srcs/ft_cl/cl.h srcs/main/ClContextProxy.hpp srcs/main/f.hpp
$(O_DIR)/srcs/main/GlfwWindowProxy.o: srcs/main/GlfwWindowProxy.cpp \
	libft/ft_gl/gl.h srcs/main/GlfwWindowProxy.hpp
$(O_DIR)/srcs/main/main.o: srcs/main/main.cpp libft/ft_gl/gl.h srcs/ft_cl/cl.h \
	srcs/main/ClContextProxy.hpp srcs/main/ClKernel.hpp srcs/main/ClKernel.tpp \
	srcs/main/GlfwWindowProxy.hpp srcs/main/f.hpp

$(O_DIR)/srcs/main/ClContextProxy.o $(O_DIR)/srcs/main/GlfwWindowProxy.o \
$(O_DIR)/srcs/main/main.o: INCLUDE_FLAGS += -Isrcs/main

# public links
$(O_DIR)/_public/ft/cl.h: srcs/ft_cl/cl.h
$(O_DIR)/_public/ft/gl.h: libft/ft_gl/gl.h
