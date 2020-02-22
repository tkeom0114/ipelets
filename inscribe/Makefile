# --------------------------------------------------------------------
# Template Makefile for ipelet (an example of a C++ ipelet)
# --------------------------------------------------------------------

IPESRCDIR = ../..
OBJDIR = $(BUILDDIR)/obj/ipelets
DEPEND = $(OBJDIR)/inscribe.depend
include ../../common.mak

TARGET = $(call ipelet_target,inscribe)
LUASCRIPT = inscribe.lua

CPPFLAGS += -I../../include
LIBS += -L$(buildlib) -lipe
CXXFLAGS += $(DLL_CFLAGS)

all: $(TARGET)

sources	= inscribe.cpp cut.cpp libs.cpp point.cpp compute.cpp

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
