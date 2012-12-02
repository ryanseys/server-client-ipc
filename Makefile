CC=gcc
OUT1=server
OUT2=client
OUT3=serverB
OUT4=clientB
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
partB_s:
	@$(CC) -o $(OUT3).out $(OUT3).c
	@echo "Compiled $(OUT3).c to $(OUT3).out successfully!"
	@echo "Running ./$(OUT3).out 42"
	@./$(OUT3).out 42
partB_c:
	@$(CC) -o $(OUT4).out $(OUT4).c
	@echo "Compiled $(OUT4).c to $(OUT4).out successfully!"
	@echo "Running ./$(OUT4).out 42"
	@./$(OUT4).out 42
