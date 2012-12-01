CC=gcc
OUT1=server
OUT2=client
OUT3=threads
# CFLAGS=`pkg-config --cflags --libs glib-2.0`
all:
	@echo "Compiling $(OUT1).c.."
	@$(CC) -o $(OUT1) $(OUT1).c
	@echo "Compiled $(OUT1).c successfully!"
	@echo "Running...\n"
	# @./$(OUT1)
run:
	@echo "Running...\n"
	@./$(OUT1)
s:
	@echo "Compiling $(OUT1).c.."
	@$(CC) -o $(OUT1) $(OUT1).c $(CFLAGS)
	@echo "Compiled $(OUT1).c successfully!"
	@echo "Running...\n"
	@./$(OUT1) 42
c:
	@echo "Compiling $(OUT2).c.."
	@$(CC) -o $(OUT2) $(OUT2).c $(CFLAGS)
	@echo "Compiled $(OUT2).c successfully!"
	@echo "Running ./$(OUT2) 42 69..."
	@./$(OUT2) 42 69
