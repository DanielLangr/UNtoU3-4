CC = g++
CXXFLAGS = -std=c++20
CXXDEBUG_FLAGS = $(CXXFLAGS) -O0 -g
CXXRELEASE_FLAGS = $(CXXFLAGS) -O2 -DNDEBUG

BOOTS_FLAGS = 
#BOOST_FLAGS = -DHAVE_BOOST

binaries = test_10401 test_input 

.PHONY: all
all: $(binaries)
		
test_10401: %: %.cpp
	$(CC) $(CXXDEBUG_FLAGS) -o $@ $<

test_input: %: %.cpp
	$(CC) $(CXXRELEASE_FLAGS) $(BOOST_FLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f $(binaries)
