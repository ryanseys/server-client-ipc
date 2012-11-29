CC=gcc
OUT1=server
OUT2=client
CFLAGS=`pkg-config --cflags --libs glib-2.0`
all:
	@echo "Compiling $(OUT1).c.."
	@$(CC) -o $(OUT1) $(OUT1).c
	@echo "Compiled $(OUT1).c successfully!"
	@echo "Running...\n"
	# @./$(OUT1)
run:
	@echo "Running...\n"
	@./$(OUT1)
ryan:
	@echo "Compiling $(OUT1).c.."
	@$(CC) -o $(OUT1) $(OUT1).c $(CFLAGS)
	@echo "Compiled $(OUT1).c successfully!"
	@$(CC) -o $(OUT2) $(OUT2).c $(CFLAGS)
	@echo "Compiled $(OUT2).c successfully!"
	@echo "Running...\n"
	@./$(OUT1)
