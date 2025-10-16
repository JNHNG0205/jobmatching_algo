#ifndef ARRAY_CPP
#define ARRAY_CPP

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>
using namespace std;

// Forward declarations
struct Job;
struct Resume;

// Base class for data items
class DataItem {
public:
    virtual ~DataItem() = default;
    virtual string getText() const = 0;
    virtual string getSkills() const = 0;
    virtual void display() const = 0;
};

// Job structure
struct Job : public DataItem {
    int id;
    string title;
    string description;
    string skills;
    string company;
    string location;
    string experience_level;
    
    Job() = default;
    Job(const string& csvLine);
    
    string getText() const override { return description; }
    string getSkills() const override { return skills; }
    void display() const override;
    void parseFromCSV(const string& csvLine);
    void parseFromDescription(const string& desc);
    string filterTechnicalSkills(const string& rawSkills);
};

// Resume structure
struct Resume : public DataItem {
    int id;
    string name;
    string summary;
    string skills;
    string experience;
    string education;
    string contact;
    
    Resume() = default;
    Resume(const string& csvLine);
    
    string getText() const override { return summary; }
    string getSkills() const override { return skills; }
    void display() const override;
    void parseFromCSV(const string& csvLine);
    void parseFromDescription(const string& desc);
    string filterTechnicalSkills(const string& rawSkills);
};

// Generic Array template for storing data items
template<typename T>
class Array {
private:
    T* dataArray;       // dynamic array for storing data items
    int capacity;       // maximum allocated slots
    int size;           // current number of items
    void resize();      // private helper to increase capacity
    string normalizeText(const string& text) const;

public:
    // Constructor & Destructor
    Array(int initialCapacity = 10);
    ~Array();

    // Core Functions
    bool insert(const T& item);        // insert data item
    bool remove(int index);            // remove at index

    // Getters
    int getSize() const { return size; }
    int getCapacity() const { return capacity; }
    T getItem(int index) const;
    
    // File operations
    bool loadFromCSV(const string& filename);
    
    // Rule-based matching functions
    void displayMatches(const string& keyword, int maxResults = 5) const;
};

#endif

// Job implementation
Job::Job(const string& csvLine) {
    parseFromCSV(csvLine);
}

void Job::parseFromCSV(const string& csvLine) {
    // Parse CSV line: Job_ID,Title,Skills
    istringstream iss(csvLine);
    string field;
    vector<string> fields;
    
    // Split by comma, handling quoted fields
    bool inQuotes = false;
    string currentField = "";
    
    for (char c : csvLine) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(currentField);
            currentField = "";
        } else {
            currentField += c;
        }
    }
    fields.push_back(currentField); // Add the last field
    
    if (fields.size() >= 3) {
        // Job_ID
        string idStr = fields[0];
        if (!idStr.empty() && idStr.front() == '"' && idStr.back() == '"') {
            idStr = idStr.substr(1, idStr.length() - 2);
        }
        try { id = stoi(idStr); } catch (...) { id = -1; }

        title = fields[1];
        skills = fields[2];
        
        // Clean up title (remove quotes if present)
        if (title.front() == '"' && title.back() == '"') {
            title = title.substr(1, title.length() - 2);
        }
        
        // Clean up skills (remove quotes if present)
        if (skills.front() == '"' && skills.back() == '"') {
            skills = skills.substr(1, skills.length() - 2);
        }
        
        // Set default values for other fields
        description = "Job: " + title + " requiring " + skills;
        company = "Company Not Specified";
        location = "Location Not Specified";
        experience_level = "Not Specified";
    } else {
        // Fallback for malformed data
        id = -1;
        title = "Unknown Position";
        skills = "Not specified";
        description = csvLine;
        company = "Company Not Specified";
        location = "Location Not Specified";
        experience_level = "Not Specified";
    }
}

// Keep the old method for backward compatibility
void Job::parseFromDescription(const string& desc) {
    description = desc;
    
    // Extract title (first part before "needed")
    size_t neededPos = desc.find("needed");
    if (neededPos != string::npos) {
        title = desc.substr(0, neededPos);
        // Clean up title
        title.erase(0, title.find_first_not_of(" \t"));
        title.erase(title.find_last_not_of(" \t") + 1);
    } else {
        title = "Unknown Position";
    }
    
    // Extract skills (text after "experience in" and before the random text)
    size_t expPos = desc.find("experience in");
    if (expPos != string::npos) {
        size_t skillsStart = expPos + 13; // "experience in" length
        size_t skillsEnd = desc.find(".", skillsStart);
        if (skillsEnd == string::npos) skillsEnd = desc.length();
        
        string rawSkills = desc.substr(skillsStart, skillsEnd - skillsStart);
        
        // Clean up and filter skills
        skills = filterTechnicalSkills(rawSkills);
    } else {
        skills = "Not specified";
    }
    
    // Set default values for other fields
    company = "Company Not Specified";
    location = "Location Not Specified";
    experience_level = "Not Specified";
}

// Helper function to filter out noise words and keep only technical skills
string Job::filterTechnicalSkills(const string& rawSkills) {
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
    
    // Split raw skills by comma and check each one
    istringstream iss(rawSkills);
    string skill;
    bool first = true;
    
    while (getline(iss, skill, ',')) {
        // Trim whitespace
        skill.erase(0, skill.find_first_not_of(" \t"));
        skill.erase(skill.find_last_not_of(" \t") + 1);
        
        // Skip empty skills
        if (skill.empty()) continue;
        
        // Check if it's a technical skill (exact match)
        bool isTechnical = false;
        for (int i = 0; i < numSkills; i++) {
            if (skill == technicalSkills[i]) {
                isTechnical = true;
                break;
            }
        }
        
        // Also check for case-insensitive matches
        if (!isTechnical) {
            string lowerSkill = skill;
            transform(lowerSkill.begin(), lowerSkill.end(), lowerSkill.begin(), ::tolower);
            
            for (int i = 0; i < numSkills; i++) {
                string lowerTech = technicalSkills[i];
                transform(lowerTech.begin(), lowerTech.end(), lowerTech.begin(), ::tolower);
                
                if (lowerSkill == lowerTech) {
                    isTechnical = true;
                    skill = technicalSkills[i]; // Use proper case
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

void Job::display() const {
    cout << "Job Description: " << title << " needed with experience in " << skills << "." << endl;
    cout << "Title: " << title << endl;
    cout << "Skills: " << skills << endl;
    cout << "----------------------------------------" << endl;
}

// Resume implementation
Resume::Resume(const string& csvLine) {
    parseFromCSV(csvLine);
}

void Resume::parseFromCSV(const string& csvLine) {
    // Parse CSV line: Resume_ID,Skills
    istringstream iss(csvLine);
    string field;
    vector<string> fields;
    
    // Split by comma, handling quoted fields
    bool inQuotes = false;
    string currentField = "";
    
    for (char c : csvLine) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(currentField);
            currentField = "";
        } else {
            currentField += c;
        }
    }
    fields.push_back(currentField); // Add the last field
    
    if (fields.size() >= 2) {
        // Resume_ID
        string idStr = fields[0];
        if (!idStr.empty() && idStr.front() == '"' && idStr.back() == '"') {
            idStr = idStr.substr(1, idStr.length() - 2);
        }
        try { id = stoi(idStr); } catch (...) { id = -1; }

        skills = fields[1];
        
        // Clean up skills (remove quotes if present)
        if (skills.front() == '"' && skills.back() == '"') {
            skills = skills.substr(1, skills.length() - 2);
        }
        
        // Set default values for other fields
        summary = "Professional with skills in " + skills;
        name = "Professional";
        experience = "Experienced";
        education = "Not Specified";
        contact = "Not Provided";
    } else {
        // Fallback for malformed data
        id = -1;
        skills = "Not specified";
        summary = csvLine;
        name = "Professional";
        experience = "Experienced";
        education = "Not Specified";
        contact = "Not Provided";
    }
}

// Keep the old method for backward compatibility
void Resume::parseFromDescription(const string& desc) {
    summary = desc;
    
    // Extract skills (text after "skilled in" and before the random text)
    size_t skilledPos = desc.find("skilled in");
    if (skilledPos != string::npos) {
        size_t skillsStart = skilledPos + 10; // "skilled in" length
        size_t skillsEnd = desc.find(".", skillsStart);
        if (skillsEnd == string::npos) skillsEnd = desc.length();
        
        string rawSkills = desc.substr(skillsStart, skillsEnd - skillsStart);
        
        // Clean up and filter skills
        skills = filterTechnicalSkills(rawSkills);
    } else {
        skills = "Not specified";
    }
    
    // Set default values for other fields
    name = "Professional";
    experience = "Experienced";
    education = "Not Specified";
    contact = "Not Provided";
}

// Helper function to filter out noise words and keep only technical skills
string Resume::filterTechnicalSkills(const string& rawSkills) {
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
    
    // Split raw skills by comma and check each one
    istringstream iss(rawSkills);
    string skill;
    bool first = true;
    
    while (getline(iss, skill, ',')) {
        // Trim whitespace
        skill.erase(0, skill.find_first_not_of(" \t"));
        skill.erase(skill.find_last_not_of(" \t") + 1);
        
        // Skip empty skills
        if (skill.empty()) continue;
        
        // Check if it's a technical skill (exact match)
        bool isTechnical = false;
        for (int i = 0; i < numSkills; i++) {
            if (skill == technicalSkills[i]) {
                isTechnical = true;
                break;
            }
        }
        
        // Also check for case-insensitive matches
        if (!isTechnical) {
            string lowerSkill = skill;
            transform(lowerSkill.begin(), lowerSkill.end(), lowerSkill.begin(), ::tolower);
            
            for (int i = 0; i < numSkills; i++) {
                string lowerTech = technicalSkills[i];
                transform(lowerTech.begin(), lowerTech.end(), lowerTech.begin(), ::tolower);
                
                if (lowerSkill == lowerTech) {
                    isTechnical = true;
                    skill = technicalSkills[i]; // Use proper case
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

void Resume::display() const {
    cout << "Details: " << "Experienced professional skilled in " << skills << "." << endl;
    cout << "Skills: " << skills << endl;
    cout << "----------------------------------------" << endl;
}

// Template Array implementation
template<typename T>
Array<T>::Array(int initialCapacity) {
    capacity = initialCapacity;
    size = 0;
    dataArray = new T[capacity];
}

// Destructor
template<typename T>
Array<T>::~Array() {
    delete[] dataArray;
}

// Resize (double the capacity when full)
template<typename T>
void Array<T>::resize() {
    int newCapacity = capacity * 2;
    T* newArray = new T[newCapacity];

    for (int i = 0; i < size; i++) {
        newArray[i] = dataArray[i];
    }

    delete[] dataArray;
    dataArray = newArray;
    capacity = newCapacity;
}

// Normalize text: lowercase + trim + remove punctuation
template<typename T>
string Array<T>::normalizeText(const string& text) const {
    string normalized = text;

    // Convert to lowercase
    transform(normalized.begin(), normalized.end(), normalized.begin(),
              [](unsigned char c){ return tolower(c); });

    // Remove punctuation (optional but helps matching)
    normalized.erase(remove_if(normalized.begin(), normalized.end(),
              [](unsigned char c){ return ispunct(c); }), normalized.end());

    // Trim leading/trailing spaces
    size_t start = normalized.find_first_not_of(" \t\n\r");
    size_t end = normalized.find_last_not_of(" \t\n\r");
    if (start == string::npos) return "";
    return normalized.substr(start, end - start + 1);
}

// Insert new item
template<typename T>
bool Array<T>::insert(const T& item) {
    if (size == capacity) {
        resize();
    }
    dataArray[size++] = item;
    return true;
}

// Remove item at index
template<typename T>
bool Array<T>::remove(int index) {
    if (index < 0 || index >= size) return false;

    for (int i = index; i < size - 1; i++) {
        dataArray[i] = dataArray[i + 1];
    }
    size--;
    return true;
}

// (search removed as unused)

// Get item at specific index
template<typename T>
T Array<T>::getItem(int index) const {
    if (index < 0 || index >= size) {
        return T(); // return default constructed object
    }
    return dataArray[index];
}

// Load from CSV file
template<typename T>
bool Array<T>::loadFromCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return false;
    }

    string line;
    bool firstLine = true;
    while (getline(file, line)) {
        if (firstLine) {
            firstLine = false; // Skip header
            continue;
        }
        
        if (!line.empty()) {
            T item(line);
            insert(item);
        }
    }

    file.close();
    return true;
}

// Display multiple matches with scores
template<typename T>
void Array<T>::displayMatches(const string& keyword, int maxResults) const {
    string normKey = normalizeText(keyword);
    
    // Create array to store scores and indices
    struct Match {
        int index;
        int score;
    };
    
    Match* matches = new Match[size];
    int matchCount = 0;
    
    for (int i = 0; i < size; i++) {
        int score = 0;
        
        // Check skills match
        string normSkills = normalizeText(dataArray[i].getSkills());
        if (normSkills.find(normKey) != string::npos) {
            score += 10;
        }
        
        // Check description match
        string normDesc = normalizeText(dataArray[i].getText());
        if (normDesc.find(normKey) != string::npos) {
            score += 5;
        }
        
        // Count skill overlaps
        istringstream iss(normKey);
        string word;
        while (iss >> word) {
            if (normSkills.find(word) != string::npos) {
                score += 2;
            }
        }
        
        if (score > 0) {
            matches[matchCount].index = i;
            matches[matchCount].score = score;
            matchCount++;
        }
    }
    
    // Sort matches by score (simple bubble sort)
    for (int i = 0; i < matchCount - 1; i++) {
        for (int j = 0; j < matchCount - i - 1; j++) {
            if (matches[j].score < matches[j + 1].score) {
                Match temp = matches[j];
                matches[j] = matches[j + 1];
                matches[j + 1] = temp;
            }
        }
    }
    
    // Display top matches
    cout << "\n=== Top " << min(maxResults, matchCount) << " Matches for '" << keyword << "' ===" << endl;
    for (int i = 0; i < min(maxResults, matchCount); i++) {
        cout << "\nMatch " << (i + 1) << " (Score: " << matches[i].score << "):" << endl;
        cout << "ID: " << matches[i].index << endl;
        dataArray[matches[i].index].display();
    }
    
    if (matchCount == 0) {
        cout << "No matches found for '" << keyword << "'" << endl;
    }
    
    delete[] matches;
}

// Explicit template instantiations
template class Array<Job>;
template class Array<Resume>;

