#include "array.cpp"
#include <iostream>
#include <limits>
#include <chrono>
#include <iomanip>
using namespace std;

// Function declarations
int calculateCompatibility(const Job& job, const Resume& resume);
void findCandidatesForJob(const Job& job, const Array<Resume>& resumeStorage, int maxResults = 10);
int countWords(const string& text);
bool runDataCleaning();

// Function to calculate compatibility score between job and resume
int calculateCompatibility(const Job& job, const Resume& resume) {
    int score = 0;
    
    // Normalize skills for comparison
    string jobSkills = job.getSkills();
    string resumeSkills = resume.getSkills();
    
    // Convert to lowercase for comparison
    transform(jobSkills.begin(), jobSkills.end(), jobSkills.begin(), ::tolower);
    transform(resumeSkills.begin(), resumeSkills.end(), resumeSkills.begin(), ::tolower);
    
    // Count matching skills
    istringstream jobStream(jobSkills);
    string skill;
    while (jobStream >> skill) {
        if (resumeSkills.find(skill) != string::npos) {
            score += 5; // Each matching skill adds 5 points
        }
    }
    
    // Bonus for exact job title match in resume
    string jobTitle = job.title;
    transform(jobTitle.begin(), jobTitle.end(), jobTitle.begin(), ::tolower);
    if (resume.getText().find(jobTitle) != string::npos) {
        score += 10; // Bonus for title match
    }
    
    return score;
}

// Function to find best candidates for a specific job
void findCandidatesForJob(const Job& job, const Array<Resume>& resumeStorage, int maxResults) {
    cout << "\n=== Top " << min(maxResults, resumeStorage.getSize()) << " Candidates for '" << job.title << "' ===" << endl;
    cout << "Required Skills: " << job.skills << endl;
    cout << "==========================================\n";
    
    // Create array to store candidate scores and indices
    struct CandidateMatch {
        int index;
        int score;
        int skillMatches;
        string matchedSkills;
    };
    
    CandidateMatch* candidates = new CandidateMatch[resumeStorage.getSize()];
    int candidateCount = 0;
    
    // Calculate compatibility scores for all candidates
    for (int i = 0; i < resumeStorage.getSize(); i++) {
        Resume candidate = resumeStorage.getItem(i);
        int score = calculateCompatibility(job, candidate);
        
        if (score > 0) {
            candidates[candidateCount].index = i;
            candidates[candidateCount].score = score;
            
            // Count and list matched skills
            string jobSkills = job.getSkills();
            string candidateSkills = candidate.getSkills();
            
            // Convert to lowercase for comparison
            transform(jobSkills.begin(), jobSkills.end(), jobSkills.begin(), ::tolower);
            transform(candidateSkills.begin(), candidateSkills.end(), candidateSkills.begin(), ::tolower);
            
            int skillMatches = 0;
            string matchedSkills = "";
            bool first = true;
            
            istringstream jobStream(jobSkills);
            string skill;
            while (jobStream >> skill) {
                if (candidateSkills.find(skill) != string::npos) {
                    skillMatches++;
                    if (!first) matchedSkills += ", ";
                    matchedSkills += skill;
                    first = false;
                }
            }
            
            candidates[candidateCount].skillMatches = skillMatches;
            candidates[candidateCount].matchedSkills = matchedSkills;
            candidateCount++;
        }
    }
    
    // Sort candidates by score (simple bubble sort)
    for (int i = 0; i < candidateCount - 1; i++) {
        for (int j = 0; j < candidateCount - i - 1; j++) {
            if (candidates[j].score < candidates[j + 1].score) {
                CandidateMatch temp = candidates[j];
                candidates[j] = candidates[j + 1];
                candidates[j + 1] = temp;
            }
        }
    }
    
    // Display top candidates
    int displayCount = min(maxResults, candidateCount);
    
    if (displayCount > 100) {
        cout << "Displaying " << displayCount << " candidates (this may take a moment)..." << endl;
    }
    
    for (int i = 0; i < displayCount; i++) {
        Resume candidate = resumeStorage.getItem(candidates[i].index);
        
        cout << "\nCandidate " << (i + 1) << " (Score: " << candidates[i].score << "):" << endl;
        cout << "ID: " << candidates[i].index << endl;
        cout << "Skills: " << candidate.skills << endl;
        cout << "Matched Skills: " << candidates[i].matchedSkills << endl;
        cout << "Skill Matches: " << candidates[i].skillMatches << " out of " << 
                (job.skills.empty() ? 0 : countWords(job.skills)) << " required skills" << endl;
        cout << "Summary: " << candidate.summary.substr(0, 80) << "..." << endl;
        cout << "----------------------------------------" << endl;
        
        // Show progress for large displays
        if (displayCount > 100 && (i + 1) % 100 == 0) {
            cout << "\n[Progress: " << (i + 1) << "/" << displayCount << " candidates displayed]" << endl;
        }
    }
    
    if (candidateCount == 0) {
        cout << "No suitable candidates found for this job description." << endl;
    } else {
        cout << "\nFound " << candidateCount << " total candidates with matching skills." << endl;
    }
    
    delete[] candidates;
}

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
        cout << "1. Search Jobs by Skills\n";
        cout << "2. Search Resumes by Skills\n";
        cout << "3. Filter Resumes with Specific Job\n";
        cout << "4. Show Best Matches for Each Job\n";
        cout << "5. Clean Data (Regenerate Cleaned CSVs)\n";
        cout << "6. Exit\n";
        cout << "-----------------------------------------\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear buffer

        switch (choice) {
            case 1: {
                cout << "\nEnter skills to search in Jobs: ";
                getline(cin, keyword);
                cout << "\n=== Job Search Results ===" << endl;
                jobStorage.displayMatches(keyword, 10000); // Show all matches
                break;
            }

            case 2: {
                cout << "\nEnter skills to search in Resumes: ";
                getline(cin, keyword);
                cout << "\n=== Resume Search Results ===" << endl;
                resumeStorage.displayMatches(keyword, 10000); // Show all matches
                break;
            }

            case 3: {
                cout << "\n=== Filter Resumes with Specific Job ===" << endl;
                cout << "Enter the job description you want to match candidates for:" << endl;
                cout << "Example: 'Software Engineer needed with experience in Python, Java, REST APIs, Docker'" << endl;
                cout << "Job Description: ";
                getline(cin, keyword);
                
                if (keyword.empty()) {
                    cout << "No job description entered. Returning to menu.\n";
                    break;
                }
                
                // Parse the entered job description
                Job userJob(keyword);
                    // Try to extract technical skills using the Job helper. If none are found,
                    // fall back to showing the raw input so the user can still search by their
                    // full description. Also display both filtered and raw for transparency.
                    string rawSkills = keyword;
                    string filteredSkills = userJob.filterTechnicalSkills(rawSkills);
                    if (filteredSkills != "Not specified") {
                        userJob.skills = filteredSkills;
                    } else {
                        // If filtering didn't find anything, use the raw text so we "show all"
                        userJob.skills = rawSkills;
                    }

                    cout << "\nParsed Job Details:" << endl;
                    cout << "Title: " << userJob.title << endl;
                    cout << "Required Skills (filtered): " << (filteredSkills.empty() ? "Not specified" : filteredSkills) << endl;
                    if (filteredSkills == "Not specified") {
                        cout << "Required Skills (raw): " << rawSkills << endl;
                    }
                cout << "\nSearching for matching candidates...\n";
                
                int maxResults = 10000; // Show all matches

                // Find best matching candidates
                findCandidatesForJob(userJob, resumeStorage, maxResults);
                break;
            }

            case 4: {
                cout << "\n=== Best Matches for Each Job ===" << endl;
                
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
                
                cout << "\nAnalyzing compatibility between jobs and resumes...\n";
                cout << "This may take a moment for " << maxJobsToShow << " jobs...\n";
                
                // Start timing
                auto startTime = chrono::high_resolution_clock::now();
                
                int maxResumes = min(100, resumeStorage.getSize()); // Use more resumes for better matching
                
                // Create array to store all job matches with scores
                struct JobMatch {
                    int jobIndex;
                    int resumeIndex;
                    int score;
                };
                
                JobMatch* allMatches = new JobMatch[jobStorage.getSize()];
                int matchCount = 0;
                
                // Calculate best match for each job
                for (int i = 0; i < jobStorage.getSize(); i++) {
                    Job currentJob = jobStorage.getItem(i);
                    int bestMatch = -1;
                    int bestScore = 0;
                    
                    // Find best matching resume for this job
                    for (int j = 0; j < maxResumes; j++) {
                        Resume currentResume = resumeStorage.getItem(j);
                        int score = calculateCompatibility(currentJob, currentResume);
                        if (score > bestScore) {
                            bestScore = score;
                            bestMatch = j;
                        }
                    }
                    
                    if (bestMatch != -1 && bestScore > 0) {
                        allMatches[matchCount].jobIndex = i;
                        allMatches[matchCount].resumeIndex = bestMatch;
                        allMatches[matchCount].score = bestScore;
                        matchCount++;
                    }
                    
                    // Show progress every 1000 jobs
                    if ((i + 1) % 1000 == 0) {
                        cout << "[Progress: " << (i + 1) << "/" << jobStorage.getSize() << " jobs processed]\n";
                    }
                }
                
                // Sort matches by score (descending)
                for (int i = 0; i < matchCount - 1; i++) {
                    for (int j = i + 1; j < matchCount; j++) {
                        if (allMatches[j].score > allMatches[i].score) {
                            JobMatch temp = allMatches[i];
                            allMatches[i] = allMatches[j];
                            allMatches[j] = temp;
                        }
                    }
                }
                
                // Display top matches
                cout << "\n=== Top " << min(maxJobsToShow, matchCount) << " Job-Resume Matches ===\n";
                cout << "==========================================\n";
                
                for (int i = 0; i < min(maxJobsToShow, matchCount); i++) {
                    Job currentJob = jobStorage.getItem(allMatches[i].jobIndex);
                    Resume currentResume = resumeStorage.getItem(allMatches[i].resumeIndex);
                    
                    cout << "\nRank #" << (i + 1) << " - Score: " << allMatches[i].score << endl;
                    cout << "Job ID: " << allMatches[i].jobIndex << " - " << currentJob.title << endl;
                    cout << "Job Skills: " << currentJob.skills << endl;
                    cout << "Resume ID: " << allMatches[i].resumeIndex << endl;
                    cout << "Resume Skills: " << currentResume.skills << endl;
                    cout << "----------------------------------------" << endl;
                }
                
                // End timing
                auto endTime = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
                
                cout << "\nCompleted analysis! Processed " << jobStorage.getSize() << " jobs, found " << matchCount << " matches." << endl;
                cout << "Displayed top " << min(maxJobsToShow, matchCount) << " matches." << endl;
                cout << "Processing time: " << duration.count() << " ms (" 
                     << fixed << setprecision(2) << duration.count() / 1000.0 << " seconds)" << endl;
                
                delete[] allMatches;
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
