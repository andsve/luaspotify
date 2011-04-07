LUA_INC= /usr/include/lua5.1

#~ WARN= -Wall -Wmissing-prototypes -Wmissing-declarations -ansi -pedantic
WARN= 
INCS= -I$(LUA_INC) -Isrc/
CFLAGS= -O2 $(WARN) $(INCS) $(DEFS)
CXXFLAGS= -O2 $(WARN) $(INCS) $(DEFS)
CC= g++

# OS dependent
#LIB_OPTION= -llibspotify -shared #for Linux
LIB_OPTION= -framework libspotify -bundle -undefined dynamic_lookup #for MacOS X

LIBNAME= spotify.so

SRCS= $(wildcard src/*.cpp)
OBJS= $(SRCS:.cpp=.o)
AR= ar rcu
RANLIB= ranlib

lib: $(LIBNAME)

$(LIBNAME): $(OBJS)
	export MACOSX_DEPLOYMENT_TARGET="10.3"; $(CC) $(CFLAGS) -o $@ $(LIB_OPTION) $(OBJS)

#~ $(COMPAT_DIR)/compat-5.1.o: $(COMPAT_DIR)/compat-5.1.c
#~ $(CC) -c $(CFLAGS) -o $@ $(COMPAT_DIR)/compat-5.1.c

#~ install:
#~ mkdir -p $(LUA_LIBDIR)/spotify
#~ cp $(LIBNAME) $(LUA_LIBDIR)/spotify

clean:
	rm -f $(LIBNAME) src/*.o