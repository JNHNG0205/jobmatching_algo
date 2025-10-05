#include "array.cpp"
#include <iostream>
#include <limits>
using namespace std;

// Function declarations
int calculateCompatibility(const Job& job, const Resume& resume);
void findCandidatesForJob(const Job& job, const Array<Resume>& resumeStorage, int maxResults = 10);
int countWords(const string& text);

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

int main() {
    Array<Job> jobStorage(100);
    Array<Resume> resumeStorage(100);

    cout << "=========================================\n";
    cout << "   Job Matching System (Rule-Based)\n";
    cout << "   Using Custom Array Data Structures\n";
    cout << "=========================================\n";

    // ===== Step 1: Auto-load datasets =====
    string jobPath = "./data/job_description.csv";
    string resumePath = "./data/resume.csv";

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
        cout << "5. Exit\n";
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
                cout << "Analyzing compatibility between ALL jobs and resumes...\n";
                cout << "This may take a moment for " << jobStorage.getSize() << " jobs...\n";
                
                int maxJobs = jobStorage.getSize(); // Show ALL jobs
                int maxResumes = min(100, resumeStorage.getSize()); // Use more resumes for better matching
                
                cout << "\nJob-Resume Matches for All " << maxJobs << " Jobs:\n";
                cout << "==========================================\n";
                
                for (int i = 0; i < maxJobs; i++) {
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
                        cout << "\nJob " << (i + 1) << " (ID: " << i << ") - Best Match (Score: " << bestScore << "):" << endl;
                        cout << "Job: " << currentJob.title << endl;
                        cout << "Skills: " << currentJob.skills << endl;
                        cout << "Best Resume Match: ID " << bestMatch << endl;
                        cout << "Resume Skills: " << resumeStorage.getItem(bestMatch).skills << endl;
                        cout << "----------------------------------------" << endl;
                    }
                    
                    // Show progress every 1000 jobs
                    if ((i + 1) % 1000 == 0) {
                        cout << "\n[Progress: " << (i + 1) << "/" << maxJobs << " jobs processed]" << endl;
                    }
                }
                
                cout << "\nCompleted analysis of all " << maxJobs << " jobs!" << endl;
                break;
            }

            case 5:
                cout << "\nExiting program...\n";
                break;

            default:
                cout << "Invalid choice. Please enter a valid option.\n";
        }

    } while (choice != 5);

    return 0;
}
