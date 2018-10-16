CC=g++
CFLAGS=-fPIC -shared -Wall -Iinclude -isystem /usr/include/vulkan
LDFLAGS=-lvulkan -lglfw
NAME=librend.so

INCL=$(wildcard include/*.h)
SRCS=$(wildcard src/*.cpp)
OBJS=$(SRCS:.cpp=.o)
DEPS=$(SRCS:.cpp=.d)

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
