//Neha Sawant PA0-B 
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>  
#include <vector>
#include <algorithm>

#define GATE_DELAY_N 7 

using namespace std;

// Structure to store gate information
struct Gate {
    int nodeno;   //nonde number
    string type;  //gate type
    vector<int> inputvalue;  //inputnode
    double outputSlew;      //outputslew
};

//function to read NLDM_lib_max2Inp and circuit file
int readFilecircuit(string fName ,string filename, int argc, char* argv[]);

//main function
int main(int argc, char* argv[])
{
    //to store the 1st lib file name 
    string fName;
   fName = argv[1];
   // to store 2nd circuit file name
    string ffileName;
    ffileName = argv[2];
    //store count of no of argument
    int argcount = argc;
    //function call to process the both files
    readFilecircuit(fName, ffileName, argcount, argv);

    return 0;
}

int readFilecircuit(string fName, string filename, int argc, char* argv[])
{  
    vector<string> v;                              // to store gate names
    std::map<int, vector<vector<double>> > map2;  //to store delay 7x7 values
    std::map<int, vector<vector<double>> > map3;  //to store slew 7x7 values
    vector<double> d;                            //to store one row of delay values and then to push back to map2
    vector<double> s;                           //to store one row of slew values and then to push back to map3
    int db_idx = 0;                             // count of delay for all gates iteration completed
    int db_idx1 = 0;                            // count of slew for all gates iteration completed
    ifstream loadFile(fName);
    
    //loads the files
    if(!loadFile)
    {
        cout << "The file \"" << fName << "\" failed to open.\n\n";
        return -1;
        }
    
    // Keep reading file while no stream error occurs
    // like end of file (eof) etc.
    while (loadFile.good()) {
        string first_word;
        loadFile >> first_word;

        // An info entry must start with "cell" as the first word exactly.
        if (first_word.compare("cell") == 0) {
            string second_word;
            loadFile >> second_word;

            // The gate name has the format of "(<gate_name>)".
            // So find the index till the closing bracket.
            // Make sure to ignore the first index which has the opening bracket (hence the 1 and -1)
            size_t delim_pos = second_word.find(")");
            string gate_name = second_word.substr(1, delim_pos - 1);

            cout << "Reading info about gate: " << gate_name << endl;
            v.push_back(gate_name);
           
        }
        // The cell delays will start after the word "cell_delay(Timing_7_7)" exactly
        else if (first_word.compare("cell_delay(Timing_7_7)") == 0) {
            // Read 3 lines that contain the rest of above match, index 1 and index 2
            string tmp;
            getline(loadFile, tmp);
            getline(loadFile, tmp);
            getline(loadFile, tmp);

            
             // 7 lines will have delays
            for (size_t i = 0; i < GATE_DELAY_N ; i++) {
                getline(loadFile, tmp);
                d.clear();
                // The delays will be between " ". Find the opening ".
                size_t start_delim_idx = tmp.find("\"");

                // Find the closing ".
                // The second argument is where we want to start our search
                // Ignore the first match so we don't get the same index again
                size_t end_delim_idx = tmp.find("\"", start_delim_idx + 1);

                // The second arg in substr in no. of characters, not the ending index
                string data_str = tmp.substr(start_delim_idx + 1, end_delim_idx - start_delim_idx - 1);

                // Convert this remaining string to a stream so we can parse our data in doubles
                istringstream data_stream(data_str);
                for (size_t j = 0; j < GATE_DELAY_N; j++) {
                    double delay;
                    char delim;
                    data_stream >> delay >> delim;

                    d.push_back(delay);
                   
                 }
                map2[db_idx].push_back(d);

            }

            // At the end of nested for loop we will have finised reading the 7x7 delays.
            // Increment our database pointer so we can store the next entry
            db_idx++;


        }
        else if (first_word.compare("output_slew(Timing_7_7)") == 0) {
            // Read 3 lines that contain the rest of above match, index 1 and index 2
            string tmp1;
            getline(loadFile, tmp1);
            getline(loadFile, tmp1);
            getline(loadFile, tmp1);

            // vector<vector<double>> v2;
             // From here on the next 7 lines will contain our slew
            for (size_t i = 0; i < GATE_DELAY_N; i++) {
                getline(loadFile, tmp1);
                s.clear();
                // The delays will be between " ". Find the opening ".
                size_t start_delim_idx = tmp1.find("\"");

                // Find the closing ".
                // The second argument is where we want to start our search
                // Ignore the first match so we don't get the same index again
                size_t end_delim_idx = tmp1.find("\"", start_delim_idx + 1);

                // The second arg in substr in no. of characters, not the ending index
                string data_str = tmp1.substr(start_delim_idx + 1, end_delim_idx - start_delim_idx - 1);

                // Convert this remaining string to a stream so we can parse our data in doubles
                istringstream data_stream(data_str);
                for (size_t j = 0; j < GATE_DELAY_N; j++) {
                    double slew;
                    char delim;
                    data_stream >> slew >> delim;

                    s.push_back(slew);
                   
                }
                map3[db_idx1].push_back(s);

            }

            // At the end of nested for loop we will have finised reading the 7x7 slew.
            // Increment our database pointer so we can store the next entry
            db_idx1++;


        }
    }
    loadFile.close();

    //vector of type structure created 
    vector<Gate> gates;
    ifstream circuitFile;

    //loads the file circuit
    circuitFile.open(filename.c_str(), ifstream::in);
    while (circuitFile.fail()) {
        cerr << "The file \"" << filename << "\" failed to open.\n\n";
        exit(EXIT_FAILURE);
    }

    
    string line;  //to store line from the circuit file
     vector<vector<int>> val;
     vector<int> inputs;
    vector<int> innumbers;  //to store fanin
    vector<int> outnumbers;  //to store fanout
    while (getline(circuitFile, line)) {
        inputs.clear();
        if (line.find('=') != string::npos) {
            stringstream ss(line);
            int nodeNumber;  //toget node number from line
            string d;        //to get the delimiter =
            string gatet;    //to get the gate type

            ss >> nodeNumber >> d >> gatet;
            transform(gatet.begin(), gatet.end(), gatet.begin(), ::toupper);
            Gate gate;       //create instance object of the Gate structure
            gate.nodeno = nodeNumber;   //populate node no in structre
            gate.type = gatet;          // populate ngate type in structre

            string inputsStr;
            getline(ss, inputsStr, '=');
            // Remove leading and trailing whitespaces from inputsStr
            inputsStr.erase(remove_if(inputsStr.begin(), inputsStr.end(), ::isspace), inputsStr.end());

            // Find the positions of opening and closing parentheses
            size_t openParenPos = inputsStr.find('(');
            size_t closeParenPos = inputsStr.find(')');

            // Extract values between parentheses
            string valuesStr = inputsStr.substr(openParenPos + 1, closeParenPos - openParenPos - 1);

            // Create a stringstream to extract individual values
            stringstream valuesStream(valuesStr);
            string value;
            int inputNumber;
            while (getline(valuesStream, value, ',')) {
                stringstream valuesStreamn(value);
           
                while (valuesStreamn >> inputNumber) {
                    //cout << "Extracted input value: " << inputNumber;
                    gate.inputvalue.push_back(inputNumber);

                }

            }
            gates.push_back(gate);

        }// to store the fanin of gates
        else if (line.find('IN') != string::npos) {
            stringstream ssin(line);
  
            char openingParenthesis, closingParenthesis;
            int innumber;
            string word_input;
            ssin >> word_input;
            if (word_input.compare("INPUT") == 0) {
                // Extract numbers between parentheses
                ssin >> openingParenthesis >> innumber >> closingParenthesis;
                //cout << innumber;
                innumbers.push_back(innumber);
            }
        }// to store the fanout for gates which is optional 
        else if (line.find('OU') != string::npos) {
            stringstream ssin(line);
            
            char openingParenthesis, closingParenthesis;
            int outnumber;
            string word_input;
            ssin >> word_input;
            if (word_input.compare("OUTPUT") == 0) {
                // Extract numbers between parentheses
                ssin >> openingParenthesis >> outnumber >> closingParenthesis;
                //cout << outnumber;
                outnumbers.push_back(outnumber);
            }
        }

    }

    // Process the specified node numbers
    for (int i = 3; i < argc; i++) {
        int targetValue = stoi(argv[i]);
        int nodeNumbers = stoi(argv[i]);
        // Find the gate with the specified node number
        auto it = find_if(gates.begin(), gates.end(), [nodeNumbers](const Gate& gate) {
            return gate.nodeno == nodeNumbers;
            });

        // Check if the input no was from fanin vector
        for (int number : innumbers) {
            if (targetValue == number)
            {
                cout << targetValue << " " << "INPUT" << endl;
            }
        }// Check if the input no was from fanout vector optional
        for (int number : outnumbers) {
            if (targetValue == number)
            {
                cout << targetValue << " " << "OUTPUT(optional)" << endl;
            }

        }
        if (it != gates.end()) {
            Gate& gate = *it;
            int firstInput = (gate.inputvalue.size() > 0) ? gate.inputvalue[0] : -1;
            int lastInput = (gate.inputvalue.size() > 1) ? gate.inputvalue.back() : -1; // only if multiple values then print all else if only single value then -1
            
            
            cout << gate.nodeno << " " << gate.type << " " << firstInput << " " << lastInput << " " ;
            //to print the slew values
            if (gate.type == "NAND")
            {

                cout << map3[0][2][1] << endl;
            }
            else if (gate.type == "NOR")
            {

                cout << map3[1][2][1] << endl;
            }

            else if (gate.type == "AND")
            {
               
                cout << map3[2][2][1] << endl;
            }

            else if (gate.type == "OR")
            {
                
                cout << map3[3][2][1] << endl;
            }

            else if (gate.type == "XOR")
            {
                
                cout << map3[4][2][1] << endl;
            }

            else if (gate.type == "INV")
            {
               
                cout << map3[5][2][1] << endl;
            }

            else if (gate.type == "BUF")
            {
                
                cout << map3[6][2][1] << endl;
            }
            else if (gate.type == "NOT")
            {
                
                cout << map3[7][2][1] << endl;
            }
            else if (gate.type == "BUFF")
            {
                
                cout << map3[8][2][1] << endl;
            }
            else if (gate.type == "DFF")
            {
                double z = 0;
                cout << z << endl;
            }
            
        }
       
        
    }

    return 0;


}


