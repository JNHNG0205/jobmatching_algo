# Job Matching System (Rule-Based)

A C++ application that matches job descriptions with resumes using custom array data structures and rule-based algorithms.

## Features

- **Data Cleaning**: Automatically filters and normalizes job descriptions and resumes
- **Skill Matching**: Uses whitelist-based filtering to identify technical skills
- **Interactive Menu**: Multiple search and matching options
- **Custom Data Structures**: Implements dynamic arrays with template support
- **CSV Processing**: Loads and processes job and resume data from CSV files

## Project Structure

```
jobmatching_algo/
├── main.cpp                       # Main program with job matching logic
├── array.cpp                      # Core data structures and classes
├── data_cleaning_standalone.cpp   # **RECOMMENDED** data cleaning utility
├── data_cleaning.cpp              # Alternative data cleaning utility
├── data/                          # Data directory
│   ├── job_description.csv        # Original job data
│   ├── job_description_clean.csv  # Cleaned job data
│   ├── resume.csv                 # Original resume data
│   └── resume_clean.csv           # Cleaned resume data
└── README.md                      # This file
```

## Quick Start

### Option 1: Use Pre-compiled Executables (Recommended)

The project includes working executables that you can run immediately:

```bash
# Clean data (first time setup)
./data_cleaning.exe

# Run main program
./program.exe
```

### Option 2: Compile from Source

If you want to compile from source:

```bash
# Compile main program
g++ -std=c++17 -static main.cpp -o program

# Compile data cleaning utility (recommended method)
g++ -std=c++17 -static data_cleaning.cpp -o data_cleaning

**Note:** If you encounter linker errors (ld returned 5/116 exit status), use the pre-compiled executables instead.

## Usage

### Interactive Menu Options

1. **Search Jobs by Skills** - Find jobs matching specific skills
2. **Search Resumes by Skills** - Find resumes with specific skills
3. **Filter Resumes with Specific Job** - Match candidates for a job description
4. **Show Best Matches for Each Job** - Display top resume matches for all jobs
5. **Clean Data (Regenerate Cleaned CSVs)** - Re-run data cleaning
6. **Exit** - Quit the program

### Data Cleaning Process

The data cleaning utility:
- Extracts first sentences from job descriptions and resumes
- Parses job titles (text before "needed")
- Extracts skills (text after "experience in" or "skilled in")
- Filters skills using a comprehensive whitelist
- Normalizes titles (removes quotes/punctuation)
- Generates clean CSV files with unique IDs

## Data Format

### Input CSV Format

**Job Description CSV:**
- First line: Header (ignored)
- Each line: Job description with "needed" and "experience in" keywords
- Example: `"Data Analyst needed with experience in SQL, Excel, Power BI, Reporting, Data Cleaning, Tableau. Additional text..."`

**Resume CSV:**
- First line: Header (ignored)
- Each line: Resume description with "skilled in" keyword
- Example: `"Experienced professional skilled in SQL, Power BI, Excel, Tableau. Additional text..."`

### Cleaned CSV Format

**Job Description Clean:**
- `Job_ID,Title,Skills`
- Example: `1,Data Analyst,"SQL, Excel, Power BI, Reporting, Data Cleaning, Tableau"`

**Resume Clean:**
- `Resume_ID,Skills`
- Example: `1,"SQL, Power BI, Excel, Tableau"`

## Technical Skills Supported

The system recognizes 50+ technical skills including:

- **Programming**: SQL, Python, Java, JavaScript, C++, C#, R, Scala, Go, Rust
- **Data Tools**: Power BI, Tableau, Excel, Pandas, NumPy, Matplotlib, Seaborn
- **ML/AI**: Machine Learning, Deep Learning, NLP, Computer Vision, Statistics, TensorFlow, PyTorch, Keras, Scikit-learn, MLOps
- **Web Development**: REST APIs, Spring Boot, React, Angular, Vue, Node.js, Express, Django, Flask
- **DevOps**: Docker, Kubernetes, Git, Agile, Scrum, System Design, Microservices, AWS, Azure, GCP, Cloud
- **Databases**: MongoDB, PostgreSQL, MySQL, Redis, Elasticsearch
- **Management**: Product Roadmap, User Stories, Stakeholder Management, Project Management

## Algorithm Details

### Matching Algorithm
- **Skill Matching**: +5 points per matching skill
- **Title Matching**: +10 points for job title in resume
- **Text Matching**: +5 points for keyword in description
- **Word Overlap**: +2 points per overlapping word

## Distribution Package

### Essential Files to Include:
- **Executables (Recommended):**
  - `program.exe` - **Working** main job matching program
  - `data_cleaning.exe` - **Working** data cleaning utility

- **Source Code:**
  - `main.cpp` - Main job matching program source
  - `array.cpp` - Core data structures source
  - `data_cleaning.cpp` - Alternative data cleaning utility source

- **Data Files:**
  - `data/job_description.csv` - Original job data
  - `data/resume.csv` - Original resume data
  - `data/job_description_clean.csv` - Pre-cleaned job data (optional)
  - `data/resume_clean.csv` - Pre-cleaned resume data (optional)

- **Documentation:**
  - `README.md` - This file

## Why data_cleaning_standalone.cpp is Recommended:

1. **Self-contained** - No external dependencies
2. **No linker issues** - Avoids duplicate symbol problems
3. **Easy compilation** - Single file compilation
4. **Same functionality** - Identical to original
5. **Cross-platform** - Works on Windows/Linux/macOS

## Troubleshooting

### Common Issues

1. **"Unable to open file" error**
   - Ensure CSV files exist in `data/` directory
   - Run data cleaning first: `./data_cleaning`

2. **No matches found**
   - Verify skills are in the supported whitelist
   - Check if cleaned data exists

### Rebuilding

If you modify the source code:

```bash
# Rebuild main program
g++ -std=c++17 -static main.cpp -o program

# Rebuild data cleaning utility
g++ -std=c++17 -static data_cleaning.cpp -o data_cleaning
```

## System Requirements

- C++17 compatible compiler (g++, clang++, etc.)
- Windows/Linux/macOS
- Terminal/Command Prompt access

## Performance

- Handles 10,000+ job and resume records
- Optimized with O2 compilation flag
- Memory-efficient custom array implementation
- Progress indicators for large operations

## Contributing

To add new technical skills:
1. Edit the `technicalSkills` array in `array.cpp` (lines 139-151 and 248-260)
2. Rebuild the programs
3. Re-run data cleaning

## Complete Workflow for Recipients

### Option 1: Using Pre-compiled Executables (Recommended)
1. Extract all files to a directory
2. Open terminal in that directory
3. Run: `./data_cleaning.exe` (if needed)
4. Run: `./program.exe`

### Option 2: Compiling from Source
1. Extract all files to a directory
2. Open terminal in that directory
3. Compile: `g++ -std=c++17 -static main.cpp -o program`
4. Compile: `g++ -std=c++17 -static data_cleaning.cpp -o data_cleaning`
5. Run: `./data_cleaning` (if needed)
6. Run: `./program`

**Note:** If compilation fails with linker errors, use the pre-compiled executables instead.

## License

This project is for educational purposes.
