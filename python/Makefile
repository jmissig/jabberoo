CXX=g++-3.2
CXXFLAGS=`pkg-config --cflags jabberoo sigc++-1.2`
LDFLAGS=-L/opt/boost-gcc-3.2/lib
INCLUDES=-I/usr/include/python2.2 -I/opt/boost-gcc-3.2/include
LIBS=`pkg-config --libs jabberoo` -lboost_python
SIGC_LIBS=`pkg-config --libs sigc++-1.2` -lboost_python

judo_OBJECTS=JudoLink.o
jabberoo_OBJECTS=JabberOOLink.o JabberOOLinkSession.o JabberOOLinkSigC.o

all: judo jabberoo

clean:
	-rm -f *.o SigC.so Judo.so JabberOO.so testMOD.so

judo: $(judo_OBJECTS)
	$(CXX) -o Judo.so -shared $(judo_OBJECTS) $(LDFLAGS) $(LIBS)

jabberoo: $(jabberoo_OBJECTS)
	$(CXX) -o JabberOO.so -shared $(jabberoo_OBJECTS) $(LDFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

install:
	cp Judo.so JabberOO.so /usr/local/lib/python2.2/site-packages
