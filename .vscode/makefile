#source file search paths
VPATH = ./src 

	# compiler include search paths
INCS = -I./include \
	-I../windex/include \
	-I../raven-set \

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \
	#-mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static -lComctl32

# folders
ODIR = ./.vscode/obj

OPT = -g
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_starter = \
	$(ODIR)/windex.o \
	$(ODIR)/Dxf.o \
	$(ODIR)/Line.o \
	$(ODIR)/PolyLine.o \
	$(ODIR)/Spline.o \
	$(ODIR)/Circle.o \
	$(ODIR)/Arc.o \
	$(ODIR)/Text.o \
	$(ODIR)/Solid.o \

### COMPILE

$(ODIR)/%.o: %.cpp  
	g++ $< \
	-c -o $@  $(INCS) $(CCflags) -Dwindexbuild

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/DXF_Viewer.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o

