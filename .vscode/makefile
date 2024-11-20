#source file search paths
VPATH = ./src \
	./tern_in_console/src \
	./tern_in_console/models/stops \

	# compiler include search paths
INCS = \
	-I./src \
	-I./src/ext \
	-I./tern_in_console/src \
	-I./tern_in_console/models/stops \
	-I./vase/src \
	-I../windex/include \
	-I../raven-set \
	-I../../code_ext/boost/boost1_78 

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs -lComctl32 \


static_link_flags = -static-libstdc++ -static-libgcc -static

# folders
ODIR = ./.vscode/obj

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_tst1 = \
	$(ODIR)/tst1.o \
	$(ODIR)/tern.o \
	$(ODIR)/cSource.o \
	$(ODIR)/cDelay.o \
	$(ODIR)/cSink.o \
	$(ODIR)/Probability.o \

OBJ_tst2 = \
	$(ODIR)/tst2.o 

OBJ_tst3 = \
	$(ODIR)/tst3.o \
	$(ODIR)/cStoppingMachine.o \
	$(ODIR)/tern.o \
	$(ODIR)/terntime.o \
	$(ODIR)/cSource.o \
	$(ODIR)/cDelay.o \
	$(ODIR)/cSink.o \
	$(ODIR)/Probability.o \

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++  $(CCflags) \
	-c -o $@ $< $(INCS) -Dtern_console

unit: $(OBJ_tst1)
	g++ $(OPT) \
	-o bin/tst1.exe $^  \
	$(static_link_flags) $(LIBS)
tst2: $(OBJ_tst2)
	g++ $(OPT) \
	-o bin/tst2.exe $^  \
	$(static_link_flags) $(LIBS)
tst3: $(OBJ_tst3)
	g++ $(OPT) \
	-o bin/tst3.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

