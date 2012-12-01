CC=gcc
OUT1=server
OUT2=client
# CFLAGS=`pkg-config --cflags --libs glib-2.0`
all:
	@$(CC) -o $(OUT1).out $(OUT1).c
	@echo "Compiled $(OUT1).c to $(OUT1).out successfully!"
	@$(CC) -o $(OUT2).out $(OUT2).c
	@echo "Compiled $(OUT2).c to $(OUT2).out successfully!"
run: $(OUT1).out $(OUT2).out
	@echo "Running...\n"
	@./$(OUT1)
	@./$(OUT2)
s:
	@$(CC) -o $(OUT1).out $(OUT1).c
	@echo "Compiled $(OUT1).c to $(OUT1).out successfully!"
	@echo "Running ./$(OUT1).out 42"
	@./$(OUT1).out 42
c:
	@$(CC) -o $(OUT2).out $(OUT2).c
	@echo "Compiled $(OUT2).c to $(OUT2).out successfully!"
	@echo "Running ./$(OUT2).out 42 69"
	@./$(OUT2).out 42 69
