# --------------------------------------------------------------------
# Makefile for convex hull ipelet (an example of a C++ ipelet)
# --------------------------------------------------------------------

IPESRCDIR = ../..
OBJDIR = $(BUILDDIR)/obj/ipelets
DEPEND = $(OBJDIR)/graham.depend
include ../../common.mak

TARGET = $(call ipelet_target,graham)
LUASCRIPT = graham.lua

CPPFLAGS += -I../../include
LIBS += -L$(buildlib) -lipe
CXXFLAGS += $(DLL_CFLAGS)

all: $(TARGET)

sources	= graham.cpp

$(TARGET): $(objects)
	$(MAKE_IPELETDIR)
	$(CXX) $(LDFLAGS) $(PLUGIN_LDFLAGS) -o $@ $^ $(LIBS)

clean:
	@-rm -f $(objects) $(TARGET) $(DEPEND)

$(DEPEND): Makefile
	$(MAKE_DEPEND)

-include $(DEPEND)

install: $(TARGET)
	$(INSTALL_DIR) $(INSTALL_ROOT)$(IPELETDIR)
	$(INSTALL_PROGRAMS) $(TARGET) $(INSTALL_ROOT)$(IPELETDIR)
	$(INSTALL_FILES) $(LUASCRIPT) $(INSTALL_ROOT)$(IPELETDIR)

# --------------------------------------------------------------------
