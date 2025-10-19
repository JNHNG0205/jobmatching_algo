#include "array.cpp"
#include <iostream>
#include <limits>
#include <chrono>
#include <iomanip>
using namespace std;

// Function declarations
int countWords(const string& text);
bool runDataCleaning();


// Helper function to count words in a string
int countWords(const string& text) {
    istringstream iss(text);
    string word;
    int count = 0;
    while (iss >> word) {
        count++;
    }
    return count;
}

// Function to run data cleaning process
bool runDataCleaning() {
    cout << "\nStarting data cleaning process..." << endl;
    
    // Check if data_cleaning executable exists
    ifstream check("data_cleaning");
    if (!check.good()) {
        check.close();
        // Try with .exe extension (for Windows)
        check.open("data_cleaning.exe");
        if (!check.good()) {
            cerr << "Error: data_cleaning executable not found!" << endl;
            cerr << "Please ensure data_cleaning is compiled and in the current directory." << endl;
            return false;
        }
    }
    check.close();
    
    // Run the data cleaning process
    cout << "Running data cleaning..." << endl;
    int result = system("data_cleaning");
    if (result == 0) {
        cout << "Data cleaning completed successfully!" << endl;
        return true;
    } else {
        cerr << "Data cleaning failed with exit code: " << result << endl;
        return false;
    }
}

int main() {
    Array<Job> jobStorage(100);
    Array<Resume> resumeStorage(100);

    cout << "=========================================\n";
    cout << "   Job Matching System (Rule-Based)\n";
    cout << "   Using Custom Array Data Structures\n";
    cout << "=========================================\n";

    // ===== Step 1: Auto-load datasets =====
    string jobPath = "./data/job_description_clean.csv";
    string resumePath = "./data/resume_clean.csv";

    cout << "\nLoading job and resume datasets...\n";

    bool jobLoaded = jobStorage.loadFromCSV(jobPath);
    bool resumeLoaded = resumeStorage.loadFromCSV(resumePath);

    if (!jobLoaded || !resumeLoaded) {
        cerr << "\nError: Failed to load one or more datasets.\n";
        cerr << "Please ensure the CSV files exist in ./data/ folder.\n";
        return 1;
    }

    cout << "\nSuccessfully loaded datasets!\n";
    cout << "Jobs loaded: " << jobStorage.getSize() << endl;
    cout << "Resumes loaded: " << resumeStorage.getSize() << endl;

    // ===== Step 2: Interactive Menu =====
    int choice;
    string keyword;

    do {
        cout << "\n-----------------------------------------\n";
        cout << "Choose an action:\n";
        cout << "1. Search Jobs by Job Title\n";
        cout << "2. Search Jobs by Skills\n";
        cout << "3. Search Resumes by Skills\n";
        cout << "4. Show Best Matches for Each Job\n";
        cout << "5. Clean Data (Regenerate Cleaned CSVs)\n";
        cout << "6. Exit\n";
        cout << "-----------------------------------------\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear buffer

        switch (choice) {
            case 1: {
                cout << "\nEnter job title to search for: ";
                getline(cin, keyword);
                
                // Submenu for selecting number of results to display
                int displayOption;
                cout << "\nSelect number of results to display:\n";
                cout << "1. Top 10 results\n";
                cout << "2. Top 50 results\n";
                cout << "3. Top 100 results\n";
                cout << "4. All results\n";
                cout << "Enter your choice (1-4): ";
                cin >> displayOption;
                
                if (cin.fail() || displayOption < 1 || displayOption > 4) {
                    cout << "Invalid choice. Showing top 10 results by default.\n";
                    displayOption = 1;
                }
                
                int maxResults;
                switch (displayOption) {
                    case 1: maxResults = 10; break;
                    case 2: maxResults = 50; break;
                    case 3: maxResults = 100; break;
                    case 4: maxResults = 10000; break;
                    default: maxResults = 10; break;
                }
                
                cout << "\n=== Job Title Search Results ===" << endl;
                jobStorage.displayMatchesByTitle(keyword, maxResults);
                break;
            }

            case 2: {
                cout << "\nEnter skills to search in Jobs: ";
                getline(cin, keyword);
                
                // Submenu for selecting number of results to display
                int displayOption;
                cout << "\nSelect number of results to display:\n";
                cout << "1. Top 10 results\n";
                cout << "2. Top 50 results\n";
                cout << "3. Top 100 results\n";
                cout << "4. All results\n";
                cout << "Enter your choice (1-4): ";
                cin >> displayOption;
                
                if (cin.fail() || displayOption < 1 || displayOption > 4) {
                    cout << "Invalid choice. Showing top 10 results by default.\n";
                    displayOption = 1;
                }
                
                int maxResults;
                switch (displayOption) {
                    case 1: maxResults = 10; break;
                    case 2: maxResults = 50; break;
                    case 3: maxResults = 100; break;
                    case 4: maxResults = 10000; break;
                    default: maxResults = 10; break;
                }
                
                cout << "\n=== Job Search Results ===" << endl;
                jobStorage.displayMatches(keyword, maxResults);
                break;
            }

            case 3: {
                cout << "\nEnter skills to search in Resumes: ";
                getline(cin, keyword);
                
                // Submenu for selecting number of results to display
                int displayOption;
                cout << "\nSelect number of results to display:\n";
                cout << "1. Top 10 results\n";
                cout << "2. Top 50 results\n";
                cout << "3. Top 100 results\n";
                cout << "4. All results\n";
                cout << "Enter your choice (1-4): ";
                cin >> displayOption;
                
                if (cin.fail() || displayOption < 1 || displayOption > 4) {
                    cout << "Invalid choice. Showing top 10 results by default.\n";
                    displayOption = 1;
                }
                
                int maxResults;
                switch (displayOption) {
                    case 1: maxResults = 10; break;
                    case 2: maxResults = 50; break;
                    case 3: maxResults = 100; break;
                    case 4: maxResults = 10000; break;
                    default: maxResults = 10; break;
                }
                
                cout << "\n=== Resume Search Results ===" << endl;
                resumeStorage.displayMatches(keyword, maxResults);
                break;
            }

            case 4: {
                cout << "\n=== Best Matches for Each Job (Optimized) ===" << endl;
                
                // Submenu for selecting number of matches to display
                int displayOption;
                cout << "\nSelect number of top matches to display:\n";
                cout << "1. Top 10 matches\n";
                cout << "2. Top 50 matches\n";
                cout << "3. Top 100 matches\n";
                cout << "4. All matches\n";
                cout << "Enter your choice (1-4): ";
                cin >> displayOption;
                
                if (cin.fail() || displayOption < 1 || displayOption > 4) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid choice. Defaulting to Top 10 matches.\n";
                    displayOption = 1;
                }
                
                int maxJobsToShow;
                switch(displayOption) {
                    case 1: maxJobsToShow = 10; break;
                    case 2: maxJobsToShow = 50; break;
                    case 3: maxJobsToShow = 100; break;
                    case 4: maxJobsToShow = jobStorage.getSize(); break;
                    default: maxJobsToShow = 10;
                }
                
                maxJobsToShow = min(maxJobsToShow, jobStorage.getSize());
                
                // Use the new optimized function with all advanced algorithms
                jobStorage.findBestMatchesForJobs(resumeStorage, maxJobsToShow);
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
                        cout << "Please restart the program to load the updated cleaned data." << endl;
                    } else {
                        cout << "\nData cleaning failed. Check error messages above." << endl;
                    }
                } else {
                    cout << "Data cleaning cancelled." << endl;
                }
                break;
            }

            case 6:
                cout << "\nExiting program...\n";
                break;

            default:
                cout << "Invalid choice. Please enter a valid option.\n";
        }

    } while (choice != 6);

    return 0;
}
