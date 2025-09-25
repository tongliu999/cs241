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

// valid language
std::vector<char> validLanguage;

// valid states
std::map<std::string,int> stateId;
std::vector<std::string> states;
std::vector<bool> isAccepting;
int startId = -1; 

// valid transitions
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

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {
  std::istream& in = std::cin;
  std::string s;

  std::getline(in, s); // Alphabet section (skip header)
  // Read characters or ranges separated by whitespace
  while(in >> s) {
    if (s == STATES) { 
      break; 
    } else {
      if (isChar(s)) {
        //// Variable 's[0]' is an alphabet symbol
        validLanguage.push_back(s[0]);
      } else if (isRange(s)) {
        for(char c = s[0]; c <= s[2]; ++c) {
          //// Variable 'c' is an alphabet symbol
          if (c != '-') {
            validLanguage.push_back(c);
          }
        }
      } 
    }
  }

  std::getline(in, s); // States section (skip header)
  // Read states separated by whitespace
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
      
        //// Variable 's' contains the name of a state
        if (initial) {
            //// The state is initial
            startId = id;
            initial = false;
        }
      }
    }
  }

  std::getline(in, s); // Transitions section (skip header)
  // Read transitions line-by-line
  while(std::getline(in, s)) {
    if (s == INPUT) { 
      // Note: Since we're reading line by line, once we encounter the
      // input header, we will already be on the line after the header
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
      for(int i = 1; i < lineVec.size()-1; ++i) {
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

  // Input section (already skipped header)
  while(in >> s) {
    int currState = startId;
    //// Variable 's' contains an input string for the DFA
    if (s == ".EMPTY") {
        std::cout << s << " .EMPTY";
    }

    for (char c : s) {
        if (!inAlphabet(c)) {
        std::cerr << "ERROR: Invalid alphabet symbol " << c << " in input string: " << s << "\n";
        auto itInv = stateId.find("invalid");
        if (itInv != stateId.end()) currState = itInv->second;
        break;
        }
        bool moved = false;
        for (const auto& e : edges) {
        if (e.from == currState && e.ch == c) {
            currState = e.to;
            moved = true;
            break;
        }
        }
        if (!moved) {
        auto itInv = stateId.find("invalid");
        if (itInv != stateId.end()) currState = itInv->second;
        break;
        }
    }
    
    if (isAccepting[currState]) {
        std::cout << s << " true";
    } else {
        std::cout << s << " false";
    }
  }
}