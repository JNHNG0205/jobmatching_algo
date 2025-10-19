#ifndef ARRAY_CPP
#define ARRAY_CPP

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <map>
#include <set>
#include <iterator>
#include <chrono>
#include <iomanip>
using namespace std;

// Custom Dynamic Array class to replace vector
template<typename T>
class DynamicArray {
private:
    T* data;
    int capacity;
    int currentSize;
    
    void resize() {
        int newCapacity = capacity * 2;
        T* newData = new T[newCapacity];
        
        for (int i = 0; i < currentSize; i++) {
            newData[i] = data[i];
        }
        
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }
    
public:
    DynamicArray(int initialCapacity = 10) : capacity(initialCapacity), currentSize(0) {
        data = new T[capacity];
    }
    
    ~DynamicArray() {
        delete[] data;
    }
    
    void push_back(const T& item) {
        if (currentSize >= capacity) {
            resize();
        }
        data[currentSize++] = item;
    }
    
    T& operator[](int index) {
        return data[index];
    }
    
    const T& operator[](int index) const {
        return data[index];
    }
    
    int size() const {
        return currentSize;
    }
    
    bool empty() const {
        return currentSize == 0;
    }
    
    void clear() {
        currentSize = 0;
    }
    
    T* begin() {
        return data;
    }
    
    T* end() {
        return data + currentSize;
    }
    
    const T* begin() const {
        return data;
    }
    
    const T* end() const {
        return data + currentSize;
    }
};

// Forward declarations
struct Job;
struct Resume;

// Function declarations
int calculateCompatibility(const Job& job, const Resume& resume);

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

// Match struct for storing search results
struct Match {
    int index;
    int score;
};

// Generic Array template for storing data items
template<typename T>
class Array {
private:
    T* dataArray;       // dynamic array for storing data items
    int capacity;       // maximum allocated slots
    int size;           // current number of items
    void resize();      // private helper to increase capacity
    
    // Inverted Index for fast searching
    map<string, set<int>> skillIndex;      // skill -> set of document IDs
    map<string, set<int>> titleIndex;      // title words -> set of document IDs
    map<string, set<int>> descriptionIndex; // description words -> set of document IDs
    bool indexBuilt;    // flag to track if index is built
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
    
    // Sorting and optimization functions
    void quickSort(Match* matches, int low, int high) const;
    int partition(Match* matches, int low, int high) const;
    void getTopKMatches(Match* matches, int matchCount, int k, Match* topK) const;
    
    // Inverted Index functions
    void buildIndex();
    void addToIndex(const string& text, int docId, map<string, set<int>>& index);
    set<int> searchIndex(const string& keyword, const map<string, set<int>>& index) const;
    set<int> booleanSearch(const string& query) const;
    DynamicArray<string> tokenize(const string& text) const;
    
    // Optimized job-resume matching functions
    void findBestMatchesForJobs(const Array<Resume>& resumeStorage, int maxJobsToShow) const;
    set<int> findCandidateResumes(const string& jobSkills) const;
    
    // Job title search function
    void displayMatchesByTitle(const string& titleKeyword, int maxResults) const;
};

#endif

// Template specializations for buildIndex
template<>
void Array<Job>::buildIndex() {
    if (indexBuilt) return; // Index already built
    
    // Clear existing indexes
    skillIndex.clear();
    titleIndex.clear();
    descriptionIndex.clear();
    
    // Build indexes for each document
    for (int i = 0; i < size; i++) {
        // Index skills - split by comma first to handle multi-word skills as phrases
        string skills = dataArray[i].getSkills();
        istringstream skillStream(skills);
        string skill;
        while (getline(skillStream, skill, ',')) {
            // Trim whitespace
            skill.erase(0, skill.find_first_not_of(" \t"));
            skill.erase(skill.find_last_not_of(" \t") + 1);
            if (!skill.empty()) {
                string normSkill = normalizeText(skill);
                // Add the entire skill phrase as a single token
                skillIndex[normSkill].insert(i);
            }
        }
        
        // Index title (for jobs) - use the title field directly
        string title = dataArray[i].title;
        title = normalizeText(title);
        addToIndex(title, i, titleIndex);
        
        // Index full description
        string description = normalizeText(dataArray[i].getText());
        addToIndex(description, i, descriptionIndex);
    }
    
    indexBuilt = true;
    cout << "Inverted index built successfully!" << endl;
}

template<>
void Array<Resume>::buildIndex() {
    if (indexBuilt) return; // Index already built
    
    // Clear existing indexes
    skillIndex.clear();
    titleIndex.clear();
    descriptionIndex.clear();
    
    // Build indexes for each document
    for (int i = 0; i < size; i++) {
        // Index skills - split by comma first to handle multi-word skills as phrases
        string skills = dataArray[i].getSkills();
        istringstream skillStream(skills);
        string skill;
        while (getline(skillStream, skill, ',')) {
            // Trim whitespace
            skill.erase(0, skill.find_first_not_of(" \t"));
            skill.erase(skill.find_last_not_of(" \t") + 1);
            if (!skill.empty()) {
                string normSkill = normalizeText(skill);
                // Add the entire skill phrase as a single token
                skillIndex[normSkill].insert(i);
            }
        }
        
        // Index full description
        string description = normalizeText(dataArray[i].getText());
        addToIndex(description, i, descriptionIndex);
    }
    
    indexBuilt = true;
    cout << "Inverted index built successfully!" << endl;
}

// Job implementation
Job::Job(const string& csvLine) {
    parseFromCSV(csvLine);
}

void Job::parseFromCSV(const string& csvLine) {
    // Parse CSV line: Job_ID,Title,Skills
    istringstream iss(csvLine);
    string field;
    DynamicArray<string> fields;
    
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
    DynamicArray<string> fields;
    
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
    indexBuilt = false;
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

    // Remove punctuation but keep spaces for tokenization
    normalized.erase(remove_if(normalized.begin(), normalized.end(),
              [](unsigned char c){ return ispunct(c) && c != ' '; }), normalized.end());

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
    
    // Build inverted index after loading data
    if (size > 0) {
        buildIndex();
    }
    
    return true;
}

// Quick Sort implementation for sorting matches by score (descending order)
template<typename T>
void Array<T>::quickSort(Match* matches, int low, int high) const {
    if (low < high) {
        int pivotIndex = partition(matches, low, high);
        quickSort(matches, low, pivotIndex - 1);
        quickSort(matches, pivotIndex + 1, high);
    }
}

// Partition function for Quick Sort
template<typename T>
int Array<T>::partition(Match* matches, int low, int high) const {
    int pivot = matches[high].score;
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        // Sort in descending order (highest scores first)
        if (matches[j].score >= pivot) {
            i++;
            swap(matches[i], matches[j]);
        }
    }
    swap(matches[i + 1], matches[high]);
    return i + 1;
}

// Top-K optimization: Get only the top K results without sorting entire array
template<typename T>
void Array<T>::getTopKMatches(Match* matches, int matchCount, int k, Match* topK) const {
    if (k >= matchCount) {
        // If k >= matchCount, just copy all matches
        for (int i = 0; i < matchCount; i++) {
            topK[i] = matches[i];
        }
        return;
    }
    
    // Use partial sort to get top K elements
    // This is more efficient than sorting the entire array
    for (int i = 0; i < k; i++) {
        int maxIndex = i;
        for (int j = i + 1; j < matchCount; j++) {
            if (matches[j].score > matches[maxIndex].score) {
                maxIndex = j;
            }
        }
        if (maxIndex != i) {
            swap(matches[i], matches[maxIndex]);
        }
        topK[i] = matches[i];
    }
}

// Display multiple matches with scores using Inverted Index
template<typename T>
void Array<T>::displayMatches(const string& keyword, int maxResults) const {
    // Build index if not already built
    if (!indexBuilt) {
        cout << "Building inverted index for fast search..." << endl;
        const_cast<Array<T>*>(this)->buildIndex();
    }
    
    // Use inverted index for fast search
    set<int> candidateIds = booleanSearch(keyword);
    
    if (candidateIds.empty()) {
        cout << "No matches found for '" << keyword << "'" << endl;
        return;
    }
    
    // Create array to store scores and indices for candidates only
    Match* matches = new Match[candidateIds.size()];
    int matchCount = 0;
    
    // Only process candidates from inverted index (much faster!)
    for (int docId : candidateIds) {
        int score = 0;
        
        string normSkills = normalizeText(dataArray[docId].getSkills());
        string normDesc = normalizeText(dataArray[docId].getText());
        
        // Handle comma-separated skills properly (check BEFORE normalizing)
        if (keyword.find(',') != string::npos) {
            // Parse comma-separated skills
            DynamicArray<string> searchSkills;
            istringstream iss(keyword);
            string skill;
            while (getline(iss, skill, ',')) {
                // Trim whitespace
                skill.erase(0, skill.find_first_not_of(" \t"));
                skill.erase(skill.find_last_not_of(" \t") + 1);
                if (!skill.empty()) {
                    // Normalize each skill before searching
                    searchSkills.push_back(normalizeText(skill));
                }
            }
            
            // Score based on individual skill matches
            for (const string& searchSkill : searchSkills) {
                if (normSkills.find(searchSkill) != string::npos) {
                    score += 10; // Each matching skill adds 10 points
                }
                if (normDesc.find(searchSkill) != string::npos) {
                    score += 5; // Each matching skill in description adds 5 points
                }
            }
        } else {
            // Single skill search
            string normKey = normalizeText(keyword);
        if (normSkills.find(normKey) != string::npos) {
            score += 10;
        }
        if (normDesc.find(normKey) != string::npos) {
            score += 5;
        }
        
            // Count word overlaps for single term
        istringstream iss(normKey);
        string word;
        while (iss >> word) {
            if (normSkills.find(word) != string::npos) {
                score += 2;
                }
            }
        }
        
        if (score > 0) {
            matches[matchCount].index = docId;
            matches[matchCount].score = score;
            matchCount++;
        }
    }
    
    // Top-K optimization: Only get the top results we need to display
    int resultsToShow = min(maxResults, matchCount);
    Match* topMatches = new Match[resultsToShow];
    
    if (matchCount > 0) {
        getTopKMatches(matches, matchCount, resultsToShow, topMatches);
    }
    
    // Display top matches
    cout << "\n=== Top " << resultsToShow << " Matches for '" << keyword << "' ===" << endl;
    for (int i = 0; i < resultsToShow; i++) {
        cout << "\nMatch " << (i + 1) << " (Score: " << topMatches[i].score << "):" << endl;
        cout << "ID: " << topMatches[i].index << endl;
        dataArray[topMatches[i].index].display();
    }
    
    delete[] topMatches;
    
    if (matchCount == 0) {
        cout << "No matches found for '" << keyword << "'" << endl;
    }
    
    delete[] matches;
}


// Add text to inverted index
template<typename T>
void Array<T>::addToIndex(const string& text, int docId, map<string, set<int>>& index) {
    DynamicArray<string> tokens = tokenize(text);
    for (const string& token : tokens) {
        if (token.length() > 1) { // Skip single characters
            index[token].insert(docId);
        }
    }
}

// Search in a specific index
template<typename T>
set<int> Array<T>::searchIndex(const string& keyword, const map<string, set<int>>& index) const {
    string normKey = normalizeText(keyword);
    
    // For skill index, treat the keyword as a phrase (don't tokenize)
    if (&index == &skillIndex) {
        auto it = index.find(normKey);
        if (it != index.end()) {
            return it->second;
        }
        return set<int>();
    }
    
    // For other indexes, tokenize and do AND search
    DynamicArray<string> tokens = tokenize(normKey);
    
    set<int> result;
    for (const string& token : tokens) {
        auto it = index.find(token);
        if (it != index.end()) {
            if (result.empty()) {
                result = it->second; // First token
            } else {
                // Intersection for AND operation
                set<int> intersection;
                set_intersection(result.begin(), result.end(),
                               it->second.begin(), it->second.end(),
                               inserter(intersection, intersection.begin()));
                result = intersection;
            }
        } else {
            return set<int>(); // No matches if any token not found
        }
    }
    return result;
}

// Boolean search with AND/OR operations
template<typename T>
set<int> Array<T>::booleanSearch(const string& query) const {
    if (!indexBuilt) {
        cout << "Index not built! Building now..." << endl;
        const_cast<Array<T>*>(this)->buildIndex();
    }
    
    // Check for comma-separated skills BEFORE normalizing (comma will be removed by normalize)
    if (query.find(',') != string::npos) {
        DynamicArray<string> skills;
        istringstream iss(query);
        string skill;
        while (getline(iss, skill, ',')) {
            // Trim whitespace
            skill.erase(0, skill.find_first_not_of(" \t"));
            skill.erase(skill.find_last_not_of(" \t") + 1);
            if (!skill.empty()) {
                skills.push_back(skill);
            }
        }
        
        if (skills.empty()) {
            return set<int>();
        }
        
        // Start with first skill
        set<int> result = searchIndex(skills[0], skillIndex);
        
        // Union with remaining skills (OR operation)
        for (int i = 1; i < skills.size(); i++) {
            set<int> skillResults = searchIndex(skills[i], skillIndex);
            result.insert(skillResults.begin(), skillResults.end());
        }
        
        return result;
    }
    
    string normQuery = normalizeText(query);
    
    // Check for OR operation
    if (normQuery.find(" or ") != string::npos) {
        DynamicArray<string> orTerms;
        istringstream iss(normQuery);
        string term;
        while (getline(iss, term, '|')) {
            if (term.find(" or ") != string::npos) {
                size_t pos = term.find(" or ");
                orTerms.push_back(term.substr(0, pos));
                orTerms.push_back(term.substr(pos + 4));
            } else {
                orTerms.push_back(term);
            }
        }
        
        set<int> result;
        for (const string& term : orTerms) {
            set<int> termResults = searchIndex(term, skillIndex);
            result.insert(termResults.begin(), termResults.end());
        }
        return result;
    }
    
    // Default AND search for single term
    return searchIndex(normQuery, skillIndex);
}

// Tokenize text into words
template<typename T>
DynamicArray<string> Array<T>::tokenize(const string& text) const {
    DynamicArray<string> tokens;
    istringstream iss(text);
    string word;
    
    while (iss >> word) {
        // Remove punctuation
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        if (!word.empty()) {
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            tokens.push_back(word);
        }
    }
    
    return tokens;
}

// Find candidate resumes using inverted index for a given job skills string
// This function is only available for Job arrays
template<>
set<int> Array<Job>::findCandidateResumes(const string& jobSkills) const {
    if (!indexBuilt) {
        cout << "Index not built! Building now..." << endl;
        const_cast<Array<Job>*>(this)->buildIndex();
    }
    
    // Use boolean search to find resumes that match the job skills
    return booleanSearch(jobSkills);
}

// Optimized function to find best matches for each job using all advanced algorithms
// This function is only available for Job arrays
template<>
void Array<Job>::findBestMatchesForJobs(const Array<Resume>& resumeStorage, int maxJobsToShow) const {
    if (!indexBuilt) {
        cout << "Building inverted index for optimized matching..." << endl;
        const_cast<Array<Job>*>(this)->buildIndex();
    }
    
    cout << "\n=== Optimized Job-Resume Matching ===" << endl;
    cout << "Using: Inverted Index + Boolean Search + Quick Sort + Top-K" << endl;
    cout << "Processing " << min(maxJobsToShow, size) << " jobs..." << endl;
    cout << "==========================================\n";
    
    // Start timing
    auto startTime = chrono::high_resolution_clock::now();
    
    int processedJobs = 0;
    
    for (int i = 0; i < min(maxJobsToShow, size); i++) {
        Job currentJob = dataArray[i];
        
        // Step 1: Use inverted index to find candidate resumes (O(1) lookup)
        set<int> candidateResumeIds = findCandidateResumes(currentJob.skills);
        
        if (candidateResumeIds.empty()) {
            cout << "\nJob ID: " << currentJob.id << endl;
            cout << "Job Title: " << currentJob.title << endl;
            cout << "Job Skills: " << currentJob.skills << endl;
            cout << "No matching resumes found." << endl;
            cout << "----------------------------------------" << endl;
            continue;
        }
        
        // Step 2: Calculate compatibility scores only for candidates (not all resumes!)
        struct JobResumeMatch {
            int resumeId;
            int score;
        };
        
        JobResumeMatch* matches = new JobResumeMatch[candidateResumeIds.size()];
        int matchCount = 0;
        
        for (int resumeId : candidateResumeIds) {
            // resumeId is the document ID from the index, which corresponds to the array index
            Resume candidateResume = resumeStorage.getItem(resumeId);
            int score = calculateCompatibility(currentJob, candidateResume);
            
            if (score > 0) {
                matches[matchCount].resumeId = resumeId;
                matches[matchCount].score = score;
                matchCount++;
            }
        }
        
        if (matchCount == 0) {
            cout << "\nJob ID: " << currentJob.id << endl;
            cout << "Job Title: " << currentJob.title << endl;
            cout << "Job Skills: " << currentJob.skills << endl;
            cout << "No matching resumes found." << endl;
            cout << "----------------------------------------" << endl;
            delete[] matches;
            continue;
        }
        
        // Step 3: Use Quick Sort to sort matches by score (O(n log n))
        if (matchCount > 1) {
            // Custom quick sort for JobResumeMatch
    for (int i = 0; i < matchCount - 1; i++) {
        for (int j = 0; j < matchCount - i - 1; j++) {
            if (matches[j].score < matches[j + 1].score) {
                        JobResumeMatch temp = matches[j];
                matches[j] = matches[j + 1];
                matches[j + 1] = temp;
                    }
                }
            }
        }
        
        // Step 4: Find best score and collect all resumes that tie
        int bestScore = matches[0].score;
        string resumeIds = "";
        bool firstId = true;
        
        for (int j = 0; j < matchCount; j++) {
            if (matches[j].score == bestScore) {
                if (!firstId) resumeIds += ", ";
                resumeIds += to_string(matches[j].resumeId);
                firstId = false;
            } else {
                break; // Since sorted, no more matches with best score
            }
        }
        
        // Display results
        cout << "\nJob ID: " << currentJob.id << endl;
        cout << "Job Title: " << currentJob.title << endl;
        cout << "Job Skills: " << currentJob.skills << endl;
        cout << "Resume ID: " << resumeIds << endl;
        cout << "Best Score: " << bestScore << endl;
        cout << "Candidates Found: " << matchCount << " (from " << candidateResumeIds.size() << " candidates)" << endl;
        cout << "----------------------------------------" << endl;
        
        delete[] matches;
        processedJobs++;
        
        // Progress indicator
        if (processedJobs % 100 == 0) {
            cout << "[Progress: " << processedJobs << "/" << min(maxJobsToShow, size) << " jobs processed]" << endl;
        }
    }
    
    // End timing
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    double durationSeconds = duration.count() / 1000.0;
    double avgTimePerJobMs = (processedJobs > 0 ? duration.count() / processedJobs : 0);
    double avgTimePerJobSeconds = (processedJobs > 0 ? durationSeconds / processedJobs : 0);
    
    cout << "\n=== Optimization Results ===" << endl;
    cout << "Processed " << processedJobs << " jobs in " << duration.count() << " ms (" << fixed << setprecision(3) << durationSeconds << " seconds)" << endl;
    cout << "Average time per job: " << fixed << setprecision(1) << avgTimePerJobMs << " ms (" << fixed << setprecision(3) << avgTimePerJobSeconds << " seconds)" << endl;
    cout << "Performance: " << fixed << setprecision(2) << (processedJobs / durationSeconds) << " jobs/second" << endl;
}

// External compatibility function (needs to be accessible)
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
    
    return score;
}

// Search jobs by title (only for Job objects)
template<>
void Array<Job>::displayMatchesByTitle(const string& titleKeyword, int maxResults) const {
    // Build index if not already built
    if (!indexBuilt) {
        cout << "Building inverted index for fast search..." << endl;
        const_cast<Array<Job>*>(this)->buildIndex();
    }
    
    // Use title index for fast search
    set<int> candidateIds = searchIndex(titleKeyword, titleIndex);
    
    if (candidateIds.empty()) {
        cout << "No jobs found with title containing '" << titleKeyword << "'" << endl;
        return;
    }
    
    // Create array to store scores and indices for candidates only
    Match* matches = new Match[candidateIds.size()];
    int matchCount = 0;
    
    string normTitle = normalizeText(titleKeyword);
    
    // Only process candidates from inverted index (much faster!)
    for (int docId : candidateIds) {
        int score = 0;
        
        string normJobTitle = normalizeText(dataArray[docId].title);
        string normDesc = normalizeText(dataArray[docId].getText());
        
        // Score based on title match
        if (normJobTitle.find(normTitle) != string::npos) {
            score += 20; // High score for title match
        }
        if (normDesc.find(normTitle) != string::npos) {
            score += 10; // Lower score for description match
        }
        
        // Count word overlaps for better matching
        istringstream iss(normTitle);
        string word;
        while (iss >> word) {
            if (normJobTitle.find(word) != string::npos) {
                score += 5; // Bonus for each word match in title
            }
            if (normDesc.find(word) != string::npos) {
                score += 2; // Small bonus for word match in description
            }
        }
        
        if (score > 0) {
            matches[matchCount].index = docId;
            matches[matchCount].score = score;
            matchCount++;
        }
    }
    
    // Top-K optimization: Only get the top results we need to display
    int resultsToShow = min(maxResults, matchCount);
    Match* topResults = new Match[resultsToShow];
    getTopKMatches(matches, matchCount, resultsToShow, topResults);
    
    // Display results
    cout << "\n=== Top " << resultsToShow << " Job Title Matches for '" << titleKeyword << "' ===" << endl;
    cout << endl;
    
    for (int i = 0; i < resultsToShow; i++) {
        cout << "Match " << (i + 1) << " (Score: " << topResults[i].score << "):" << endl;
        dataArray[topResults[i].index].display();
    }
    
    // Clean up
    delete[] matches;
    delete[] topResults;
}


// Explicit template instantiations
template class Array<Job>;
template class Array<Resume>;

