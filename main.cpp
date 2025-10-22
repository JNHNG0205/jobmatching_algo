#include <iostream>
#include <limits>
#include <fstream>
#include <cstdlib>
using namespace std;

// Function to run data cleaning process
bool runDataCleaning() {
    cout << "\nStarting data cleaning process..." << endl;
    
    // Check if data_cleaning executable exists
    ifstream check("./data_cleaning");
    if (!check.good()) {
        check.close();
        check.open("./data_cleaning.exe");
        if (!check.good()) {
            cerr << "Error: data_cleaning executable not found!" << endl;
            return false;
        }
        check.close();
        int result = system("./data_cleaning.exe");
        return (result == 0);
    }
    check.close();
    
    int result = system("./data_cleaning");
    return (result == 0);
}

void runProgramWithFeature(const string& programName, int feature, int subOption = 1) {
    // Create input file for automated input
    ofstream inputFile("temp_input.txt");
    inputFile << feature << "\n";
    if (feature == 4 || feature == 1 || feature == 2 || feature == 3) {
        inputFile << subOption << "\n";
    }
    inputFile << "6\n";  // Exit after running
    inputFile.close();
    
    // Run the program with input redirection
    string command = "./" + programName + " < temp_input.txt";
    system(command.c_str());
    
    // Clean up
    system("rm -f temp_input.txt");
}

int main() {
    cout << "=========================================\n";
    cout << "   Job Matching System\n";
    cout << "   Runtime Algorithm Selection\n";
    cout << "=========================================\n";
    
    // Check if both executables exist
    bool linearExists = false, booleanExists = false;
    
    ifstream checkLinear("./program_linear");
    linearExists = checkLinear.good();
    checkLinear.close();
    
    ifstream checkBoolean("./program_boolean");
    booleanExists = checkBoolean.good();
    checkBoolean.close();
    
    // If executables don't exist, try to compile them
    if (!linearExists || !booleanExists) {
        cout << "\nCompiling required programs...\n";
        
        if (!linearExists) {
            cout << "Compiling Linear Search + Bubble Sort version...\n";
            int result = system("g++ -std=c++11 -o program_linear main_linear_bubble.cpp 2>&1");
            if (result == 0) {
                linearExists = true;
                cout << "✓ Linear version compiled successfully\n";
            } else {
                cout << "✗ Failed to compile linear version\n";
            }
        }
        
        if (!booleanExists) {
            cout << "Compiling Boolean Search + Selection Sort version...\n";
            int result = system("g++ -std=c++11 -o program_boolean main_boolean_selection.cpp 2>&1");
            if (result == 0) {
                booleanExists = true;
                cout << "✓ Boolean version compiled successfully\n";
            } else {
                cout << "✗ Failed to compile boolean version\n";
            }
        }
    }
    
    if (!linearExists && !booleanExists) {
        cerr << "\nError: Could not compile any algorithm versions!" << endl;
        cerr << "Please check your source files and try compiling manually:\n";
        cerr << "  g++ -std=c++11 -o program_linear main_linear_bubble.cpp\n";
        cerr << "  g++ -std=c++11 -o program_boolean main_boolean_selection.cpp\n";
        return 1;
    }
    
    cout << "\nAvailable algorithms:\n";
    if (linearExists) cout << "  ✓ Linear Search + Bubble Sort\n";
    if (booleanExists) cout << "  ✓ Boolean Search + Selection Sort\n";

    int choice;
    string keyword;
    
    do {
        cout << "\n-----------------------------------------\n";
        cout << "Choose an action:\n";
        cout << "1. Search Jobs by Job Title\n";
        cout << "2. Search Jobs by Skills\n";
        cout << "3. Search Resumes by Skills\n";
        cout << "4. Show Best Matches for Each Job [Choose Algorithm]\n";
        cout << "5. Clean Data (Regenerate Cleaned CSVs)\n";
        cout << "6. Exit\n";
        cout << "-----------------------------------------\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
            case 2:
            case 3: {
                int displayOption;
                cout << "\nSelect number of results to display:\n";
                cout << "1. Top 10 results\n";
                cout << "2. Top 50 results\n";
                cout << "3. Top 100 results\n";
                cout << "4. All results\n";
                cout << "Enter your choice (1-4): ";
                cin >> displayOption;
                
                if (cin.fail() || displayOption < 1 || displayOption > 4) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid choice. Using top 10 results.\n";
                    displayOption = 1;
                }
                
                cout << "\nUsing " << (booleanExists ? "Boolean Search (Optimized)" : "Linear Search") << " algorithm\n";
                
                if (choice == 1) {
                    cout << "\nEnter job title to search for: ";
                    getline(cin, keyword);
                    
                    // Create a more complex input file
                    ofstream inputFile("temp_input.txt");
                    inputFile << "1\n";  // Feature 1
                    inputFile << keyword << "\n";  // Search term
                    inputFile << displayOption << "\n";  // Display option
                    inputFile << "6\n";  // Exit
                    inputFile.close();
                    
                    string program = booleanExists ? "program_boolean" : "program_linear";
                    system(("./" + program + " < temp_input.txt").c_str());
                    system("rm -f temp_input.txt");
                    
                } else if (choice == 2) {
                    cout << "\nEnter skills to search in Jobs: ";
                    getline(cin, keyword);
                    
                    ofstream inputFile("temp_input.txt");
                    inputFile << "2\n";
                    inputFile << keyword << "\n";
                    inputFile << displayOption << "\n";
                    inputFile << "6\n";
                    inputFile.close();
                    
                    string program = booleanExists ? "program_boolean" : "program_linear";
                    system(("./" + program + " < temp_input.txt").c_str());
                    system("rm -f temp_input.txt");
                    
                } else if (choice == 3) {
                    cout << "\nEnter skills to search in Resumes: ";
                    getline(cin, keyword);
                    
                    ofstream inputFile("temp_input.txt");
                    inputFile << "3\n";
                    inputFile << keyword << "\n";
                    inputFile << displayOption << "\n";
                    inputFile << "6\n";
                    inputFile.close();
                    
                    string program = booleanExists ? "program_boolean" : "program_linear";
                    system(("./" + program + " < temp_input.txt").c_str());
                    system("rm -f temp_input.txt");
                }
                break;
            }

            case 4: {
                cout << "\n=== Best Matches for Each Job ===" << endl;
                
                // Select algorithm
                int algoChoice = 0;
                if (linearExists && booleanExists) {
                    cout << "\nSelect algorithm to use:\n";
                    cout << "1. Linear Search + Bubble Sort (Slower, Educational)\n";
                    cout << "2. Boolean Search + Selection Sort (Faster, Optimized)\n";
                    cout << "Enter your choice (1-2): ";
                    cin >> algoChoice;
                    
                    if (cin.fail() || (algoChoice != 1 && algoChoice != 2)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Invalid choice. Defaulting to Boolean Search.\n";
                        algoChoice = 2;
                    }
                } else if (linearExists) {
                    algoChoice = 1;
                    cout << "\nUsing Linear Search + Bubble Sort\n";
                } else {
                    algoChoice = 2;
                    cout << "\nUsing Boolean Search + Selection Sort\n";
                }
                
                // Select number of matches
                cout << "\nSelect number of top matches to display:\n";
                cout << "1. Top 10 matches\n";
                cout << "2. Top 50 matches\n";
                cout << "3. Top 100 matches\n";
                cout << "4. All matches\n";
                cout << "Enter your choice (1-4): ";
                int displayOption;
                cin >> displayOption;
                
                if (cin.fail() || displayOption < 1 || displayOption > 4) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid choice. Defaulting to Top 10 matches.\n";
                    displayOption = 1;
                }
                
                // Run the selected algorithm
                string program = (algoChoice == 1) ? "program_linear" : "program_boolean";
                string algoName = (algoChoice == 1) ? 
                    "Linear Search + Bubble Sort" : 
                    "Boolean Search + Selection Sort";
                
                cout << "\n>>> Running with " << algoName << " <<<\n\n";
                
                ofstream inputFile("temp_input.txt");
                inputFile << "4\n";  // Feature 4
                inputFile << displayOption << "\n";  // Display option
                inputFile << "6\n";  // Exit
                inputFile.close();
                
                system(("./" + program + " < temp_input.txt").c_str());
                system("rm -f temp_input.txt");
                
                break;
            }

            case 5: {
                cout << "\n=== Data Cleaning ===" << endl;
                cout << "This will regenerate the cleaned CSV files from the original data." << endl;
                cout << "Continue? (y/n): ";
                char confirm;
                cin >> confirm;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (confirm == 'y' || confirm == 'Y') {
                    if (runDataCleaning()) {
                        cout << "\nData cleaning completed successfully!" << endl;
                    } else {
                        cout << "\nData cleaning failed.\n";
                    }
                } else {
                    cout << "Data cleaning cancelled." << endl;
                }
                break;
            }

            case 6:
                cout << "\nExiting program...\n";
                cout << "Thank you for using the Job Matching System!\n";
                break;

            default:
                cout << "Invalid choice. Please enter a valid option.\n";
        }

    } while (choice != 6);

    return 0;
}
