all: webserver

webserver: webserver.cpp
	g++ -o webserver webserver.cpp -std=c++11

clean:
	rm -f webserver

submit:
	~schwesin/bin/submit cpsc328 project4

.PHONY: submit clean
