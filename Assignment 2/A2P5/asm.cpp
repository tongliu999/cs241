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

static const char* MIPS_DFA_TEXT = R"(
  .ALPHABET
  . a-z A-Z 0-9 : $ - , ( )
  .STATES
  start
  dir_head
  directive!
  id!
  label!
  zero!
  zero_x
  hexint!
  negative
  dec!
  dollar
  reg!
  reg_two!
  comma!
  lparen!
  rparen!
  .TRANSITIONS
  start . dir_head
  dir_head a-z directive
  dir_head A-Z directive
  directive a-z directive
  directive A-Z directive
  directive 0-9 directive
  start a-z id
  start A-Z id
  id a-z id
  id A-Z id
  id 0-9 id
  id : label
  start 0 zero
  zero x zero_x
  zero_x 0-9 hexint
  zero_x a-f hexint
  zero_x A-F hexint
  hexint 0-9 hexint
  hexint a-f hexint
  hexint A-F hexint
  start $ dollar
  dollar 0-9 reg
  reg 0-9 reg_two
  start - negative
  negative 1-9 dec
  start 1-9 dec
  dec 0-9 dec
  start , comma
  start ( lparen
  start ) rparen
  .INPUT
  )";
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

std::string toUpper(std::string t){
  for(char &c: t) if (c>='a' && c<='z') c = char(c - 'a' + 'A');
  return t;
}

static std::string tokenOf(const std::string& acc){
  if (acc=="directive") return "DIRECTIVE";
  if (acc=="label")     return "LABEL";
  if (acc=="id")        return "ID";
  if (acc=="hexint")    return "HEXINT";
  if (acc=="reg" || acc=="reg_two") return "REG";
  if (acc=="zero"|| acc=="dec")     return "DEC";
  if (acc=="comma")     return "COMMA";
  if (acc=="lparen")    return "LPAREN";
  if (acc=="rparen")    return "RPAREN";
  return toUpper(acc);
}

// Locations in the program that you should modify to store the
// DFA information have been marked with four-slash comments:
//// (Four-slash comment)
int main() {
  std::istringstream in(MIPS_DFA_TEXT);
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

  stateId["invalid"] = static_cast<int>(states.size());
  states.push_back("invalid");
  isAccepting.push_back(false);

  std::getline(in, s); // Transitions section (skip header)
  // Read transitions line-by-line
  while(std::getline(in, s)) {
    std::istringstream line(s);
    std::vector<std::string> lineVec;
    while(line >> s) {
      lineVec.push_back(s);
    }
    if (!lineVec.empty() && lineVec[0] == INPUT) {
      break;
    }
    if (lineVec.size() < 3) continue;
    std::string fromState, symbols, toState;
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

  // Input section (already skipped header)
  std::string input;
  char cch;
  while (std::cin >> std::noskipws >> cch) input.push_back(cch);

  size_t pos = 0;
  auto isWS = [](char c){ return c==' ' || c=='\n' || c=='\t' || c=='\r'; };

  while (pos < input.size()) {
    while (pos < input.size() && isWS(input[pos])) ++pos;
    if (pos >= input.size()) break;

    int currState = startId;
    int lastAcceptState = -1;
    size_t lastAcceptPos = pos;

    size_t i = pos;
    for (; i < input.size(); ++i) {
      char c = input[i];

      if (isWS(c)) break;

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
      char bad = input[pos];
      if (bad == '\n') std::cerr << "ERROR: unrecognized token starting at <NEWLINE>\n";
      else std::cerr << "ERROR: unrecognized token starting at '" << bad << "'\n";
      return 1;
    }

    std::string lexeme = input.substr(pos, lastAcceptPos - pos);
    std::string accName = states[lastAcceptState];
    std::cout << tokenOf(accName) << " " << lexeme << "\n";
    pos = lastAcceptPos;
  }
}