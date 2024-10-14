# Названия программ
PARALLEL_MIN_MAX = parallel_min_max
PROCESS_MEMORY = process_memory
PARALLEL_SUM = parallel_sum

# Файлы исходного кода
PARALLEL_MIN_MAX_SRC = parallel_min_max.c find_min_max.c
PROCESS_MEMORY_SRC = process_memory.c
PARALLEL_SUM_SRC = parallel_sum.c sum.c

# Компилятор
CC = gcc

# Флаги компиляции
CFLAGS = -Wall -g

# Флаги для работы с pthreads
PTHREAD_FLAGS = -pthread

# Правила для сборки программ
all: $(PARALLEL_MIN_MAX) $(PROCESS_MEMORY) $(PARALLEL_SUM)

$(PARALLEL_MIN_MAX): $(PARALLEL_MIN_MAX_SRC)
	$(CC) $(CFLAGS) $(PARALLEL_MIN_MAX_SRC) -o $(PARALLEL_MIN_MAX)

$(PROCESS_MEMORY): $(PROCESS_MEMORY_SRC)
	$(CC) $(CFLAGS) $(PROCESS_MEMORY_SRC) -o $(PROCESS_MEMORY)

$(PARALLEL_SUM): $(PARALLEL_SUM_SRC)
	$(CC) $(CFLAGS) $(PTHREAD_FLAGS) $(PARALLEL_SUM_SRC) -o $(PARALLEL_SUM)

# Дополнительные правила
clean:
	rm -f $(PARALLEL_MIN_MAX) $(PROCESS_MEMORY) $(PARALLEL_SUM)

# Правила для раздельного запуска
run_parallel_min_max: $(PARALLEL_MIN_MAX)
	./$(PARALLEL_MIN_MAX)

run_process_memory: $(PROCESS_MEMORY)
	./$(PROCESS_MEMORY)

run_parallel_sum: $(PARALLEL_SUM)
	./$(PARALLEL_SUM) -t 4 -s 12345 -a 1000000
	
#run_parallel_sum: $(PARALLEL_SUM)
#./$(PARALLEL_SUM) $(ARGS)

.PHONY: all clean run_parallel_min_max run_process_memory run_parallel_sum
