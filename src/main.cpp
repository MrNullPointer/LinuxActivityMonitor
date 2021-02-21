#include "ncurses_display.h"
#include "system.h"

//create a system object and pass that object to NCursesDisplay
int main() {
  System system;
  NCursesDisplay::Display(system);
}
