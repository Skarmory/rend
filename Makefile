CC=g++
CPPFLAGS=-std=c++2a -fno-exceptions -fPIC -shared -Wall -Wextra -Wpedantic -Iinclude -Iinclude/core -DGLFW_WINDOW
LDFLAGS=-lglfw -DGLFW_WINDOW
NAME=librend.so

SRCS=$(wildcard src/*.cpp)
SRCS+=$(wildcard src/core/*.cpp)
SRCS+=$(wildcard src/api/vulkan/*.cpp)
SRCS+=$(wildcard src/lights/*.cpp)

OBJS=$(SRCS:.cpp=.o)

DEPS=$(SRCS:.cpp=.d)

.PHONY: clean default fullclean vulkan vulkan-debug

default: $(NAME)

vulkan-debug: CPPFLAGS += -g -DDEBUG
vulkan-debug: vulkan

vulkan: CPPFLAGS += -DUSE_VULKAN -Iinclude/api/vulkan -isystem /usr/include/vulkan
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
