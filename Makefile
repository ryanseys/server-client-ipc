CC=gcc
OUT1=serverC
OUT2=clientC
OUT3=serverB
OUT4=clientB
OUT5=serverD
OUT6=clientD
# CFLAGS=`pkg-config --cflags --libs glib-2.0`
CFLAGS=-fnested-functions
all:
	@$(CC) $(CFLAGS) -o $(OUT1).out $(OUT1).c
	@echo "Compiled $(OUT1).c to $(OUT1).out successfully!"
	@$(CC) $(CFLAGS) -o $(OUT2).out $(OUT2).c
	@echo "Compiled $(OUT2).c to $(OUT2).out successfully!"
	@$(CC) $(CFLAGS) -o $(OUT3).out $(OUT3).c
	@echo "Compiled $(OUT3).c to $(OUT3).out successfully!"
	@$(CC) $(CFLAGS) -o $(OUT4).out $(OUT4).c
	@echo "Compiled $(OUT4).c to $(OUT4).out successfully!"
	@$(CC) $(CFLAGS) -o $(OUT5).out $(OUT5).c
	@echo "Compiled $(OUT5).c to $(OUT5).out successfully!"
	@$(CC) $(CFLAGS) -o $(OUT6).out $(OUT6).c
	@echo "Compiled $(OUT6).c to $(OUT6).out successfully!"
sc:
	@$(CC) $(CFLAGS) -o $(OUT1).out $(OUT1).c
	@echo "Compiled $(OUT1).c to $(OUT1).out successfully!"
	@echo "Running ./$(OUT1).out 42"
	@./$(OUT1).out 42
cc:
	@$(CC) $(CFLAGS) -o $(OUT2).out $(OUT2).c
	@echo "Compiled $(OUT2).c to $(OUT2).out successfully!"
	@echo "Running ./$(OUT2).out 42 69"
	@./$(OUT2).out 42 69
sb:
	@$(CC) $(CFLAGS) -o $(OUT3).out $(OUT3).c
	@echo "Compiled $(OUT3).c to $(OUT3).out successfully!"
	@echo "Running ./$(OUT3).out 42"
	@./$(OUT3).out 42
cb:
	@$(CC) $(CFLAGS) -o $(OUT4).out $(OUT4).c
	@echo "Compiled $(OUT4).c to $(OUT4).out successfully!"
	@echo "Running ./$(OUT4).out 42"
	@./$(OUT4).out 42
sd:
	@$(CC) $(CFLAGS) -o $(OUT5).out $(OUT5).c
	@echo "Compiled $(OUT5).c to $(OUT5).out successfully!"
	@echo "Running ./$(OUT5).out 42"
	@./$(OUT5).out 42
cd:
	@$(CC) $(CFLAGS) -o $(OUT6).out $(OUT6).c
	@echo "Compiled $(OUT6).c to $(OUT6).out successfully!"
	@echo "Running ./$(OUT6).out 42 69"
	@./$(OUT6).out 42 69
