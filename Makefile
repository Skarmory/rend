CC=g++
CPPFLAGS=-std=c++2a -fPIC -shared -Wall -Wextra -Wpedantic -DGLFW_WINDOW
CPPFLAGS+=-Iinclude
CPPFLAGS+=-isystem /usr/include/vulkan
CPPFLAGS+=-isystem /usr/include/glm
LDFLAGS=-lglfw -lvulkan -DGLFW_WINDOW
NAME=librend.so

SRCS=$(wildcard src/*.cpp)
SRCS+=$(wildcard src/core/*.cpp)
SRCS+=$(wildcard src/core/containers/*.cpp)
SRCS+=$(wildcard src/core/alloc/*.cpp)
SRCS+=$(wildcard src/api/vulkan/*.cpp)
SRCS+=$(wildcard src/lights/*.cpp)

OBJS=$(SRCS:.cpp=.o)

DEPS=$(SRCS:.cpp=.d)

.PHONY: clean default fullclean debug release

default:
	@echo "Specify a target. Options: debug, release"

debug: CPPFLAGS += -g -DDEBUG
debug: release

release: $(NAME)

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
