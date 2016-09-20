SRC = src
LIB = lib
BIN = bin
INC = $(addprefix -I,$(SRC))

vpath %.cpp $(SRC)

CC      = g++
CCFLAGS = -c  -Wno-deprecated -g -std=c++11

LD      = g++
LDFLAGS = -lpthread -lm -std=c++11 -pthread
LIBS     = $(LIB)                        # will get an -L for each entry


EXEC0        = CheckExistence
COMPONENTS0  = MainExistence OutputModule ApplicationSetup TcpUser CommunicationObject
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

$(BIN_EXEC0):
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

all: installdirs $(BIN_EXEC0)
