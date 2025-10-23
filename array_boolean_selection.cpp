#ifndef ARRAY_BOOLEAN_SELECTION_CPP
#define ARRAY_BOOLEAN_SELECTION_CPP

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

// Simple String Array class for storing strings 
class StringArray {
private:
    string* data;
    int capacity;
    int currentSize;
    
    void resize() {
        int newCapacity = capacity * 2;
        string* newData = new string[newCapacity];
        
        for (int i = 0; i < currentSize; i++) {
            newData[i] = data[i];
        }
        
        delete[] data;
        data = newData;
        capacity = newCapacity;
    }
    
public:
    StringArray(int initialCapacity = 10) : capacity(initialCapacity), currentSize(0) {
        data = new string[capacity];
    }
    
    ~StringArray() {
        delete[] data;
    }
    
    void push_back(const string& item) {
        if (currentSize >= capacity) {
            resize();
        }
        data[currentSize++] = item;
    }
    
    string& operator[](int index) {
        return data[index];
    }
    
    const string& operator[](int index) const {
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
    
    string* begin() {
        return data;
    }
    
    string* end() {
        return data + currentSize;
    }
    
    const string* begin() const {
        return data;
    }
    
    const string* end() const {
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
// USING: BOOLEAN SEARCH (INVERTED INDEX) + SELECTION SORT
template<typename T>
class Array {
private:
    T* dataArray;       // dynamic array for storing data items
    int capacity;       // maximum allocated slots
    int size;           // current number of items
    void resize();      // private helper to increase capacity
    
    // Inverted Index for BOOLEAN SEARCH
    map<string, set<int>> skillIndex;
    map<string, set<int>> titleIndex;
    map<string, set<int>> descriptionIndex;
    bool indexBuilt;
    string normalizeText(const string& text) const;

public:
    // Constructor & Destructor
    Array(int initialCapacity = 10);
    ~Array();

    // Core Functions
    bool insert(const T& item);
    bool remove(int index);

    // Getters
    int getSize() const { return size; }
    int getCapacity() const { return capacity; }
    T getItem(int index) const;
    
    // File operations
    bool loadFromCSV(const string& filename);
    
    // Rule-based matching functions
    void displayMatches(const string& keyword, int maxResults = 5) const;
    
    // SELECTION SORT implementation
    void selectionSort(Match* matches, int matchCount) const;
    
    // Inverted Index (BOOLEAN SEARCH) functions
    void buildIndex();
    void addToIndex(const string& text, int docId, map<string, set<int>>& index);
    set<int> searchIndex(const string& keyword, const map<string, set<int>>& index) const;
    set<int> booleanSearch(const string& query) const;
    StringArray tokenize(const string& text) const;
    
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
    if (indexBuilt) return;
    
    skillIndex.clear();
    titleIndex.clear();
    descriptionIndex.clear();
    
    for (int i = 0; i < size; i++) {
        // Index skills
        string skills = dataArray[i].getSkills();
        istringstream skillStream(skills);
        string skill;
        while (getline(skillStream, skill, ',')) {
            skill.erase(0, skill.find_first_not_of(" \t"));
            skill.erase(skill.find_last_not_of(" \t") + 1);
            if (!skill.empty()) {
                string normSkill = normalizeText(skill);
                skillIndex[normSkill].insert(i);
            }
        }
        
        // Index title
        string title = dataArray[i].title;
        title = normalizeText(title);
        addToIndex(title, i, titleIndex);
        
        // Index description
        string description = normalizeText(dataArray[i].getText());
        addToIndex(description, i, descriptionIndex);
    }
    
    indexBuilt = true;
    cout << "Inverted index built successfully (Boolean Search enabled)!" << endl;
}

template<>
void Array<Resume>::buildIndex() {
    if (indexBuilt) return;
    
    skillIndex.clear();
    titleIndex.clear();
    descriptionIndex.clear();
    
    for (int i = 0; i < size; i++) {
        // Index skills
        string skills = dataArray[i].getSkills();
        istringstream skillStream(skills);
        string skill;
        while (getline(skillStream, skill, ',')) {
            skill.erase(0, skill.find_first_not_of(" \t"));
            skill.erase(skill.find_last_not_of(" \t") + 1);
            if (!skill.empty()) {
                string normSkill = normalizeText(skill);
                skillIndex[normSkill].insert(i);
            }
        }
        
        // Index description
        string description = normalizeText(dataArray[i].getText());
        addToIndex(description, i, descriptionIndex);
    }
    
    indexBuilt = true;
    cout << "Inverted index built successfully (Boolean Search enabled)!" << endl;
}

// Job implementation
Job::Job(const string& csvLine) {
    parseFromCSV(csvLine);
}

void Job::parseFromCSV(const string& csvLine) {
    istringstream iss(csvLine);
    string field;
    StringArray fields;
    
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
    fields.push_back(currentField);
    
    if (fields.size() >= 3) {
        string idStr = fields[0];
        if (!idStr.empty() && idStr.front() == '"' && idStr.back() == '"') {
            idStr = idStr.substr(1, idStr.length() - 2);
        }
        try { id = stoi(idStr); } catch (...) { id = -1; }

        title = fields[1];
        skills = fields[2];
        
        if (title.front() == '"' && title.back() == '"') {
            title = title.substr(1, title.length() - 2);
        }
        
        if (skills.front() == '"' && skills.back() == '"') {
            skills = skills.substr(1, skills.length() - 2);
        }
        
        description = "Job: " + title + " requiring " + skills;
        company = "Company Not Specified";
        location = "Location Not Specified";
        experience_level = "Not Specified";
    } else {
        id = -1;
        title = "Unknown Position";
        skills = "Not specified";
        description = csvLine;
        company = "Company Not Specified";
        location = "Location Not Specified";
        experience_level = "Not Specified";
    }
}

void Job::parseFromDescription(const string& desc) {
    description = desc;
    
    size_t neededPos = desc.find("needed");
    if (neededPos != string::npos) {
        title = desc.substr(0, neededPos);
        title.erase(0, title.find_first_not_of(" \t"));
        title.erase(title.find_last_not_of(" \t") + 1);
    } else {
        title = "Unknown Position";
    }
    
    size_t expPos = desc.find("experience in");
    if (expPos != string::npos) {
        size_t skillsStart = expPos + 13;
        size_t skillsEnd = desc.find(".", skillsStart);
        if (skillsEnd == string::npos) skillsEnd = desc.length();
        
        string rawSkills = desc.substr(skillsStart, skillsEnd - skillsStart);
        skills = filterTechnicalSkills(rawSkills);
    } else {
        skills = "Not specified";
    }
    
    company = "Company Not Specified";
    location = "Location Not Specified";
    experience_level = "Not Specified";
}

string Job::filterTechnicalSkills(const string& rawSkills) {
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
    istringstream iss(csvLine);
    string field;
    StringArray fields;
    
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
    fields.push_back(currentField);
    
    if (fields.size() >= 2) {
        string idStr = fields[0];
        if (!idStr.empty() && idStr.front() == '"' && idStr.back() == '"') {
            idStr = idStr.substr(1, idStr.length() - 2);
        }
        try { id = stoi(idStr); } catch (...) { id = -1; }

        skills = fields[1];
        
        if (skills.front() == '"' && skills.back() == '"') {
            skills = skills.substr(1, skills.length() - 2);
        }
        
        summary = "Professional with skills in " + skills;
        name = "Professional";
        experience = "Experienced";
        education = "Not Specified";
        contact = "Not Provided";
    } else {
        id = -1;
        skills = "Not specified";
        summary = csvLine;
        name = "Professional";
        experience = "Experienced";
        education = "Not Specified";
        contact = "Not Provided";
    }
}

void Resume::parseFromDescription(const string& desc) {
    summary = desc;
    
    size_t skilledPos = desc.find("skilled in");
    if (skilledPos != string::npos) {
        size_t skillsStart = skilledPos + 10;
        size_t skillsEnd = desc.find(".", skillsStart);
        if (skillsEnd == string::npos) skillsEnd = desc.length();
        
        string rawSkills = desc.substr(skillsStart, skillsEnd - skillsStart);
        skills = filterTechnicalSkills(rawSkills);
    } else {
        skills = "Not specified";
    }
    
    name = "Professional";
    experience = "Experienced";
    education = "Not Specified";
    contact = "Not Provided";
}

string Resume::filterTechnicalSkills(const string& rawSkills) {
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

template<typename T>
Array<T>::~Array() {
    delete[] dataArray;
}

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

template<typename T>
string Array<T>::normalizeText(const string& text) const {
    string normalized = text;

    transform(normalized.begin(), normalized.end(), normalized.begin(),
              [](unsigned char c){ return tolower(c); });

    normalized.erase(remove_if(normalized.begin(), normalized.end(),
              [](unsigned char c){ return ispunct(c) && c != ' '; }), normalized.end());

    size_t start = normalized.find_first_not_of(" \t\n\r");
    size_t end = normalized.find_last_not_of(" \t\n\r");
    if (start == string::npos) return "";
    return normalized.substr(start, end - start + 1);
}

template<typename T>
bool Array<T>::insert(const T& item) {
    if (size == capacity) {
        resize();
    }
    dataArray[size++] = item;
    return true;
}

template<typename T>
bool Array<T>::remove(int index) {
    if (index < 0 || index >= size) return false;

    for (int i = index; i < size - 1; i++) {
        dataArray[i] = dataArray[i + 1];
    }
    size--;
    return true;
}

template<typename T>
T Array<T>::getItem(int index) const {
    if (index < 0 || index >= size) {
        return T();
    }
    return dataArray[index];
}

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
            firstLine = false;
            continue;
        }
        
        if (!line.empty()) {
            T item(line);
            insert(item);
        }
    }

    file.close();
    
    if (size > 0) {
        buildIndex();
    }
    
    return true;
}

// SELECTION SORT implementation - O(n^2) but better than bubble sort
template<typename T>
void Array<T>::selectionSort(Match* matches, int matchCount) const {
    // Sort in descending order (highest scores first)
    for (int i = 0; i < matchCount - 1; i++) {
        int maxIndex = i;
        for (int j = i + 1; j < matchCount; j++) {
            if (matches[j].score > matches[maxIndex].score) {
                maxIndex = j;
            }
        }
        if (maxIndex != i) {
            Match temp = matches[i];
            matches[i] = matches[maxIndex];
            matches[maxIndex] = temp;
        }
    }
}

// Display multiple matches using BOOLEAN SEARCH (Inverted Index) + SELECTION SORT
template<typename T>
void Array<T>::displayMatches(const string& keyword, int maxResults) const {
    // Start timing
    auto startTime = chrono::high_resolution_clock::now();
    
    if (!indexBuilt) {
        cout << "Building inverted index for Boolean Search..." << endl;
        const_cast<Array<T>*>(this)->buildIndex();
    }
    
    cout << "\nUsing: Boolean Search (Inverted Index) + Selection Sort" << endl;
    
    // BOOLEAN SEARCH using inverted index
    set<int> candidateIds = booleanSearch(keyword);
    
    if (candidateIds.empty()) {
        cout << "No matches found for '" << keyword << "'" << endl;
        return;
    }
    
    Match* matches = new Match[candidateIds.size()];
    int matchCount = 0;
    
    // Only process candidates from inverted index
    for (int docId : candidateIds) {
        int score = 0;
        
        string normSkills = normalizeText(dataArray[docId].getSkills());
        string normDesc = normalizeText(dataArray[docId].getText());
        
        if (keyword.find(',') != string::npos) {
            StringArray searchSkills;
            istringstream iss(keyword);
            string skill;
            while (getline(iss, skill, ',')) {
                skill.erase(0, skill.find_first_not_of(" \t"));
                skill.erase(skill.find_last_not_of(" \t") + 1);
                if (!skill.empty()) {
                    searchSkills.push_back(normalizeText(skill));
                }
            }
            
            for (const string& searchSkill : searchSkills) {
                if (normSkills.find(searchSkill) != string::npos) {
                    score += 10;
                }
                if (normDesc.find(searchSkill) != string::npos) {
                    score += 5;
                }
            }
        } else {
            string normKey = normalizeText(keyword);
            if (normSkills.find(normKey) != string::npos) {
                score += 10;
            }
            if (normDesc.find(normKey) != string::npos) {
                score += 5;
            }
            
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
    
    if (matchCount == 0) {
        cout << "No matches found for '" << keyword << "'" << endl;
        delete[] matches;
        return;
    }
    
    // Use SELECTION SORT to sort matches
    selectionSort(matches, matchCount);
    
    // End timing
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    
    int resultsToShow = min(maxResults, matchCount);
    cout << "\n=== Top " << resultsToShow << " Matches for '" << keyword << "' ===" << endl;
    
    for (int i = 0; i < resultsToShow; i++) {
        cout << "\nMatch " << (i + 1) << " (Score: " << matches[i].score << "):" << endl;
        cout << "ID: " << matches[i].index << endl;
        dataArray[matches[i].index].display();
    }
    
    // Display statistics at the end
    cout << "\n==========================================";
    cout << "\nJobs/Resumes Found: " << candidateIds.size() << " (from " << size << " total items)";
    cout << "\nMatches with Scores: " << matchCount;
    cout << "\nTime Taken: " << duration.count() << " ms (" << fixed << setprecision(3) << (duration.count() / 1000.0) << " seconds)";
    cout << "\n==========================================\n" << endl;
    
    delete[] matches;
}

// Add text to inverted index
template<typename T>
void Array<T>::addToIndex(const string& text, int docId, map<string, set<int>>& index) {
    StringArray tokens = tokenize(text);
    for (const string& token : tokens) {
        if (token.length() > 1) {
            index[token].insert(docId);
        }
    }
}

// Search in a specific index
template<typename T>
set<int> Array<T>::searchIndex(const string& keyword, const map<string, set<int>>& index) const {
    string normKey = normalizeText(keyword);
    
    if (&index == &skillIndex) {
        auto it = index.find(normKey);
        if (it != index.end()) {
            return it->second;
        }
        return set<int>();
    }
    
    StringArray tokens = tokenize(normKey);
    
    set<int> result;
    for (const string& token : tokens) {
        auto it = index.find(token);
        if (it != index.end()) {
            if (result.empty()) {
                result = it->second;
            } else {
                set<int> intersection;
                set_intersection(result.begin(), result.end(),
                               it->second.begin(), it->second.end(),
                               inserter(intersection, intersection.begin()));
                result = intersection;
            }
        } else {
            return set<int>();
        }
    }
    return result;
}

// BOOLEAN SEARCH with AND/OR operations
template<typename T>
set<int> Array<T>::booleanSearch(const string& query) const {
    if (!indexBuilt) {
        cout << "Index not built! Building now..." << endl;
        const_cast<Array<T>*>(this)->buildIndex();
    }
    
    // Check for comma-separated skills (OR operation)
    if (query.find(',') != string::npos) {
        StringArray skills;
        istringstream iss(query);
        string skill;
        while (getline(iss, skill, ',')) {
            skill.erase(0, skill.find_first_not_of(" \t"));
            skill.erase(skill.find_last_not_of(" \t") + 1);
            if (!skill.empty()) {
                skills.push_back(skill);
            }
        }
        
        if (skills.empty()) {
            return set<int>();
        }
        
        set<int> result = searchIndex(skills[0], skillIndex);
        
        for (int i = 1; i < skills.size(); i++) {
            set<int> skillResults = searchIndex(skills[i], skillIndex);
            result.insert(skillResults.begin(), skillResults.end());
        }
        
        return result;
    }
    
    string normQuery = normalizeText(query);
    
    // Check for OR operation
    if (normQuery.find(" or ") != string::npos) {
        StringArray orTerms;
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
    
    return searchIndex(normQuery, skillIndex);
}

// Tokenize text into words
template<typename T>
StringArray Array<T>::tokenize(const string& text) const {
    StringArray tokens;
    istringstream iss(text);
    string word;
    
    while (iss >> word) {
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        if (!word.empty()) {
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            tokens.push_back(word);
        }
    }
    
    return tokens;
}

// Find candidate resumes using BOOLEAN SEARCH
template<>
set<int> Array<Job>::findCandidateResumes(const string& jobSkills) const {
    if (!indexBuilt) {
        cout << "Index not built! Building now..." << endl;
        const_cast<Array<Job>*>(this)->buildIndex();
    }
    
    return booleanSearch(jobSkills);
}

// Optimized function using BOOLEAN SEARCH + SELECTION SORT
template<>
void Array<Job>::findBestMatchesForJobs(const Array<Resume>& resumeStorage, int maxJobsToShow) const {
    if (!indexBuilt) {
        cout << "Building inverted index for Boolean Search..." << endl;
        const_cast<Array<Job>*>(this)->buildIndex();
    }
    
    // Calculate inverted index memory usage
    size_t indexMemory = 0;
    for (const auto& entry : skillIndex) {
        indexMemory += entry.first.capacity() + entry.second.size() * sizeof(int);
    }
    for (const auto& entry : titleIndex) {
        indexMemory += entry.first.capacity() + entry.second.size() * sizeof(int);
    }
    for (const auto& entry : descriptionIndex) {
        indexMemory += entry.first.capacity() + entry.second.size() * sizeof(int);
    }
    
    cout << "\n=== Job-Resume Matching (Boolean Search + Selection Sort) ===" << endl;
    cout << "Using: Inverted Index (Boolean Search) + Selection Sort" << endl;
    cout << "Processing " << min(maxJobsToShow, size) << " jobs..." << endl;
    cout << "==========================================\n";
    
    auto startTime = chrono::high_resolution_clock::now();
    
    // Memory tracking
    size_t totalMemoryUsed = 0;
    size_t peakMemoryUsed = 0;
    
    int processedJobs = 0;
    
    for (int i = 0; i < min(maxJobsToShow, size); i++) {
        Job currentJob = dataArray[i];
        
        // BOOLEAN SEARCH to find candidate resumes using inverted index
        set<int> candidateResumeIds = findCandidateResumes(currentJob.skills);
        
        if (candidateResumeIds.empty()) {
            cout << "\nJob ID: " << currentJob.id << endl;
            cout << "Job Title: " << currentJob.title << endl;
            cout << "Job Skills: " << currentJob.skills << endl;
            cout << "No matching resumes found." << endl;
            cout << "----------------------------------------" << endl;
            continue;
        }
        
        struct JobResumeMatch {
            int resumeId;
            int score;
        };
        
        JobResumeMatch* matches = new JobResumeMatch[candidateResumeIds.size()];
        int matchCount = 0;
        
        // Track memory: much smaller array (only candidates, not all resumes)
        size_t iterationMemory = candidateResumeIds.size() * sizeof(JobResumeMatch);
        totalMemoryUsed += iterationMemory;
        if (iterationMemory > peakMemoryUsed) {
            peakMemoryUsed = iterationMemory;
        }
        
        for (int resumeId : candidateResumeIds) {
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
        
        // SELECTION SORT to sort matches by score
        for (int k = 0; k < matchCount - 1; k++) {
            int maxIndex = k;
            for (int m = k + 1; m < matchCount; m++) {
                if (matches[m].score > matches[maxIndex].score) {
                    maxIndex = m;
                }
            }
            if (maxIndex != k) {
                JobResumeMatch temp = matches[k];
                matches[k] = matches[maxIndex];
                matches[maxIndex] = temp;
            }
        }
        
        int bestScore = matches[0].score;
        string resumeIds = "";
        bool firstId = true;
        
        for (int j = 0; j < matchCount; j++) {
            if (matches[j].score == bestScore) {
                if (!firstId) resumeIds += ", ";
                resumeIds += to_string(matches[j].resumeId);
                firstId = false;
            } else {
                break;
            }
        }
        
        cout << "\nJob ID: " << currentJob.id << endl;
        cout << "Job Title: " << currentJob.title << endl;
        cout << "Job Skills: " << currentJob.skills << endl;
        cout << "Resume ID: " << resumeIds << endl;
        cout << "Best Score: " << bestScore << endl;
        cout << "Candidates Found: " << matchCount << " (from " << candidateResumeIds.size() << " candidates)" << endl;
        cout << "----------------------------------------" << endl;
        
        delete[] matches;
        processedJobs++;
        
        if (processedJobs % 100 == 0) {
            cout << "[Progress: " << processedJobs << "/" << min(maxJobsToShow, size) << " jobs processed]" << endl;
        }
    }
    
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    double durationSeconds = duration.count() / 1000.0;
    
    cout << "\n=== Performance Results ===" << endl;
    cout << "Processed " << processedJobs << " jobs in " << duration.count() << " ms (" << fixed << setprecision(3) << durationSeconds << " seconds)" << endl;
    cout << "Average time per job: " << fixed << setprecision(1) << (processedJobs > 0 ? duration.count() / processedJobs : 0) << " ms" << endl;
    cout << "Performance: " << fixed << setprecision(2) << (processedJobs / durationSeconds) << " jobs/second" << endl;
    
    // Display memory usage
    double indexMemoryMB = indexMemory / (1024.0 * 1024.0);
    double peakMemoryMB = peakMemoryUsed / (1024.0 * 1024.0);
    double avgMemoryMB = (processedJobs > 0 ? totalMemoryUsed / processedJobs : 0) / (1024.0 * 1024.0);
    cout << "\n=== Memory Usage ===" << endl;
    cout << "Inverted index size: " << fixed << setprecision(2) << indexMemoryMB << " MB (one-time preprocessing cost)" << endl;
    cout << "Peak memory per iteration: " << fixed << setprecision(2) << peakMemoryMB << " MB (candidates only)" << endl;
    cout << "Average memory per job: " << fixed << setprecision(2) << avgMemoryMB << " MB" << endl;
    cout << "Total temporary memory allocated: " << fixed << setprecision(2) << (totalMemoryUsed / (1024.0 * 1024.0)) << " MB" << endl;
    cout << "Total memory (index + temporary): " << fixed << setprecision(2) << (indexMemoryMB + totalMemoryUsed / (1024.0 * 1024.0)) << " MB" << endl;
}

// Search jobs by title using BOOLEAN SEARCH
template<>
void Array<Job>::displayMatchesByTitle(const string& titleKeyword, int maxResults) const {
    // Start timing
    auto startTime = chrono::high_resolution_clock::now();
    
    if (!indexBuilt) {
        cout << "Building inverted index for Boolean Search..." << endl;
        const_cast<Array<Job>*>(this)->buildIndex();
    }
    
    cout << "\nUsing: Boolean Search (Inverted Index) for Title Matching" << endl;
    
    set<int> candidateIds = searchIndex(titleKeyword, titleIndex);
    
    if (candidateIds.empty()) {
        cout << "No jobs found with title containing '" << titleKeyword << "'" << endl;
        return;
    }
    
    Match* matches = new Match[candidateIds.size()];
    int matchCount = 0;
    
    string normTitle = normalizeText(titleKeyword);
    
    for (int docId : candidateIds) {
        int score = 0;
        
        string normJobTitle = normalizeText(dataArray[docId].title);
        string normDesc = normalizeText(dataArray[docId].getText());
        
        if (normJobTitle.find(normTitle) != string::npos) {
            score += 20;
        }
        if (normDesc.find(normTitle) != string::npos) {
            score += 10;
        }
        
        istringstream iss(normTitle);
        string word;
        while (iss >> word) {
            if (normJobTitle.find(word) != string::npos) {
                score += 5;
            }
            if (normDesc.find(word) != string::npos) {
                score += 2;
            }
        }
        
        if (score > 0) {
            matches[matchCount].index = docId;
            matches[matchCount].score = score;
            matchCount++;
        }
    }
    
    if (matchCount == 0) {
        cout << "No jobs found with title containing '" << titleKeyword << "'" << endl;
        delete[] matches;
        return;
    }
    
    // Use SELECTION SORT
    selectionSort(matches, matchCount);
    
    // End timing
    auto endTime = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    
    int resultsToShow = min(maxResults, matchCount);
    cout << "\n=== Top " << resultsToShow << " Job Title Matches for '" << titleKeyword << "' ===" << endl;
    
    for (int i = 0; i < resultsToShow; i++) {
        cout << "\nMatch " << (i + 1) << " (Score: " << matches[i].score << "):" << endl;
        dataArray[matches[i].index].display();
    }
    
    // Display statistics at the end
    cout << "\n==========================================";
    cout << "\nJobs Processed: " << candidateIds.size() << " jobs (from " << size << " total jobs)";
    cout << "\nJobs with Scores: " << matchCount;
    cout << "\nTime Taken: " << duration.count() << " ms (" << fixed << setprecision(3) << (duration.count() / 1000.0) << " seconds)";
    cout << "\n==========================================\n" << endl;
    
    delete[] matches;
}

// External compatibility function
int calculateCompatibility(const Job& job, const Resume& resume) {
    int score = 0;
    
    string jobSkills = job.getSkills();
    string resumeSkills = resume.getSkills();
    
    transform(jobSkills.begin(), jobSkills.end(), jobSkills.begin(), ::tolower);
    transform(resumeSkills.begin(), resumeSkills.end(), resumeSkills.begin(), ::tolower);
    
    istringstream jobStream(jobSkills);
    string skill;
    while (jobStream >> skill) {
        if (resumeSkills.find(skill) != string::npos) {
            score += 5;
        }
    }
    
    return score;
}

// Explicit template instantiations
template class Array<Job>;
template class Array<Resume>;

