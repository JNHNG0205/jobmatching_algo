#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "array.cpp"

using namespace std;

// Helper function to trim whitespace
static inline string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

// Helper function to normalize job titles
static inline string normalizeTitle(const string &raw) {
    string t = trim(raw);
    // Strip leading/trailing quotes repeatedly
    while (!t.empty() && (t.front() == '"' || t.front() == '\'')) t.erase(t.begin());
    while (!t.empty() && (t.back() == '"' || t.back() == '\'')) t.pop_back();
    // Strip trailing punctuation commonly leaking from sentence boundaries
    while (!t.empty() && (t.back() == '.' || t.back() == ',' || t.back() == ';' || t.back() == ':')) t.pop_back();
    // Trim again after removals
    t = trim(t);
    return t;
}

// Helper function to escape CSV fields
static inline string csvEscape(const string &s) {
    if (s.find(',') != string::npos || s.find('"') != string::npos || s.find('\n') != string::npos) {
        string escaped = "\"";
        for (char c : s) {
            if (c == '"') escaped += "\"\"";
            else escaped += c;
        }
        escaped += "\"";
        return escaped;
    }
    return s;
}

int main() {
    cout << "Starting data cleaning process..." << endl;
    
    // Process job descriptions
    cout << "Processing job descriptions..." << endl;
    ifstream jobFile("./data/job_description.csv");
    if (!jobFile.is_open()) {
        cerr << "Error: Cannot open job_description.csv" << endl;
        return 1;
    }
    
    ofstream out("./data/job_description_clean.csv");
    if (!out.is_open()) {
        cerr << "Error: Cannot create job_description_clean.csv" << endl;
        return 1;
    }
    
    // Write header
    out << "Job_ID,Title,Skills\n";
    
    string line;
    int jobId = 1;
    
    // Skip header line
    getline(jobFile, line);
    
    while (getline(jobFile, line)) {
        if (line.empty()) continue;
        
        // Create a Job object to use its parsing logic
        Job job;
        
        // Parse the job using the existing logic
        size_t neededPos = line.find(" needed");
        if (neededPos != string::npos) {
            job.title = line.substr(0, neededPos);
        }
        
        // Extract skills from the line
        string rawSkills = "";
        size_t expPos = line.find("experience in");
        if (expPos != string::npos) {
            rawSkills = line.substr(expPos + 13);
            // Find the end of the skills (first period or end of line)
            size_t endPos = rawSkills.find('.');
            if (endPos != string::npos) {
                rawSkills = rawSkills.substr(0, endPos);
            }
        }
        
        // Filter skills using the existing method
        job.skills = job.filterTechnicalSkills(rawSkills);
        
        // Normalize title
        string titleNorm = normalizeTitle(job.title);
        
        // Write to output
        out << jobId << "," << csvEscape(titleNorm) << "," << csvEscape(job.skills) << "\n";
        jobId++;
    }
    
    jobFile.close();
    out.close();
    
    // Atomic file replacement
    if (rename("./data/job_description_clean.csv.tmp", "./data/job_description_clean.csv") != 0) {
        // If rename fails, the file is already in place
    }
    
    cout << "Wrote cleaned jobs to: ./data/job_description_clean.csv" << endl;
    
    // Process resumes
    cout << "Processing resumes..." << endl;
    ifstream resumeFile("./data/resume.csv");
    if (!resumeFile.is_open()) {
        cerr << "Error: Cannot open resume.csv" << endl;
        return 1;
    }
    
    ofstream rout("./data/resume_clean.csv");
    if (!rout.is_open()) {
        cerr << "Error: Cannot create resume_clean.csv" << endl;
        return 1;
    }
    
    // Write header
    rout << "Resume_ID,Skills\n";
    
    int resumeId = 1;
    
    // Skip header line
    getline(resumeFile, line);
    
    while (getline(resumeFile, line)) {
        if (line.empty()) continue;
        
        // Create a Resume object to use its parsing logic
        Resume r;
        
        // Extract skills from the line
        string rawSkills = "";
        size_t skilledPos = line.find("skilled in");
        if (skilledPos != string::npos) {
            rawSkills = line.substr(skilledPos + 10);
            // Find the end of the skills (first period or end of line)
            size_t endPos = rawSkills.find('.');
            if (endPos != string::npos) {
                rawSkills = rawSkills.substr(0, endPos);
            }
        }
        
        // Filter skills using the existing method
        r.skills = r.filterTechnicalSkills(rawSkills);
        
        // Write to output
        rout << resumeId << "," << csvEscape(r.skills) << "\n";
        resumeId++;
    }
    
    resumeFile.close();
    rout.close();
    
    // Atomic file replacement
    if (rename("./data/resume_clean.csv.tmp", "./data/resume_clean.csv") != 0) {
        // If rename fails, the file is already in place
    }
    
    cout << "Wrote cleaned resumes to: ./data/resume_clean.csv" << endl;
    
    cout << "Data cleaning completed successfully!" << endl;
    return 0;
}
