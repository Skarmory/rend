CC=g++
CFLAGS=-std=c++2a -fno-exceptions -fPIC -shared -Wall -Wextra -Wpedantic -Iinclude -isystem /usr/include/vulkan -DGLFW_WINDOW
LDFLAGS=-lvulkan -lglfw -DGLFW_WINDOW
NAME=librend.so

INCL=$(wildcard include/*.h)
INCL_LIGHT=$(wildcard include/lights/*.h)

SRCS=$(wildcard src/*.cpp)
SRCS_LIGHT=$(wildcard src/lights/*.cpp)

OBJS=$(SRCS:.cpp=.o)
OBJS+=$(SRCS_LIGHT:.cpp=.o)

DEPS=$(SRCS:.cpp=.d)
DEPS+=$(SRCS_LIGHT:.cpp=.d)

.PHONY: clean fullclean

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

%.o: %.cpp
	$(CC) -MMD $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

fullclean: clean
	@rm -f $(NAME)

-include $(DEPS)
