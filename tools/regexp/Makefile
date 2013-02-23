AR=ar
ARFLAGS=-cr
RL=ranlib
RLFLAGS=
CC=g++
CXXFLAGS=-I. -c -Wall

SRC=                \
Matcher.cpp         \
Pattern.cpp         \
UnicodeMatcher.cpp  \
UnicodePattern.cpp  \

OBJ=$(SRC:.cpp=.o)

libcppre.a: $(OBJ)
	$(AR) $(ARFLAGS) libcppre.a $(OBJ)
	$(RL) libcppre.a

.cpp.o:
	$(CC) $(CXXFLAGS) $<

distclean: clean

clean:
	rm -f $(OBJ) libcppre.a
