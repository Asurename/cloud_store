CC := gcc
CFLAGS := -Wall -Wextra -g
LDFLAGS := 
LDLIBS := -lpthread -lmysqlclient -lcjson # 根据实际需要的库添加

# 自动获取所有子目录作为头文件包含路径
DIRS := $(shell find . -type d)
INCLUDES := $(foreach dir,$(DIRS),-I$(dir))

# 递归查找所有.c文件
SRCS := $(shell find . -name "*.c")
OBJS := $(SRCS:.c=.o)

# 最终目标
TARGET := server

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# 模式规则处理所有.c文件的编译
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	find . -name "*.o" -exec rm -f {} \;
	rm -f $(TARGET)
# 在Makefile中添加
DEPFLAGS = -MT $@ -MMD -MP -MF $(@:.o=.d)

%.o: %.c
	$(CC) $(DEPFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@cp $(@:.o=.d) $(@:.o=.P); sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' -e '/^$$/d' -e 's/$$/ :/' < $(@:.o=.d) >> $(@:.o=.P); rm -f $(@:.o=.d)

-include $(OBJS:.o=.P)
