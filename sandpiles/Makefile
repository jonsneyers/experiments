CFLAGS=`pkg-config --cflags opencv`
LDFLAGS=`pkg-config --libs opencv`

sandpile: sandpile.cpp
	g++ -std=c++11 -O2 $(CFLAGS) sandpile.cpp $(LDFLAGS) -o sandpile
