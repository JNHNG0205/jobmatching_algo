#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

// Standalone structures for data cleaning (no dependencies on array implementations)

struct Job {
    string title;
    string skills;
    
    string filterTechnicalSkills(const string& rawSkills) {
        // List of technical skills to keep
        string technicalSkills[] = {
            "SQL", "Python", "Java", "JavaScript", "C++", "C#", "R", "Scala", "Go", "Rust",
            "Power BI", "Tableau", "Excel", "Pandas", "NumPy", "Matplotlib", "Seaborn",
            "Machine Learning", "Deep Learning", "NLP", "Computer Vision", "Statistics",
            "TensorFlow", "PyTorch", "Keras", "Scikit-learn", "MLOps", "ML",
            "REST APIs", "Spring Boot", "Docker", "Kubernetes", "Git", "Agile", "Scrum",
            "System Design", "Microservices", "AWS", "Azure", "GCP", "Cloud",
            "Data Cleaning", "Data Analysis", "Reporting", "ETL", "Data Pipeline",
            "Product Roadmap", "User Stories", "Stakeholder Management", "Project Management",
            "React", "Angular", "Vue", "Node.js", "Express", "Django", "Flask",
            "MongoDB", "PostgreSQL", "MySQL", "Redis", "Elasticsearch",
            "Linux", "Windows", "macOS", "Bash", "Shell", "DevOps", "CI/CD"
        };
        
        int numSkills = sizeof(technicalSkills) / sizeof(technicalSkills[0]);
        string filteredSkills = "";
        
        istringstream iss(rawSkills);
        string skill;
        bool first = true;
        
        while (getline(iss, skill, ',')) {
            // Trim whitespace
            skill.erase(0, skill.find_first_not_of(" \t"));
            skill.erase(skill.find_last_not_of(" \t") + 1);
            
            if (skill.empty()) continue;
            
            bool isTechnical = false;
            for (int i = 0; i < numSkills; i++) {
                if (skill == technicalSkills[i]) {
                    isTechnical = true;
                    break;
                }
            }
            
            if (!isTechnical) {
                string lowerSkill = skill;
                transform(lowerSkill.begin(), lowerSkill.end(), lowerSkill.begin(), ::tolower);
                
                for (int i = 0; i < numSkills; i++) {
                    string lowerTech = technicalSkills[i];
                    transform(lowerTech.begin(), lowerTech.end(), lowerTech.begin(), ::tolower);
                    
                    if (lowerSkill == lowerTech) {
                        isTechnical = true;
                        skill = technicalSkills[i];
                        break;
                    }
                }
            }
            
            if (isTechnical) {
                if (!first) filteredSkills += ", ";
                filteredSkills += skill;
                first = false;
            }
        }
        
        return filteredSkills.empty() ? "Not specified" : filteredSkills;
    }
};

struct Resume {
    string skills;
    
    string filterTechnicalSkills(const string& rawSkills) {
        // List of technical skills to keep
        string technicalSkills[] = {
            "SQL", "Python", "Java", "JavaScript", "C++", "C#", "R", "Scala", "Go", "Rust",
            "Power BI", "Tableau", "Excel", "Pandas", "NumPy", "Matplotlib", "Seaborn",
            "Machine Learning", "Deep Learning", "NLP", "Computer Vision", "Statistics",
            "TensorFlow", "PyTorch", "Keras", "Scikit-learn", "MLOps", "ML",
            "REST APIs", "Spring Boot", "Docker", "Kubernetes", "Git", "Agile", "Scrum",
            "System Design", "Microservices", "AWS", "Azure", "GCP", "Cloud",
            "Data Cleaning", "Data Analysis", "Reporting", "ETL", "Data Pipeline",
            "Product Roadmap", "User Stories", "Stakeholder Management", "Project Management",
            "React", "Angular", "Vue", "Node.js", "Express", "Django", "Flask",
            "MongoDB", "PostgreSQL", "MySQL", "Redis", "Elasticsearch",
            "Linux", "Windows", "macOS", "Bash", "Shell", "DevOps", "CI/CD"
        };
        
        int numSkills = sizeof(technicalSkills) / sizeof(technicalSkills[0]);
        string filteredSkills = "";
        
        istringstream iss(rawSkills);
        string skill;
        bool first = true;
        
        while (getline(iss, skill, ',')) {
            // Trim whitespace
            skill.erase(0, skill.find_first_not_of(" \t"));
            skill.erase(skill.find_last_not_of(" \t") + 1);
            
            if (skill.empty()) continue;
            
            bool isTechnical = false;
            for (int i = 0; i < numSkills; i++) {
                if (skill == technicalSkills[i]) {
                    isTechnical = true;
                    break;
                }
            }
            
            if (!isTechnical) {
                string lowerSkill = skill;
                transform(lowerSkill.begin(), lowerSkill.end(), lowerSkill.begin(), ::tolower);
                
                for (int i = 0; i < numSkills; i++) {
                    string lowerTech = technicalSkills[i];
                    transform(lowerTech.begin(), lowerTech.end(), lowerTech.begin(), ::tolower);
                    
                    if (lowerSkill == lowerTech) {
                        isTechnical = true;
                        skill = technicalSkills[i];
                        break;
                    }
                }
            }
            
            if (isTechnical) {
                if (!first) filteredSkills += ", ";
                filteredSkills += skill;
                first = false;
            }
        }
        
        return filteredSkills.empty() ? "Not specified" : filteredSkills;
    }
};

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
    cout << "=========================================\n";
    cout << "   Data Cleaning Utility\n";
    cout << "   Standalone Version\n";
    cout << "=========================================\n";
    cout << "\nStarting data cleaning process..." << endl;
    
    int totalJobsProcessed = 0;
    int totalResumesProcessed = 0;
    int jobsSkipped = 0;
    int resumesSkipped = 0;
    
    // Process job descriptions
    cout << "\n[1/2] Processing job descriptions..." << endl;
    ifstream jobFile("./data/job_description.csv");
    if (!jobFile.is_open()) {
        cerr << "Error: Cannot open ./data/job_description.csv" << endl;
        cerr << "Make sure the file exists in the ./data/ directory" << endl;
        return 1;
    }
    
    ofstream out("./data/job_description_clean.csv");
    if (!out.is_open()) {
        cerr << "Error: Cannot create ./data/job_description_clean.csv" << endl;
        return 1;
    }
    
    // Write header
    out << "Job_ID,Title,Skills\n";
    
    string line;
    int jobId = 1;
    
    // Skip header line
    getline(jobFile, line);
    
    while (getline(jobFile, line)) {
        if (line.empty()) {
            jobsSkipped++;
            continue;
        }
        
        Job job;
        
        // Parse the job using the existing logic
        size_t neededPos = line.find(" needed");
        if (neededPos != string::npos) {
            job.title = line.substr(0, neededPos);
        } else {
            job.title = "Unknown Position";
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
        
        // Filter skills using the method
        job.skills = job.filterTechnicalSkills(rawSkills);
        
        // Normalize title
        string titleNorm = normalizeTitle(job.title);
        
        // Write to output
        out << jobId << "," << csvEscape(titleNorm) << "," << csvEscape(job.skills) << "\n";
        jobId++;
        totalJobsProcessed++;
        
        // Progress indicator every 1000 records
        if (totalJobsProcessed % 1000 == 0) {
            cout << "  Processed " << totalJobsProcessed << " jobs..." << endl;
        }
    }
    
    jobFile.close();
    out.close();
    
    cout << "✓ Wrote cleaned jobs to: ./data/job_description_clean.csv" << endl;
    cout << "  Total jobs processed: " << totalJobsProcessed << endl;
    if (jobsSkipped > 0) {
        cout << "  Jobs skipped (empty): " << jobsSkipped << endl;
    }
    
    // Process resumes
    cout << "\n[2/2] Processing resumes..." << endl;
    ifstream resumeFile("./data/resume.csv");
    if (!resumeFile.is_open()) {
        cerr << "Error: Cannot open ./data/resume.csv" << endl;
        cerr << "Make sure the file exists in the ./data/ directory" << endl;
        return 1;
    }
    
    ofstream rout("./data/resume_clean.csv");
    if (!rout.is_open()) {
        cerr << "Error: Cannot create ./data/resume_clean.csv" << endl;
        return 1;
    }
    
    // Write header
    rout << "Resume_ID,Skills\n";
    
    int resumeId = 1;
    
    // Skip header line
    getline(resumeFile, line);
    
    while (getline(resumeFile, line)) {
        if (line.empty()) {
            resumesSkipped++;
            continue;
        }
        
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
        
        // Filter skills using the method
        r.skills = r.filterTechnicalSkills(rawSkills);
        
        // Write to output
        rout << resumeId << "," << csvEscape(r.skills) << "\n";
        resumeId++;
        totalResumesProcessed++;
        
        // Progress indicator every 1000 records
        if (totalResumesProcessed % 1000 == 0) {
            cout << "  Processed " << totalResumesProcessed << " resumes..." << endl;
        }
    }
    
    resumeFile.close();
    rout.close();
    
    cout << "✓ Wrote cleaned resumes to: ./data/resume_clean.csv" << endl;
    cout << "  Total resumes processed: " << totalResumesProcessed << endl;
    if (resumesSkipped > 0) {
        cout << "  Resumes skipped (empty): " << resumesSkipped << endl;
    }
    
    // Summary
    cout << "\n=========================================\n";
    cout << "Data cleaning completed successfully!\n";
    cout << "=========================================\n";
    cout << "Summary:\n";
    cout << "  Jobs cleaned: " << totalJobsProcessed << endl;
    cout << "  Resumes cleaned: " << totalResumesProcessed << endl;
    cout << "  Total records: " << (totalJobsProcessed + totalResumesProcessed) << endl;
    cout << "\nCleaned files are ready to use!\n";
    cout << "=========================================\n";
    
    return 0;
}
