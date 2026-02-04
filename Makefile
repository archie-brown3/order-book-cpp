
test:
	g++ -o orderbook ./orderbook.cpp && ./orderbook 	

clean:
	rm -f orderbook

all:
	g++ -o orderbook orderbook.cpp -std=c++17

