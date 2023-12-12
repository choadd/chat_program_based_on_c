CC = gcc

OUTPUT = ./output

CFLAGS  += -Wall -Wextra
LFLAGS += -I ./src/includes -lpthread
# LFLAGS +=  

EXEC = SERVER_MAIN

EXED = ./src

EXES = $(EXED)/server_main.o \
		$(EXED)/inf/inf_svr_sock.o \

all : $(EXEC)

EXEO = $(EXES:%.c=%.o)

%.o : %.c
	$(CC) $(CFLAGS) $(DFLAGS) -c -o $@ $< $(LFLAGS)
	
$(EXEC) : $(EXEO)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(OUTPUT)/$(EXEC) $(EXEO) $(LFLAGS)	

build : $(EXEC)

clean :
	$(RM) ./$(EXED)/*.o
	$(RM) ./$(EXED)/*/*.o
	$(RM) ./$(EXED)/*/*/*.o
	$(RM) ./$(EXED)/*/*/*/*.o		
	$(RM) ./$(OUTPUT)/$(EXEC)
