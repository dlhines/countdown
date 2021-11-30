#include <iostream>
#include <cctype>
#include <vector>
#include <random>
#include <fstream>
#include <filesystem>
#include <cpr/cpr.h>
#include "countdown.h"

void Countdown::runHeader() {
  std::cout << clear;
  std::cout << "------------------------" << std::endl;
  std::cout << "| Welcome to COUNTDOWN |" << std::endl;
  std::cout << "------------------------" << std::endl;
}
void Countdown::regenerateHeader(char head) {
  std::cout << clear ;
  runHeader();
  std::cout << std::endl;

  switch(head) {
    case 'r' :
      std::cout << "Play On!" << std::endl;
      std::cout << "------------------------" << std::endl;
      std::cout << "Target: " << target << std::endl;
      std::cout << "Big Values: " << big << std::endl;
      std::cout << "Small Values: " << little << std::endl;
      std::cout << "Your Values: ";
      for(int ele : values) std::cout << ele << " ";
      std::cout << std::endl;
      std::cout << "------------------------" << std::endl;
      break;
    case 's' :
      std::cout << "You have " << little << " Small values set" << std::endl;
      break;
    case 'l' :
      std::cout << "You have " << big << " Large values set" << std::endl;
      break;
    case 'd' :
      std::cout << "Defaults have been restored:"<<  std::endl;
      std::cout << " - Target value: " << target << std::endl;
      std::cout << " - Large values: " << big << std::endl;
      std::cout << " - Small values: " << little << std::endl;
      break;
    case 'a' :
      std::cout << "Based on the British game show"<<  std::endl;
      std::cout << "'8 Out of 10 Cats does Countdown'." <<  std::endl;
      std::cout << std::endl;
      std::cout << "Use addition, subtraction, multiplication, and" << std::endl;
      std::cout << "division to reach the Target value using" << std::endl;
      std::cout << "6 randomly generated values" <<  std::endl;
      break;
  }

  // Regenerated headers for printing internal errors
  switch(head) {
    case 'P' :
      std::cout << "Success! " << filename << " was created in the games folder!" << std::endl;
      break;
    case 'H' :
      std::cout << "FAILED! Header file was not found for file creation." << std::endl;
      break;
    case 'B' :
      std::cout << "FAILED! " << filename << " has failed to be created." << std::endl;
      break;
    case 'F' :
      std::cout << "FAILED! Footer file was not found for file creation." << std::endl;
      break;
    case 'N' :
      std::cout << filename << " was not created. Check errors below:" << std::endl;
      for (auto errs : errors) {
        std::cout << "- " << errs << std::endl;
      }
      break;
    case 'O' :
      std::cout << "Please enter a valid selection from the list below." << std::endl;
  }

  runMenu();
}

void Countdown::runMenu() {
  std::cout << std::endl;
  std::cout << "Main Menu" << std::endl;
  std::cout << "------------------------" << std::endl;
  std::cout << "R - Run Game with Defaults" << std::endl;
  std::cout << "P - Print a Game to HTML" << std::endl;
  std::cout << "L - Set # of Large values (#'s 50 - 100)" << std::endl;
  std::cout << "S - Set # of Small values (#'s 1 - 10)" << std::endl;
  std::cout << "D - Reset ALL Default values" << std::endl;
  std::cout << "A - About Countdown" << std::endl;
  std::cout << "Q - Quit Game" << std::endl;
  std::cout << "------------------------" << std::endl;
  std::cout << "Choose: ";
  std::cin >> option;
  runOptions(option);
}

void Countdown::runOptions(char option) {
  switch(tolower(option)) {
    case 'r' :
      runGame(true);
      break;
    case 'p' :
      runPrintToFile();
      break;
    case 'l' :
      setBig();
      break;
    case 's' :
      setLittle();
      break;
    case 'd' :
      resetDefaults();
      break;
    case 'a' :
      runAbout();
      break;
    case 'q' :
      std::cout << clear ;
      exit(0);
      break;
    default :
      char head = 'O';
      regenerateHeader(head);
      break;
  }
}

void Countdown::runGame(bool h){

  values.clear();
  setTargetNumber();
  for(int b = 0; b < big; b++) values.push_back(returnBig());
  for(int l = 0; l < little; l++) values.push_back(returnLittle());

  // Run regenerate head by value ot bool h
  if (h) {
    char head = 'r';
    regenerateHeader(head);
  }
}

void Countdown::runEnd() {}

void Countdown::resetDefaults() {
  big = 2;
  little = 4;
  target = 0;
  values.clear();
  char head = 'd';

  regenerateHeader(head);
}

void Countdown::runAbout() {
  char head = 'a';
  regenerateHeader(head);
}

void Countdown::runPrintToFile() {

  // Run game instance
  runGame(false);

  // char return from buildFile()
  char head = buildFile(std::string("header_template.html"), std::string("footer_template.html"));

  // Clear file vector
  file.clear();

  regenerateHeader(head);
}

char Countdown::buildFile(std::string header, std::string footer) {
  char result;
  std::fstream datafile;

  // https://stackoverflow.com/questions/16357999/current-date-and-time-as-string/16358264
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,sizeof(buffer),"%d_%m_%Y",timeinfo);
  std::string str(buffer);
  // to here

  filename = std::to_string(target) + "_" + str +".html";

  cpr::Response h = cpr::Get(cpr::Url{"https://infnx.space/countdown/bin/" + header},
  cpr::VerifySsl(false));

  cpr::Response f = cpr::Get(cpr::Url{"https://infnx.space/countdown/bin/" + footer},
  cpr::VerifySsl(false));

  datafile.open("../games/" + filename, std::ios::app);

  if (h.text.length() != 0) {
    result = 'P';
  } else {
    errors.push_back(h.error.message);
    result = 'H';
  }

  if (f.text.length() != 0) {
    result = 'P';
  } else {
    errors.push_back(f.error.message);
    result = 'F';
  }

  if (datafile) {
    // Write header to file
    datafile << h.text << std::endl;

    // Write game data
    datafile << "<h1>" << std::to_string(target) << "</h1>\n";
    datafile << "<div class=\"center-box container\">\n";
    for(int ele : values) datafile << "<span class=\"values\">" << ele << "</span>\n";
    datafile << "</div>\n";

    // Write footer to file
    datafile << f.text << std::endl;
    datafile.close();

  } else {
    result = 'B';
  }

  bool check = std::filesystem::exists("../games/" + filename);

  if(check) {
    result = 'P';
  } else {
    result = 'N';
  }

  return result;
}

// Accsessors
int Countdown::returnBig() {
  int num = numberGenerator(big_values_limits[0], big_values_limits[1]);

  // While big is not a number divisible by %
  while (num % 5 != 0) {
    num = numberGenerator(big_values_limits[0], big_values_limits[1]);
  }
  return num;
}

int Countdown::returnLittle() {
  return numberGenerator(little_values_limits[0], little_values_limits[1]);
}

int Countdown::numberGenerator(int min, int max) {
  std::random_device seeder;
  std::mt19937 gen(seeder());
  std::uniform_int_distribution<> dist(min, max);
  return dist(gen);

  // // Seed with a real random value, if available
  // std::random_device r;
  //
  // // Choose a random mean between 1 and 6
  // std::default_random_engine e1(r());
  // std::uniform_int_distribution<int> uniform_dist(min, max);
  // int _rand = uniform_dist(e1);
  //
  // return _rand;
}
