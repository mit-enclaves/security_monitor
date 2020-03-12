API_DIR:=../../api
# Assumes: $(API_DIR)

CC=riscv64-linux-gnu-gcc

LINUX_APPS_INCLUDES := \
	$(API_DIR) \


TARGET=bin/test_ecall

all: $(TARGET)

bin/%: src/%.c
	$(CC) -static $(addprefix -I , $(LINUX_APPS_INCLUDES)) $^ -o $@
