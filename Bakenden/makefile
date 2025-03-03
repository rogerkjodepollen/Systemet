all: systemet bakenden hemmelig


WARNINGS = -Wall -Weffc++ -Wextra -Wsign-conversion -pedantic-errors
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2
STANDARD = -std=c++2a
LINKER = -lGL -lGLEW -lglfw -ldl -lsodium

SRCS = main.cpp graphics_base.cpp base_window.cpp panel_window.cpp text_window.cpp scroll_window.cpp images.cpp topmenu_window.cpp draw_tools.cpp client.cpp

BAKENDENSRC = ./Bakenden/main.cpp ./Bakenden/server.cpp

HEMMELIGSRC = ./Hemmelig/main.cpp ./Hemmelig/randgen.cpp

systemet: $(SRCS)
	$(CXX) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) $(STANDARD) $(SRCS) $(LINKER)

bakenden: $(BAKENDENSRC)
	$(CXX) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) $(STANDARD) $(BAKENDENSRC) -lsodium


hemmelig: $(HEMMELIGSRC)
	$(CXX) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) $(STANDARD) $(HEMMELIGSRC)



clean:
	rm -f systemet bakenden hemmelig


