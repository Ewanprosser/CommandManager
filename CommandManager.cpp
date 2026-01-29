/*
    Command Manager Message Parser

    Implements parsing logic for text-based Command Manager messages used to control wind tunnel measurement subsystems. 
    Supports opcode-based commands, parameter parsing, and history tracking of recently received commands.

    Assumptions: 
    - All messages are UTF-8 encoded and terminate with a '#' character.
    - Opcodes are exactly 10 characters in length
    - D_USR_FLD_ messages contain comma-separated parameter name/value pairs.
    - Parameter names are no longer than 15 characters.
    - Parameter values are valid floating-point numbers with up to 6 decimal places.
    - Invalid or unknown opcodes are ignored safely.

    This file also contains a test in main() demonstrating example messages,
    followed by optional interactive input for manual testing.

    Developed as a technical assesment using C++17 in Visual Studio 2019.
*/

#include <iostream>
#include <string>
#include <deque>
#include <sstream>
#include <vector>

//Function to update the history of the 5 previous Opcodes, this uses a deque (double ended queue) so we can add and remove elements easily from the front and back.
void updateHistory(std::deque<std::string>& strHistory, const std::string& strOpCode) {

    //Add the current OpCode to the start of the queue.
    strHistory.push_front(strOpCode);

    //If the queue is now longer than 5, remove the entry at the back of the queue (The history function only needs to show the last 5 entries). 
    if (strHistory.size() > 5) {
        strHistory.pop_back();
    }
}

//Main function to parse command manager messages.
void parseCommand(const std::string& strInput, std::deque<std::string>& strHistory) {

    //Bool Identifier to check whether the opcode is recognised, if it's not we don't want to add it to the history.
    bool bRecognised = false;

    //Check whether the message ends in a #
    if (strInput.empty() || strInput.back() != '#') {
        return;
    }
    //If the length of the input if over 11, it has enough characters to be a valid opcode (10 for the opcode plus #), proceed with handling.
    if (strInput.length() >= 11) {
        //Extract the opcode from the first 10 letters of the input, and then anything past that extract as the message content.
        std::string strOpCode = strInput.substr(0, 10);
        std::string strMessageContent = strInput.substr(10);

        //If the message content is not empty and the last character is a #, remove the #.
        if (!strMessageContent.empty() && strMessageContent.back() == '#') {
            strMessageContent.pop_back();
        }

        //If the opcode is RUN_NO____ print the run number, set recognised to true
        if (strOpCode == "RUN_NO____") {
            bRecognised = true;
            //Convert the message content to an int and check whether it's a valid number.
            try {
                int iRunNumber = std::stoi(strMessageContent);
                //Print run number to console. 
                std::cout << "Run number: " << iRunNumber << "\n";
            }
            //If it's not a valid number show exception message.
            catch (const std::exception&) {
                std::cout << "Invalid Run number: " << strMessageContent << "\n";
            }

        }
        //if the opcode is POLAR_NO__ print the polar number, set recognised to true
        else if (strOpCode == "POLAR_NO__") {
            bRecognised = true;
            //Convert the message content to an int and check whether it's a valid number.
            try {
                int iPolarNumber = std::stoi(strMessageContent);
                //Print polar number to console. 
                std::cout << "Polar number: " << iPolarNumber << "\n";
            }
            //If it's not a valid number show exception message.
            catch (const std::exception&) {
                std::cout << "Invalid Polar number: " << strMessageContent << "\n";
            }

        }
        //if the opcode is USR_MSG___ print the message contents.
        else if (strOpCode == "USR_MSG___") {
            bRecognised = true;
            //Print message contents to console. 
            std::cout << strMessageContent << "\n";
        }
        //if the OpCode is D_USR_FLD_, print the list of parameter names and values.
        else if (strOpCode == "D_USR_FLD_") {
            bRecognised = true;
            //Split the message content on commas to extract parameter names and values, removing the commas. Store in a vector.
            std::vector<std::string> vecTokens;
            std::stringstream ssMessageContent(strMessageContent);
            std::string strToken;
            while (std::getline(ssMessageContent, strToken, ',')) {
                //Ignore empty tokens while tokenising
                if (!strToken.empty()) {
                    vecTokens.push_back(strToken);
                }
            }
            //Check whether we have an equal number of parameter names and parameter values.
            if (vecTokens.size() % 2 != 0) {
                return;
            }
            //Loop through the vector token list in pairs ( parameter name, parameter value ) and print each value.
            std::cout << "Parameters:\n";
            for (size_t i = 0; i + 1 < vecTokens.size(); i += 2) {
                // Check parameter name length (max 15 characters)
                if (vecTokens[i].length() > 15) {
                    std::cout << "Parameter name too long: " << vecTokens[i] << "\n";
                    continue;  // Skip this pair and move to the next one
                }
                //Attempt to convert the parameter value to a double. If it fails, it's not a number and is therefore invalid.
                try {
                    std::string strParmName = vecTokens[i];
                    double dblParmValue = std::stod(vecTokens[i + 1]);
                    std::cout << strParmName << " = " << dblParmValue << "\n";
                }
                catch (const std::exception&) {
                    std::cout << "Invalid parameter value for parameter: " << vecTokens[i] << "\n";
                }
            }
        }
        //If the OpCode is HISTORY___, loop through the History double ended queue and show the last five entries.
        else if (strOpCode == "HISTORY___") {
            for (size_t i = 0; i < strHistory.size(); i++) {
                std::cout << strHistory[i] << "\n";
            }
        }
        else {
            //The entry is not a valid OpCode - do nothing. 
        }

        // Add only recognised opcodes to history (HISTORY___ and unknown opcodes are excluded)
        if (bRecognised) {
              updateHistory(strHistory, strOpCode);
        }
    }
}

int main()
{
    //Declare variables to be used in testing
    std::deque<std::string> strHistory;
    std::string strUserInput;

    //Run a test example for each OpCode type, as well as an invalid opcode
    std::cout << "Running example Command Manager messages...\n\n";
    parseCommand("RUN_NO____123#", strHistory);
    parseCommand("POLAR_NO__2#", strHistory);
    parseCommand("USR_MSG___Start Tunnel#", strHistory);
    parseCommand("D_USR_FLD_Parameter1,0.004947,Parameter2,0.203044,#", strHistory);
    parseCommand("RUN_NO____124#", strHistory);
    parseCommand("POLAR_NO__3#", strHistory);
    parseCommand("D_USR_FLD_Parameter3,0.02347,Parameter4,0.12343044,ParameterT,1.12345,#", strHistory);
    parseCommand("HISTORY___#", strHistory);

    //Test Unknown and failsafes
    parseCommand("UNKNOWN___test#", strHistory);
    parseCommand("RUN_NO____ABC#", strHistory);
    parseCommand("RUN_NO____123", strHistory);

    //Allow user to test functionality by manually adding OpCode
    std::cout << "Enter command messages (type EXIT to quit):\n";
    //Program will keep allowing user to enter OpCodes until they enter EXIT.
    while (true) {
        std::getline(std::cin, strUserInput);
        if (strUserInput == "EXIT") {
            break;
        }
        parseCommand(strUserInput, strHistory);
    }

    return 0;
}

