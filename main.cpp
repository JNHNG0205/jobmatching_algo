#include "array.cpp"
#include <iostream>
#include <limits>
using namespace std;

// Function declarations
int calculateCompatibility(const Job& job, const Resume& resume);
void findCandidatesForJob(const Job& job, const Array<Resume>& resumeStorage, int maxResults = 10);
int countWords(const string& text);
int calculateTitleMatchScore(const string& jobTitle, const Resume& resume);
void findCandidatesByJobTitle(const string& jobTitle, const Array<Resume>& resumeStorage, int maxResults = 10000);
int calculateJobTitleMatchScore(const string& jobTitle, const Job& job);
void findJobsByJobTitle(const string& jobTitle, const Array<Job>& jobStorage, int maxResults = 10000);

// Function to calculate compatibility score between job and resume
int calculateCompatibility(const Job& job, const Resume& resume) {
    int score = 0;
    
    // Normalize skills for comparison
    string jobSkills = job.getSkills();
    string resumeSkills = resume.getSkills();
    
    // Convert to lowercase for comparison
    transform(jobSkills.begin(), jobSkills.end(), jobSkills.begin(), ::tolower);
    transform(resumeSkills.begin(), resumeSkills.end(), resumeSkills.begin(), ::tolower);
    
    // Count matching skills (split by comma; trim spaces/punctuation)
    {
        istringstream jobStream(jobSkills);
        string token;
        while (getline(jobStream, token, ',')) {
            // Trim whitespace
            token.erase(0, token.find_first_not_of(" \t"));
            if (!token.empty()) token.erase(token.find_last_not_of(" \t") + 1);
            // Remove trailing punctuation
            while (!token.empty() && (token.back() == '.' || token.back() == ';' || token.back() == ':')) {
                token.pop_back();
            }
            // Lowercase already applied to jobSkills; ensure token lower as well
            string tokenLower = token;
            transform(tokenLower.begin(), tokenLower.end(), tokenLower.begin(), ::tolower);
            if (!tokenLower.empty() && resumeSkills.find(tokenLower) != string::npos) {
                score += 5; // Each matching skill adds 5 points
            }
        }
    }
    
    // Bonus for job title match in resume (case-insensitive)
    string jobTitle = job.title;
    transform(jobTitle.begin(), jobTitle.end(), jobTitle.begin(), ::tolower);
    string resumeText = resume.getText();
    transform(resumeText.begin(), resumeText.end(), resumeText.begin(), ::tolower);
    if (resumeText.find(jobTitle) != string::npos) {
        score += 10; // Bonus for title match
    }
    
    return score;
}

// Title-only match scoring (ignores technical skills)
int calculateTitleMatchScore(const string& jobTitle, const Resume& resume) {
    string title = jobTitle;
    transform(title.begin(), title.end(), title.begin(), ::tolower);
    // Build a text haystack from multiple resume fields for better coverage
    string hay = resume.summary + " " + resume.experience + " " + resume.education;
    transform(hay.begin(), hay.end(), hay.begin(), ::tolower);

    // Trim simple surrounding spaces from title
    if (!title.empty()) {
        title.erase(0, title.find_first_not_of(" \t"));
        if (!title.empty()) title.erase(title.find_last_not_of(" \t") + 1);
    }

    if (title.empty() || hay.empty()) return 0;

    int score = 0;
    // Strong bonus if the full job title appears in the resume text
    if (hay.find(title) != string::npos) {
        score += 20;
    }

    // Additional points for occurrences of meaningful title words (length >= 3)
    istringstream iss(title);
    string word;
    while (iss >> word) {
        if (word.size() >= 3 && hay.find(word) != string::npos) {
            score += 3;
        }
    }

    return score;
}

// Find and display candidates ranked purely by job title relevance
void findCandidatesByJobTitle(const string& jobTitle, const Array<Resume>& resumeStorage, int maxResults) {
    struct CandidateTitleMatch {
        int index;
        int score;
    };

    int total = resumeStorage.getSize();
    if (total == 0) {
        cout << "No resumes available to search." << endl;
        return;
    }

    CandidateTitleMatch* matches = new CandidateTitleMatch[total];
    int count = 0;

    for (int i = 0; i < total; i++) {
        Resume candidate = resumeStorage.getItem(i);
        int score = calculateTitleMatchScore(jobTitle, candidate);
        if (score > 0) {
            matches[count].index = i;
            matches[count].score = score;
            count++;
        }
    }

    // Sort by score descending (simple bubble sort to stay consistent with rest of code)
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (matches[j].score < matches[j + 1].score) {
                CandidateTitleMatch tmp = matches[j];
                matches[j] = matches[j + 1];
                matches[j + 1] = tmp;
            }
        }
    }

    if (count == 0) {
        cout << "No candidates matched the job title." << endl;
        delete[] matches;
        return;
    }

    int displayCount = count < maxResults ? count : maxResults;
    cout << "\n=== Top " << displayCount << " Candidates by Title Match ===" << endl;

    for (int i = 0; i < displayCount; i++) {
        Resume candidate = resumeStorage.getItem(matches[i].index);
        cout << "\nCandidate " << (i + 1) << " (Title Score: " << matches[i].score << "):" << endl;
        cout << "ID: " << matches[i].index << endl;
        cout << "Summary: " << candidate.summary.substr(0, 120) << "..." << endl;
        cout << "----------------------------------------" << endl;
    }

    cout << "\nFound " << count << " total candidates matching the title." << endl;

    delete[] matches;
}

// Title-only scoring for jobs (match provided title against job.title and job.description)
int calculateJobTitleMatchScore(const string& jobTitle, const Job& job) {
    string title = jobTitle;
    transform(title.begin(), title.end(), title.begin(), ::tolower);

    string hay = job.title + " " + job.description;
    transform(hay.begin(), hay.end(), hay.begin(), ::tolower);

    if (!title.empty()) {
        title.erase(0, title.find_first_not_of(" \t"));
        if (!title.empty()) title.erase(title.find_last_not_of(" \t") + 1);
    }

    if (title.empty() || hay.empty()) return 0;

    // Prefer exact phrase match
    if (hay.find(title) != string::npos) {
        return 100; // strong match
    }

    // Require all meaningful words (len >= 3) to appear; otherwise, reject
    istringstream iss(title);
    string word;
    int totalMeaningful = 0;
    int foundMeaningful = 0;
    while (iss >> word) {
        if (word.size() >= 3) {
            totalMeaningful++;
            if (hay.find(word) != string::npos) {
                foundMeaningful++;
            }
        }
    }

    if (totalMeaningful > 0 && foundMeaningful == totalMeaningful) {
        // All key words found (e.g., "software" and "engineer")
        return 60;
    }

    return 0; // too weak (e.g., only "engineer" matches)
}

// Find and display jobs ranked purely by provided job title relevance
void findJobsByJobTitle(const string& jobTitle, const Array<Job>& jobStorage, int maxResults) {
    struct JobTitleMatch { int index; int score; };
    int total = jobStorage.getSize();
    if (total == 0) {
        cout << "No jobs available to search." << endl;
        return;
    }

    JobTitleMatch* matches = new JobTitleMatch[total];
    int count = 0;

    for (int i = 0; i < total; i++) {
        Job job = jobStorage.getItem(i);
        int score = calculateJobTitleMatchScore(jobTitle, job);
        if (score > 0) {
            matches[count].index = i;
            matches[count].score = score;
            count++;
        }
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (matches[j].score < matches[j + 1].score) {
                JobTitleMatch tmp = matches[j];
                matches[j] = matches[j + 1];
                matches[j + 1] = tmp;
            }
        }
    }

    if (count == 0) {
        cout << "No jobs matched the given title." << endl;
        delete[] matches;
        return;
    }

    int displayCount = count < maxResults ? count : maxResults;
    cout << "\n=== Top " << displayCount << " Jobs by Title Match ===" << endl;

    for (int i = 0; i < displayCount; i++) {
        Job job = jobStorage.getItem(matches[i].index);
        cout << "\nJob " << (i + 1) << " (Title Score: " << matches[i].score << "):" << endl;
        cout << "ID: " << matches[i].index << endl;
        cout << "Title: " << job.title << endl;
        cout << "Skills: " << job.skills << endl;
        cout << "Description: " << job.description.substr(0, 120) << "..." << endl;
        cout << "----------------------------------------" << endl;
    }

    cout << "\nFound " << count << " total jobs matching the title." << endl;

    delete[] matches;
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
            
            // Build matched skills list by splitting job skills on commas and trimming
            {
                istringstream jobStream(jobSkills);
                string token;
                while (getline(jobStream, token, ',')) {
                    // Trim whitespace
                    token.erase(0, token.find_first_not_of(" \t"));
                    if (!token.empty()) token.erase(token.find_last_not_of(" \t") + 1);
                    // Remove trailing punctuation
                    while (!token.empty() && (token.back() == '.' || token.back() == ';' || token.back() == ':' || token.back() == ',')) {
                        token.pop_back();
                        if (!token.empty()) {
                            // trim spaces again if any
                            token.erase(token.find_last_not_of(" \t") + 1);
                        }
                    }
                    if (token.empty()) continue;
                    if (candidateSkills.find(token) != string::npos) {
                        skillMatches++;
                        if (!first) matchedSkills += ", ";
                        matchedSkills += token;
                        first = false;
                    }
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
    
    // If there are no candidates, report and return safely
    if (candidateCount == 0) {
        cout << "No suitable candidates found for this job description." << endl;
        delete[] candidates;
        return;
    }

    // Display top candidates (limit to available results)
    int displayCount = candidateCount < 10 ? candidateCount : 10;

    if (displayCount > 100) {
        cout << "Displaying " << displayCount << " candidates (this may take a moment)..." << endl;
    }

    for (int i = 0; i < displayCount; i++) {
        Resume candidate = resumeStorage.getItem(candidates[i].index);

        cout << "\nCandidate " << (i + 1) << " (Score: " << candidates[i].score << "):" << endl;
        cout << "ID: " << candidates[i].index << endl;
        cout << "Skills: " << candidate.skills << endl;
        cout << "Matched Skills: " << candidates[i].matchedSkills << endl;
        // Count required skills by commas (trim empty tokens)
        int required = 0; {
            istringstream req(job.skills);
            string token; while (getline(req, token, ',')) {
                token.erase(0, token.find_first_not_of(" \t"));
                if (!token.empty()) token.erase(token.find_last_not_of(" \t") + 1);
                while (!token.empty() && (token.back() == '.' || token.back() == ';' || token.back() == ':' || token.back() == ',')) {
                    token.pop_back();
                    if (!token.empty()) token.erase(token.find_last_not_of(" \t") + 1);
                }
                if (!token.empty()) required++;
            }
        }
        cout << "Skill Matches: " << candidates[i].skillMatches << " out of " << required << " required skills" << endl;
        cout << "Summary: " << candidate.summary.substr(0, 80) << "..." << endl;
        cout << "----------------------------------------" << endl;

        // Show progress for large displays
        if (displayCount > 100 && (i + 1) % 100 == 0) {
            cout << "\n[Progress: " << (i + 1) << "/" << displayCount << " candidates displayed]" << endl;
        }
    }

    cout << "\nFound " << candidateCount << " total candidates with matching skills." << endl;

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
        cout << "2. Search Employees by Skills\n";
        cout << "3. Filter Jobs with Specific Job\n";
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
                {
                    // Filter to technical skills before searching
                    Job helper;
                    string filtered = helper.filterTechnicalSkills(keyword);
                    if (filtered == "Not specified") {
                        cout << "\nNo technical skills detected in input. Please enter technical skills like 'Python, SQL, Docker'.\n";
                        break;
                    }
                    cout << "\n=== Job Search Results ===" << endl;
                    jobStorage.displayMatches(filtered, 10000); // Show all matches
                }
                break;
            }

            case 2: {
                cout << "\nEnter skills to search in Resumes: ";
                getline(cin, keyword);
                {
                    // Filter to technical skills before searching
                    Job helper;
                    string filtered = helper.filterTechnicalSkills(keyword);
                    if (filtered == "Not specified") {
                        cout << "\nNo technical skills detected in input. Please enter technical skills like 'Python, SQL, Docker'.\n";
                        break;
                    }
                    cout << "\n=== Resume Search Results ===" << endl;
                    resumeStorage.displayMatches(filtered, 10000); // Show all matches
                }
                break;
            }

            case 3: {
                cout << "\n=== Filter Resumes by Job Title (Title-only search) ===" << endl;
                cout << "Enter the job title to match candidates for:" << endl;
                cout << "- Example: Software Engineer, Data Analyst" << endl;
                cout << "Job Title: ";
                getline(cin, keyword);

                if (keyword.empty()) {
                    cout << "No job title entered. Returning to menu.\n";
                    break;
                }

                cout << "\nSearching for jobs with title matching: '" << keyword << "' (ignoring skills)...\n";

                int maxResults = 10000; // Show all matches

                // Find matching jobs by title only
                findJobsByJobTitle(keyword, jobStorage, maxResults);
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
                return 0;
        }

    } while (choice != 5);

    return 0;
}
