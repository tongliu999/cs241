#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

const std::string ALPHABET    = ".ALPHABET";
const std::string STATES      = ".STATES";
const std::string TRANSITIONS = ".TRANSITIONS";
const std::string INPUT       = ".INPUT";
const std::string EMPTY       = ".EMPTY";

std::vector<char> validLanguage;
std::map<std::string,int> stateId;
std::vector<std::string> states;
std::vector<bool> isAccepting;
int startId = -1; 

struct Edge { int from, to; char ch; };
std::vector<Edge> edges;

bool isChar(std::string s) {
  return s.length() == 1;
}
bool isRange(std::string s) {
  return s.length() == 3 && s[1] == '-';
}
bool inAlphabet(char ch) {
  return std::find(validLanguage.begin(), validLanguage.end(), ch) != validLanguage.end();
}

int main() {
  std::istream& in = std::cin;
  std::string s;

  std::getline(in, s);
  while(in >> s) {
    if (s == STATES) { 
      break; 
    } else {
      if (isChar(s)) {
        validLanguage.push_back(s[0]);
      } else if (isRange(s)) {
        for(char c = s[0]; c <= s[2]; ++c) {
          if (c != '-') {
            validLanguage.push_back(c);
          }
        }
      } 
    }
  }

  std::getline(in, s);
  while(in >> s) {
    if (s == TRANSITIONS) { 
      break; 
    } else {
      static bool initial = true;
      bool accepting = false;
      if (s.back() == '!' && !isChar(s)) {
        accepting = true;
        s.pop_back();
      }

      if (stateId.find(s) == stateId.end()) {
        int id = static_cast<int>(states.size());
        stateId[s] = id;
        states.push_back(s);
        isAccepting.push_back(accepting);
        if (initial) {
          startId = id;
          initial = false;
        }
      } else {
        int id = stateId[s];
        if (accepting) isAccepting[id] = true;
      }
    }
  }

  stateId["invalid"] = static_cast<int>(states.size());
  states.push_back("invalid");
  isAccepting.push_back(false);

  std::getline(in, s);
  while(std::getline(in, s)) {
    if (s == INPUT) { 
      break; 
    } else {
      std::string fromState, symbols, toState;
      std::istringstream line(s);
      std::vector<std::string> lineVec;
      while(line >> s) {
        lineVec.push_back(s);
      }
      fromState = lineVec.front();
      toState = lineVec.back();
      for(int i = 1; i < (int)lineVec.size()-1; ++i) {
        std::string s = lineVec[i];
        if (isChar(s)) {
          symbols += s;
        } else if (isRange(s)) {
          for(char c = s[0]; c <= s[2]; ++c) {
            symbols += c;
          }
        }
      }

      int fromId = stateId.at(fromState);
      int toId = stateId.at(toState);

      for (char c : symbols) {
        if (!inAlphabet(c)) continue;
        edges.push_back({fromId, toId, c});
      }
    }
  }

  std::string input;
  char c;
  while (in >> std::noskipws >> c) {
    input.push_back(c);
  }

  if (!input.empty() && input.back() == '\n') {
    input.pop_back();
  } 

  size_t pos = 0;
  while (pos < input.size()) {
    int currState = startId;
    int lastAcceptState = -1;
    size_t lastAcceptPos = pos;

    size_t i = pos;
    for (; i < input.size(); ++i) {
      char c = input[i];
      bool moved = false;
      for (const auto &e : edges) {
        if (e.from == currState && e.ch == c) {
          currState = e.to;
          moved = true;
          if (isAccepting[currState]) {
            lastAcceptState = currState;
            lastAcceptPos = i + 1;
          }
          break;
        }
      }
      if (!moved) break;
    }

    if (lastAcceptState == -1) {
      std::cerr << "ERROR: Not an accepted final state\n";
      return 1;
    }

    std::cout << input.substr(pos, lastAcceptPos - pos) << "\n";
    pos = lastAcceptPos;
  }
}
