CXX = g++
CXXFLAGS = -O0 -g -Wall -Wextra -Werror -Wno-sign-compare -Wno-deprecated -std=c++14 -I $(CPLUS_INCLUDE_PATH)

driver_easydial.exe: driver_easydial.o phone.o call_registry.o easy_dial.o dialog.o
	$(CXX) -o driver_easydial.exe driver_easydial.o phone.o call_registry.o easy_dial.o dialog.o $(CXXFLAGS) -L $(LIBRARY_PATH) -lesin
	
driver_easydial.o: driver_easydial.cpp dialog.hpp easy_dial.hpp call_registry.hpp phone.hpp 
	$(CXX) -c driver_easydial.cpp $(CXXFLAGS)

dialog.o: dialog.cpp dialog.hpp easy_dial.hpp call_registry.hpp phone.hpp 
	$(CXX) -c dialog.cpp $(CXXFLAGS) 

easy_dial.o: easy_dial.cpp easy_dial.hpp easy_dial.rep call_registry.hpp phone.hpp
	$(CXX) -c easy_dial.cpp $(CXXFLAGS)

call_registry.exe: call_registry.o call_registry.cpp call_registry.hpp call_registry.rep phone.o
	$(CXX) -o call_registry.exe call_registry.o phone.o  $(CXXFLAGS) -L $(LIBRARY_PATH) -lesin

call_registry.o: call_registry.cpp call_registry.hpp call_registry.rep phone.hpp
	$(CXX) -c call_registry.cpp $(CXXFLAGS)

phone.o: phone.cpp phone.hpp phone.rep
	$(CXX) -c phone.cpp $(CXXFLAGS)

clean:
	rm -vf *.o
	rm -vf *.exe
	rm -vf *.gch
	
test:
	./driver_easydial.exe < jp_public.in | diff - jp_public.res
#	./driver_easydial.exe < jp_public_phone.in | diff - jp_public_phone.res
#	./driver_easydial.exe < jp_public_call_registry.in | diff - jp_public_call_registry.res
#	./driver_easydial.exe < jp_public_easy_dial.in | diff - jp_public_easy_dial.res
#	./driver_easydial.exe < jp_public_dialog.in | diff - jp_public_dialog.res