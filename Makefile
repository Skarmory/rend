CC=g++
CPPFLAGS=-std=c++2a -fno-exceptions -fPIC -shared -Wall -Wextra -Wpedantic -Iinclude -DGLFW_WINDOW
LDFLAGS=-lglfw -DGLFW_WINDOW
NAME=librend.so

INCL=$(wildcard include/*.h)
INCL_LIGHT=$(wildcard include/lights/*.h)

SRCS=$(wildcard src/*.cpp)
SRCS_LIGHT=$(wildcard src/lights/*.cpp)

OBJS=$(SRCS:.cpp=.o)
OBJS+=$(SRCS_LIGHT:.cpp=.o)

DEPS=$(SRCS:.cpp=.d)
DEPS+=$(SRCS_LIGHT:.cpp=.d)

.PHONY: clean default fullclean vulkan vulkan-debug

default: $(NAME)

vulkan-debug: CPPFLAGS += -g -DDEBUG
vulkan-debug: vulkan

vulkan: CPPFLAGS += -DUSE_VULKAN -isystem /usr/include/vulkan
vulkan: LDFLAGS += -lvulkan
vulkan: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

%.o: %.cpp
	$(CC) -MMD $(CPPFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

fullclean: clean
	@rm -f $(NAME)

-include $(DEPS)
