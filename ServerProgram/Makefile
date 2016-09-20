SRC = src
LIB = lib
BIN = bin
INC = $(addprefix -I,$(SRC))

vpath %.cpp $(SRC)

CC      = g++
CCFLAGS = -c  -Wno-deprecated -g -std=c++11

LD      = g++
LDFLAGS = -lpthread -lm -std=c++11 -pthread

LIBS     = $(LIB)   # will get an -L for each entry


EXEC0        = CollSoft
COMPONENTS0  = Main Input serialib BasicModbusLibrary OutputModule ApplicationSetup LogFile TcpUser CommunicationObject DrvList Utils FilePar DriverFunction CommandExecutor Linear Encoder
BIN_EXEC0    = $(addprefix $(BIN)/,$(EXEC0) )


default: all

# compile sources
$(LIB)/%.o: %.cpp
	@echo .
	@echo ... compiling source: $< to $@
	$(CC) $(CCFLAGS) $< $(ROOTINCS) -o $@

$(LIB)/%.o: %.c
	@echo .
	@echo ... compiling source: $< to $@
	$(CC) $(CCFLAGS) $< $(ROOTINCS) -o $@

$(LIB)/%.o: SourceCode/%.c
	@echo .
	@echo ... compiling source: $< to $@
	$(CC) $(CCFLAGS) $< $(ROOTINCS) -o $@

$(LIB)/%.o: SourceCode/%.cpp
	@echo .
	@echo ... compiling source: $< to $@
	$(CC) $(CCFLAGS) $< $(ROOTINCS) -o $@

$(BIN_EXEC0):
	@echo .
	@echo Compiling CollSoft program...
	$(LD) $(LDFLAGS) $^ $(ROOTLIBS) -o $@
$(BIN)/$(EXEC0): $(addprefix $(LIB)/, $(addsuffix .o, $(COMPONENTS0) ) )


.PHONY : clean
clean:
	@echo ... cleaning
	rm -f $(LIB)/*.o
	rm -f $(LIB)/*.so
	rm -f $(BIN_EXEC0)

$(LIB):
	mkdir -p $(LIB)

$(BIN):
	mkdir -p $(BIN)

installdirs: $(LIB) $(BIN)

selector: installdirs $(BIN_EXEC0)

flex:
	flex -o./SourceCode/DrvList.c -PDrv1 ./LogFile/SerialDrvLog.flex
	flex -o./SourceCode/FilePar.c -PFilePar ./LogFile/FileParLog.flex
	flex -o./SourceCode/Encoder.c -PEncoder ./LogFile/EncoderLog.flex

all: installdirs $(BIN_EXEC0)
