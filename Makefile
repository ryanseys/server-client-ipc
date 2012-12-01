CC=gcc
OUT1=server
OUT2=client
OUT3=threads
# CFLAGS=`pkg-config --cflags --libs glib-2.0`
all:
	@echo "Compiling $(OUT1).c.."
	@$(CC) -o $(OUT1).out $(OUT1).c
	@echo "Compiled $(OUT1).c successfully!"
	@echo "Compiling $(OUT2).c.."
	@$(CC) -o $(OUT2).out $(OUT1).c
	@echo "Compiled $(OUT2).c successfully!"
run: $(OUT1).out $(OUT2).out
	@echo "Running...\n"
	@./$(OUT1)
	@./$(OUT2)
s:
	@echo "Compiling $(OUT1).c.."
	@$(CC) -o $(OUT1).out $(OUT1).c $(CFLAGS)
	@echo "Compiled $(OUT1).c successfully!"
	@echo "Running ./$(OUT1).out 42"
	@./$(OUT1).out 42
c:
	@echo "Compiling $(OUT2).c.."
	@$(CC) -o $(OUT2).out $(OUT2).c $(CFLAGS)
	@echo "Compiled $(OUT2).c successfully!"
	@echo "Running ./$(OUT2).out 42 69"
	@./$(OUT2).out 42 69
