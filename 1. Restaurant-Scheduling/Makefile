sourcefiles = src/Restaurant.java

classfiles  = $(sourcefiles:.java=.class)

all: $(classfiles)

MAIN: Restaurant

%.class: %.java
	javac -d . -classpath . $<

run: classfiles
    $(JVM) $(MAIN)

clean:
	rm -f *.class
