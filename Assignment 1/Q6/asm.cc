#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <map>

/** Prints an error to stderr with an "ERROR: " prefix, and newline suffix.
 *
 * @param message The error to print
 */
 void formatError(const std::string & message)
 {
     std::cerr << "ERROR: " << message << std::endl;
 }

// Helper functions to check if the register and immediate are in range
bool checkReg(uint32_t r) {
    if (r > 31) {
        formatError("Register out of range (0–31)");
        return false;
    }
    return true;
}

bool checkImm(uint32_t imm) {
    if (imm > 0xFFFF) {
        formatError("Immediate out of range (0–65535)");
        return false;
    }
    return true;
}


/** For a given instruction, returns the machine code for that instruction.
 *
 * @param[out] word The machine code for the instruction
 * @param instruction The name of the instruction
 * @param one The value of the first parameter
 * @param two The value of the second parameter, 0 if the instruction has < 2 parameter
 * @param three The value of the third parameter, 0 if the instruction has < 3 parameter
 */
bool compileLine(uint32_t & word,
                 const std::string & instruction,
                 uint32_t one,
                 uint32_t two,
                 uint32_t three)
{
    word = 0;

    if (instruction == "add") {
        if (!checkReg(one) || !checkReg(two) || !checkReg(three)) {
            return false;
        }
        word = (0 << 26) | (two << 21) | (three << 16) | (one << 11) | (0 << 6) | 32;
    } else if (instruction == "sub") {
        if (!checkReg(one) || !checkReg(two) || !checkReg(three)) {
            return false;
        }
        word = (0 << 26) | (two << 21) | (three << 16) | (one << 11) | (0 << 6) | 34;
    } else if (instruction == "mult") {
        if (!checkReg(one) || !checkReg(two)) {
            return false;
        }
        word = (0 << 26) | (one << 21) | (two << 16) | (0 << 11) | (0 << 6) | 24;
    } else if (instruction == "multu") {
        if (!checkReg(one) || !checkReg(two)) {
            return false;
        }
        word = (0 << 26) | (one << 21) | (two << 16) | (0 << 11) | (0 << 6) | 25;
    } else if (instruction == "div") {
        if (!checkReg(one) || !checkReg(two)) {
            return false;
        }
        word = (0 << 26) | (one << 21) | (two << 16) | (0 << 11) | (0 << 6) | 26;
    } else if (instruction == "divu") {
        if (!checkReg(one) || !checkReg(two)) {
            return false;
        }
        word = (0 << 26) | (one << 21) | (two << 16) | (0 << 11) | (0 << 6) | 27;
    } else if (instruction == "mfhi") {
        if (!checkReg(one)) {
            return false;
        }
        word = (0 << 26) | (0 << 21) | (0 << 16) | (one << 11) | (0 << 6) | 16;
    } else if (instruction == "mflo") {
        if (!checkReg(one)) {
            return false;
        }
        word = (0 << 26) | (0 << 21) | (0 << 16) | (one << 11) | (0 << 6) | 18;
    } else if (instruction == "lis") {
        if (!checkReg(one)) {
            return false;
        }
        word = (0 << 26) | (0 << 21) | (0 << 16) | (one << 11) | (0 << 6) | 20;
    } else if (instruction == "slt") {
        if (!checkReg(one) || !checkReg(two) || !checkReg(three)) {
            return false;
        }
        word = (0 << 26) | (two << 21) | (three << 16) | (one << 11) | (0 << 6) | 42;
    } else if (instruction == "sltu") {
        if (!checkReg(one) || !checkReg(two) || !checkReg(three)) {
            return false;
        }
        word = (0 << 26) | (two << 21) | (three << 16) | (one << 11) | (0 << 6) | 43;
    } else if (instruction == "jr") {
        if (!checkReg(one)) {
            return false;
        }
        word = (0 << 26) | (one << 21) | (0 << 16) | (0 << 11) | (0 << 6) | 8;
    } else if (instruction == "jalr") {
        if (!checkReg(one)) {
            return false;
        }
        word = (0 << 26) | (one << 21) | (0 << 16) | (0 << 11) | (0 << 6) | 9;
    } else if (instruction == "beq") {
        if (!checkReg(one) || !checkReg(two) || !checkReg(three)) {
            return false;
        }
        word = (4 << 26) | (one << 21) | (two << 16) | (three & 0xFFFF);
    } else if (instruction == "bne") {
        if (!checkReg(one) || !checkReg(two) || !checkReg(three)) {
            return false;
        }
        word = (5 << 26) | (one << 21) | (two << 16) | (three & 0xFFFF);
    } else if (instruction == "lw") {
        if (!checkReg(one) || !checkReg(three)) {
            formatError("Register out of range (must be 0–31)");
            return false;
        }
        if (two > 0xFFFF) {
            formatError("Immediate out of range (must be 16 bits)");
            return false;
        }
        word = (35 << 26) | (three << 21) | (one << 16) | (two & 0xFFFF);
    } else if (instruction == "sw") {
        word = (43 << 26) | (three << 21) | (one << 16) | (two & 0xFFFF);
    } else {
        formatError("Unknown instruction: " + instruction);
        return false;
    }

    return true;
}

/** Matches a line of mips assembly */
std::regex ASM_LINE_PATTERN(
    "^\\s*([a-z]+)\\s*((\\$)(\\d+)|((0x[0-9a-fA-F]*)|(\\d+)))\\s*(,\\s*(((\\$)?(\\d+)|0x[0-9a-fA-F]*)|((0x[0-9a-fA-F]*)|(-?\\d+))\\s*\\(\\s*(\\$)(\\d+)\\s*\\))?\\*?(,\\s*(((\\$)?(-?\\d+))|(0x[0-9a-fA-F]*)))?)?\\s*$");

/** Recognizes an empty line (or an empty line with a comment) */
std::regex EMPTY_LINE("^\\s*(;.*)?$");

/** For a type of instruction--represented by the meta variables d, s, t, a, and i--records the
 *  location of the parameter value in the regex match output. */
struct MatchOffsets
{
    const std::string pattern;
    const uint8_t parameters[3];
    const uint8_t registerChecks[3];
};
const std::vector<MatchOffsets> MATCH_OFFSETS = {
    // a $1, $2, $3
    {"rrr", {4, 12, 22}, {3, 11, 21}},

    // a $1, $2, 3
    // a $1, $2, 0x3
    {"rri", {4, 12, 19}, {3, 11,  0}},

    // a $1, $2
    {"rr ", {4, 12,  0}, {3, 11,  0}},

    // a $1, 1($3)
    // a $1, 0x1($3)
    {"rir", {4, 13, 17}, {3,  0, 16}},

    // a $1, 2
    // a $1, 0x2
    {"ri ", {4,  9,  0}, {3,  0,  0}},

    // a $1
    {"r  ", {4,  0,  0}, {3,  0,  0}},

    // a i
    {"i  ", {2,  0,  0}, {0,  0,  0}},
};

/** Maps the instruction name to the parameter type.  The value must be a 3 character string, 'r'
 *  represents a register, and 'i' represents an immediate. */
const std::map<std::string, std::string> INSTRUCTION_PARAMETER_PATTERN = {
    {"add",   "rrr"},
    {"addi",  "rri"},
    {"and",   "rrr"},
    {"andi",  "rri"},
    {"div",   "rr "},
    {"divu",  "rr "},
    {"mult",  "rr "},
    {"multu", "rr "},
    {"nor",   "rrr"},
    {"or",    "rrr"},
    {"ori",   "rri"},
    {"sll",   "rri"},
    {"sllv",  "rrr"},
    {"sra",   "rri"},
    {"srav",  "rrr"},
    {"srl",   "rri"},
    {"srlv",  "rrr"},
    {"sub",   "rrr"},
    {"xor",   "rrr"},
    {"xori",  "rri"},
    {"lui",   "ri "},
    {"mfhi",  "r  "},
    {"mflo",  "r  "},
    {"mthi",  "r  "},
    {"mtlo",  "r  "},
    {"slt",   "rrr"},
    {"sltu",  "rrr"},
    {"slti",  "rri"},
    {"slti",  "rri"},
    {"sltiu", "rri"},
    {"beq",   "rri"},
    {"bgtz",  "ri "},
    {"blez",  "ri "},
    {"bne",   "rri"},
    {"j",     "i  "},
    {"jal",   "i  "},
    {"jalr",  "r  "},
    {"jr",    "r  "},
    {"lb",    "rir"},
    {"lbu",   "rir"},
    {"lis",   "r  "},
    {"lh",    "rir"},
    {"lhu",   "rir"},
    {"lw",    "rir"},
    {"sb",    "rir"},
    {"sh",    "rir"},
    {"sw",    "rir"},
};

/** Convert a string representation of a number to an unsigned integer.
 *
 * If the string is "0", then 0 is returned.  If the string starts with "0x", the string is
 * interpreted as an unsigned hexidecimal number.  If the string starts with a "0", the string is
 * interpreted as an unsigned octal number.  Otherwise, the string is interpreted as a signed
 * decimal number.
 *
 * The function name is read as "string to uint32".
 *
 * @param s The string to parse
 * @return The uint32_t representation of the string
 */
uint32_t stouint32(const std::string & s)
{
    if(s == "0")
    {
        return 0;
    }
    if(s.starts_with("0x"))
    {
        return std::stoi(s.substr(2), nullptr, 16);
    }

    if(s.starts_with("0"))
    {
        return std::stoi(s.substr(1), nullptr, 8);
    }

    return std::stoi(s);
}


/** Compiles one line of assembly and send the binary to standard out.  If the assembly is invalid,
 *  print an error to stderr and return false.  Assumes that the assembly does not have a trailing
 *  comment.
 *
 * @param line The line to parse
 * @return True if the line is valid assembly and was output to stdout, false otherwise
 */
bool parseLine(const std::string & line)
{
    std::smatch matches;
    if(!std::regex_search(line, matches, ASM_LINE_PATTERN))
    {
        formatError((std::stringstream() << "Unable to parse line: \"" << line << "\"").str());
        return false;
    }

    std::string instruction = matches[1];

    uint32_t parameters[3] = {0, 0, 0};
    std::string parsedParameterPattern = "";
    bool matchPatternFound = false;
    for(const auto & iter : MATCH_OFFSETS)
    {
        if(((iter.parameters[0] == 0 || matches[iter.parameters[0]] != "") && (iter.registerChecks[0] == 0 || matches[iter.registerChecks[0]] == "$"))
        && ((iter.parameters[1] == 0 || matches[iter.parameters[1]] != "") && (iter.registerChecks[1] == 0 || matches[iter.registerChecks[1]] == "$"))
        && ((iter.parameters[2] == 0 || matches[iter.parameters[2]] != "") && (iter.registerChecks[2] == 0 || matches[iter.registerChecks[2]] == "$"))
        && (iter.pattern.at(1) != ' ' || (matches[9]  == "" && matches[12] == "" && matches[13] == ""))
        && (iter.pattern.at(2) != ' ' || (matches[17] == "" && matches[19] == "" && matches[22] == "")))
        {
            matchPatternFound = true;
            parsedParameterPattern = iter.pattern;
            for(int i = 0; i < 3; i += 1)
            {
                if(iter.parameters[i] != 0)
                {
                    parameters[i] = stouint32(matches[iter.parameters[i]]);
                }
            }

            break;
        }
    }

    if(!matchPatternFound)
    {
        formatError((std::stringstream() << "Unable to parse line: \"" << line << "\"").str());
        return false;
    }

    const auto & instructionParameterPattern = INSTRUCTION_PARAMETER_PATTERN.find(instruction);
    if(instructionParameterPattern == INSTRUCTION_PARAMETER_PATTERN.end())
    {
        formatError((std::stringstream() << "'" << instruction << "' is not a known instruction").str());
        return false;
    }

    if(parsedParameterPattern != instructionParameterPattern->second)
    {
        formatError(
            (std::stringstream() << "Invalid parameter pattern '" << parsedParameterPattern << "' "
                                 << "for " << instruction << ", "
                                 << "expecting '" << instructionParameterPattern->second << "'").str());
        return false;
    }

    uint32_t binary = 0;
    bool compiled = compileLine(binary,
                                instruction,
                                parameters[0],
                                parameters[1],
                                parameters[2]);
    if(compiled)
    {
        std::cout << (char)((binary >> 24) & 0xFF)
                  << (char)((binary >> 16) & 0xFF)
                  << (char)((binary >>  8) & 0xFF)
                  << (char)((binary >>  0) & 0xFF);
        return true;
    }
    else
    {
        // compileLine (should have) printed an error.  We don't have to print one here.
        return false;
    }
}

/** Entrypoint for the assembler.  The first parameter (optional) is a mips assembly file to
 *  read.  If no parameter is specified, read assembly from stdin.  Prints machine code to stdout.
 *  If invalid assembly is found, prints an error to stderr, stops reading assembly, and return a
 *  non-0 value.
 *
 * If the file is not found, print an error and returns a non-0 value.
 *
 * @return 0 on success, non-0 on error
 */
int main(int argc, char * argv[])
{
    if(argc > 2)
    {
        std::cerr << "Usage:" << std::endl
                  << "\tasm [$FILE]" << std::endl
                  << std::endl
                  << "If $FILE is unspecified or if $FILE is `-`, read the assembly from standard "
                  << "in. Otherwise, read the assembly from $FILE." << std::endl;
        return 1;
    }

    std::ifstream fp;
    std::istream &in =
        (argc > 1 && std::string(argv[1]) != "-")
      ? [&]() -> std::istream& {
            fp.open(argv[1]);
            return fp;
        }()
      : std::cin;

    if(!fp && argc > 1)
    {
        formatError((std::stringstream() << "file '" << argv[1] << "' not found!").str());
        return 1;
    }

    while(!in.eof())
    {
        std::string line;
        std::getline( in, line );

        // Filter out any comments
        uint32_t startComment = line.find(";");
        if(startComment != std::string::npos)
        {
            line = line.substr(0, line.find(";"));
        }

        std::smatch matches;
        if(std::regex_search(line, matches, EMPTY_LINE))
        {
            continue;
        }

        if(!parseLine(line))
        {
            return 1;
        }
    }

    return 0;
}