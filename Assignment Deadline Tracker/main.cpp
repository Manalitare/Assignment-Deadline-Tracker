#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cctype>
#include <ctime>

using namespace std;

// =============================
// STRUCTURES
// =============================

struct User
{
    int id;
    string name;
    string password;
    string role;
};

struct Assignment
{
    int id;
    int userId;
    string subject;
    string title;
    string description;
    string deadline;
    string priority;
    string status;
};

// =============================
// FILES
// =============================

const string USERS_FILE = "users.txt";
const string ASSIGNMENTS_FILE = "assignments.txt";
const string STATUS_FILE = "status.txt";

// =============================
// HELPER FUNCTIONS
// =============================

void clearInput()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string toLowerCase(string text)
{
    transform(text.begin(), text.end(), text.begin(),
              [](unsigned char c)
              {
                  return tolower(c);
              });

    return text;
}

bool isValidDate(string date)
{
    if (date.length() != 10)
        return false;

    return date[4] == '-' && date[7] == '-';
}

// =============================
// CREATE FILES
// =============================

void createFiles()
{
    ofstream(USERS_FILE, ios::app).close();
    ofstream(ASSIGNMENTS_FILE, ios::app).close();
    ofstream(STATUS_FILE, ios::app).close();
}

// =============================
// USER FUNCTIONS
// =============================

vector<User> loadUsers()
{
    vector<User> users;
    ifstream file(USERS_FILE);

    string line;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);

        User user;
        string idText;

        getline(ss, idText, '|');
        getline(ss, user.name, '|');
        getline(ss, user.password, '|');
        getline(ss, user.role);

        user.id = stoi(idText);

        if (user.role.empty())
            user.role = "Student";

        users.push_back(user);
    }

    return users;
}

void saveUser(const User& user)
{
    ofstream file(USERS_FILE, ios::app);

    file << user.id << "|"
         << user.name << "|"
         << user.password << "|"
         << user.role << endl;
}

int getNextUserId()
{
    vector<User> users = loadUsers();

    int maxId = 0;

    for (const User& user : users)
    {
        maxId = max(maxId, user.id);
    }

    return maxId + 1;
}

bool userExists(const string& name)
{
    vector<User> users = loadUsers();

    for (const User& user : users)
    {
        if (user.name == name)
            return true;
    }

    return false;
}

// =============================
// REGISTER
// =============================

void registerUser()
{
    string name;
    string password;

    cout << "\n===== USER REGISTRATION =====\n";

    cout << "Enter username: ";
    cin >> name;

    if (userExists(name))
    {
        cout << "Username already exists!\n";
        return;
    }

    cout << "Enter password: ";
    cin >> password;

    User user;

    user.id = getNextUserId();
    user.name = name;
    user.password = password;
    user.role = "Student";

    saveUser(user);

    cout << "\nRegistration successful!\n";
    cout << "Your User ID: " << user.id << endl;
}

// =============================
// LOGIN
// =============================

bool loginUser(User& loggedInUser)
{
    string name;
    string password;

    cout << "\n===== LOGIN =====\n";

    cout << "Enter username: ";
    cin >> name;

    cout << "Enter password: ";
    cin >> password;

    vector<User> users = loadUsers();

    for (const User& user : users)
    {
        if (user.name == name && user.password == password)
        {
            loggedInUser = user;

            cout << "\nLogin successful!\n";
            cout << "Welcome, " << user.name << "!\n";

            return true;
        }
    }

    cout << "\nInvalid username or password!\n";

    return false;
}

// =============================
// TEACHER LOGIN
// =============================

bool teacherLogin(User& loggedInUser)
{
    string name;
    string password;

    cout << "\n===== TEACHER LOGIN =====\n";
    cout << "Username: ";
    cin >> name;
    cout << "Password: ";
    cin >> password;

    // Simple fixed teacher account for the college project
    if (name == "teacher" && password == "1234")
    {
        loggedInUser.id = 0;
        loggedInUser.name = "teacher";
        loggedInUser.password = password;
        loggedInUser.role = "Teacher";

        cout << "\nTeacher login successful!\n";
        return true;
    }

    cout << "\nInvalid teacher username or password!\n";
    return false;
}

// =============================
// ASSIGNMENT FUNCTIONS
// =============================

vector<Assignment> loadAssignments()
{
    vector<Assignment> assignments;

    ifstream file(ASSIGNMENTS_FILE);

    string line;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);

        Assignment assignment;

        string idText;
        string userIdText;

        getline(ss, idText, '|');
        getline(ss, userIdText, '|');
        getline(ss, assignment.subject, '|');
        getline(ss, assignment.title, '|');
        getline(ss, assignment.description, '|');
        getline(ss, assignment.deadline, '|');
        getline(ss, assignment.priority);

        assignment.id = stoi(idText);
        assignment.userId = stoi(userIdText);
        assignment.status = "Pending";

        assignments.push_back(assignment);
    }

    return assignments;
}

// =============================
// STATUS FUNCTIONS
// =============================

vector<pair<int, string>> loadStatuses()
{
    vector<pair<int, string>> statuses;

    ifstream file(STATUS_FILE);

    string line;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        stringstream ss(line);

        string idText;
        string status;

        getline(ss, idText, '|');
        getline(ss, status);

        int id = stoi(idText);

        bool found = false;

        for (auto& item : statuses)
        {
            if (item.first == id)
            {
                item.second = status;
                found = true;
                break;
            }
        }

        if (!found)
            statuses.push_back({id, status});
    }

    return statuses;
}

void applySavedStatuses(vector<Assignment>& assignments)
{
    vector<pair<int, string>> statuses = loadStatuses();

    for (Assignment& assignment : assignments)
    {
        for (const auto& item : statuses)
        {
            if (item.first == assignment.id)
            {
                assignment.status = item.second;
                break;
            }
        }
    }
}

void saveAllStatuses(const vector<Assignment>& assignments)
{
    ofstream file(STATUS_FILE);

    for (const Assignment& assignment : assignments)
    {
        file << assignment.id << "|"
             << assignment.status << endl;
    }
}

// =============================
// ASSIGNMENT ID
// =============================

int getNextAssignmentId()
{
    vector<Assignment> assignments = loadAssignments();

    int maxId = 100;

    for (const Assignment& assignment : assignments)
    {
        maxId = max(maxId, assignment.id);
    }

    return maxId + 1;
}

// =============================
// SAVE ASSIGNMENT
// =============================

void saveAssignment(const Assignment& assignment)
{
    ofstream file(ASSIGNMENTS_FILE, ios::app);

    file << assignment.id << "|"
         << assignment.userId << "|"
         << assignment.subject << "|"
         << assignment.title << "|"
         << assignment.description << "|"
         << assignment.deadline << "|"
         << assignment.priority << endl;
}

// =============================
// ADD ASSIGNMENT
// =============================

void addAssignment(const User& currentUser)
{
    Assignment assignment;

    assignment.id = getNextAssignmentId();
    assignment.userId = currentUser.id;
    assignment.status = "Pending";

    clearInput();

    cout << "\n===== ADD ASSIGNMENT =====\n";

    cout << "Enter subject: ";
    getline(cin, assignment.subject);

    cout << "Enter assignment title: ";
    getline(cin, assignment.title);

    cout << "Enter description: ";
    getline(cin, assignment.description);

    do
    {
        cout << "Enter deadline (YYYY-MM-DD): ";
        getline(cin, assignment.deadline);

        if (!isValidDate(assignment.deadline))
            cout << "Please use YYYY-MM-DD format.\n";

    } while (!isValidDate(assignment.deadline));

    int choice;

    cout << "\nSelect Priority:\n";
    cout << "1. High\n";
    cout << "2. Medium\n";
    cout << "3. Low\n";
    cout << "Enter choice: ";
    cin >> choice;

    if (choice == 1)
        assignment.priority = "High";
    else if (choice == 2)
        assignment.priority = "Medium";
    else
        assignment.priority = "Low";

    saveAssignment(assignment);

    ofstream statusFile(STATUS_FILE, ios::app);

    statusFile << assignment.id << "|Pending\n";

    cout << "\nAssignment added successfully!\n";
    cout << "Assignment ID: " << assignment.id << endl;
}

// =============================
// DISPLAY ASSIGNMENT
// =============================

void displayAssignment(const Assignment& assignment)
{
    cout << "\n----------------------------------\n";
    cout << "Assignment ID : " << assignment.id << endl;
    cout << "Subject       : " << assignment.subject << endl;
    cout << "Title         : " << assignment.title << endl;
    cout << "Description   : " << assignment.description << endl;
    cout << "Deadline      : " << assignment.deadline << endl;
    cout << "Priority      : " << assignment.priority << endl;
    cout << "Status        : " << assignment.status << endl;
    cout << "----------------------------------\n";
}

// =============================
// VIEW
// =============================

void displayUserAssignments(const User& currentUser)
{
    vector<Assignment> assignments = loadAssignments();

    applySavedStatuses(assignments);

    bool found = false;

    cout << "\n===== MY ASSIGNMENTS =====\n";

    for (const Assignment& assignment : assignments)
    {
        if (currentUser.role == "Student" || assignment.userId == currentUser.id)
        {
            displayAssignment(assignment);
            found = true;
        }
    }

    if (!found)
        cout << "No assignments found.\n";
}

// =============================
// SEARCH
// =============================

void searchAssignment(const User& currentUser)
{
    vector<Assignment> assignments = loadAssignments();

    applySavedStatuses(assignments);

    clearInput();

    string keyword;

    cout << "\n===== SEARCH ASSIGNMENT =====\n";
    cout << "Enter subject or title: ";
    getline(cin, keyword);

    keyword = toLowerCase(keyword);

    bool found = false;

    for (const Assignment& assignment : assignments)
    {
        if (currentUser.role != "Student" && assignment.userId != currentUser.id)
            continue;

        if (toLowerCase(assignment.subject).find(keyword) != string::npos ||
            toLowerCase(assignment.title).find(keyword) != string::npos)
        {
            displayAssignment(assignment);
            found = true;
        }
    }

    if (!found)
        cout << "No matching assignment found.\n";
}

// =============================
// FILTER PRIORITY
// =============================

void filterByPriority(const User& currentUser)
{
    vector<Assignment> assignments = loadAssignments();

    applySavedStatuses(assignments);

    int choice;

    cout << "\n===== FILTER BY PRIORITY =====\n";
    cout << "1. High\n";
    cout << "2. Medium\n";
    cout << "3. Low\n";
    cout << "Enter choice: ";
    cin >> choice;

    string selected;

    if (choice == 1)
        selected = "High";
    else if (choice == 2)
        selected = "Medium";
    else if (choice == 3)
        selected = "Low";
    else
    {
        cout << "Invalid choice!\n";
        return;
    }

    bool found = false;

    for (const Assignment& assignment : assignments)
    {
        if ((currentUser.role == "Student" || assignment.userId == currentUser.id) &&
            assignment.priority == selected)
        {
            displayAssignment(assignment);
            found = true;
        }
    }

    if (!found)
        cout << "No assignments found.\n";
}

// =============================
// FILTER STATUS
// =============================

void filterByStatus(const User& currentUser)
{
    vector<Assignment> assignments = loadAssignments();

    applySavedStatuses(assignments);

    int choice;

    cout << "\n===== FILTER BY STATUS =====\n";
    cout << "1. Pending\n";
    cout << "2. In Progress\n";
    cout << "3. Completed\n";
    cout << "Enter choice: ";
    cin >> choice;

    string selected;

    if (choice == 1)
        selected = "Pending";
    else if (choice == 2)
        selected = "In Progress";
    else if (choice == 3)
        selected = "Completed";
    else
    {
        cout << "Invalid choice!\n";
        return;
    }

    bool found = false;

    for (const Assignment& assignment : assignments)
    {
        if ((currentUser.role == "Student" || assignment.userId == currentUser.id) &&
            assignment.status == selected)
        {
            displayAssignment(assignment);
            found = true;
        }
    }

    if (!found)
        cout << "No assignments found.\n";
}

// =============================
// UPDATE STATUS
// =============================

void updateAssignmentStatus(const User& currentUser)
{
    vector<Assignment> assignments = loadAssignments();

    applySavedStatuses(assignments);

    cout << "\n===== UPDATE ASSIGNMENT STATUS =====\n";

    for (const Assignment& assignment : assignments)
    {
        if (assignment.userId == currentUser.id)
        {
            cout << "ID: " << assignment.id
                 << " | " << assignment.title
                 << " | " << assignment.status << endl;
        }
    }

    int id;

    cout << "\nEnter Assignment ID: ";
    cin >> id;

    for (Assignment& assignment : assignments)
    {
        if (assignment.id == id &&
            (currentUser.role == "Student" || assignment.userId == currentUser.id))
        {
            int choice;

            cout << "\n1. Pending\n";
            cout << "2. In Progress\n";
            cout << "3. Completed\n";
            cout << "Enter choice: ";
            cin >> choice;

            if (choice == 1)
                assignment.status = "Pending";
            else if (choice == 2)
                assignment.status = "In Progress";
            else if (choice == 3)
                assignment.status = "Completed";
            else
            {
                cout << "Invalid choice!\n";
                return;
            }

            saveAllStatuses(assignments);

            cout << "\nStatus updated successfully!\n";
            return;
        }
    }

    cout << "Invalid Assignment ID!\n";
}

// =============================
// DELETE
// =============================

void deleteAssignment(const User& currentUser)
{
    vector<Assignment> assignments = loadAssignments();

    applySavedStatuses(assignments);

    cout << "\n===== DELETE ASSIGNMENT =====\n";

    for (const Assignment& assignment : assignments)
    {
        if (assignment.userId == currentUser.id)
        {
            cout << "ID: " << assignment.id
                 << " | " << assignment.title << endl;
        }
    }

    int id;

    cout << "\nEnter Assignment ID to delete: ";
    cin >> id;

    bool found = false;

    for (const Assignment& assignment : assignments)
    {
        if (assignment.id == id &&
            (currentUser.role == "Teacher" || assignment.userId == currentUser.id))
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        cout << "Invalid Assignment ID!\n";
        return;
    }

    int confirmation;

    cout << "\nAre you sure?\n";
    cout << "1. Yes\n";
    cout << "2. No\n";
    cout << "Enter choice: ";
    cin >> confirmation;

    if (confirmation != 1)
    {
        cout << "Deletion cancelled.\n";
        return;
    }

    vector<Assignment> remaining;

    for (const Assignment& assignment : assignments)
    {
        if (assignment.id != id)
            remaining.push_back(assignment);
    }

    ofstream assignmentFile(ASSIGNMENTS_FILE);

    for (const Assignment& assignment : remaining)
    {
        assignmentFile << assignment.id << "|"
                       << assignment.userId << "|"
                       << assignment.subject << "|"
                       << assignment.title << "|"
                       << assignment.description << "|"
                       << assignment.deadline << "|"
                       << assignment.priority << endl;
    }

    assignmentFile.close();

    saveAllStatuses(remaining);

    cout << "\nAssignment deleted successfully!\n";
}

// =============================
// DATE CONVERSION
// =============================

time_t convertDateToTime(string date)
{
    tm timeInfo = {};

    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));

    timeInfo.tm_year = year - 1900;
    timeInfo.tm_mon = month - 1;
    timeInfo.tm_mday = day;
    timeInfo.tm_hour = 0;
    timeInfo.tm_min = 0;
    timeInfo.tm_sec = 0;

    return mktime(&timeInfo);
}

// =============================
// DEADLINE STATUS
// =============================

string getDeadlineStatus(const Assignment& assignment)
{
    if (assignment.status == "Completed")
        return "COMPLETED";

    time_t currentTime = time(nullptr);

    tm todayInfo = *localtime(&currentTime);

    todayInfo.tm_hour = 0;
    todayInfo.tm_min = 0;
    todayInfo.tm_sec = 0;

    time_t today = mktime(&todayInfo);

    time_t deadline = convertDateToTime(assignment.deadline);

    double difference = difftime(deadline, today);

    int days = static_cast<int>(difference / (60 * 60 * 24));

    if (days < 0)
        return "OVERDUE";

    if (days == 0)
        return "DUE TODAY";

    if (days <= 7)
        return "DUE SOON";

    return "UPCOMING";
}

// =============================
// DASHBOARD
// =============================

void deadlineDashboard(const User& currentUser)
{
    vector<Assignment> assignments = loadAssignments();

    applySavedStatuses(assignments);

    int total = 0;
    int pending = 0;
    int inProgress = 0;
    int completed = 0;

    int high = 0;
    int medium = 0;
    int low = 0;

    int overdue = 0;
    int dueToday = 0;
    int dueSoon = 0;
    int upcoming = 0;

    cout << "\n========================================\n";
    cout << "          DEADLINE DASHBOARD\n";
    cout << "========================================\n";

    for (const Assignment& assignment : assignments)
    {
        if (currentUser.role != "Teacher" && assignment.userId != currentUser.id)
            continue;

        total++;

        // Status statistics
        if (assignment.status == "Pending")
            pending++;
        else if (assignment.status == "In Progress")
            inProgress++;
        else if (assignment.status == "Completed")
            completed++;

        // Priority statistics
        if (assignment.priority == "High")
            high++;
        else if (assignment.priority == "Medium")
            medium++;
        else if (assignment.priority == "Low")
            low++;

        // Deadline statistics
        string deadlineStatus = getDeadlineStatus(assignment);

        if (deadlineStatus == "OVERDUE")
            overdue++;
        else if (deadlineStatus == "DUE TODAY")
            dueToday++;
        else if (deadlineStatus == "DUE SOON")
            dueSoon++;
        else if (deadlineStatus == "UPCOMING")
            upcoming++;
    }

    cout << "\n----- ASSIGNMENT SUMMARY -----\n";
    cout << "Total Assignments : " << total << endl;
    cout << "Pending           : " << pending << endl;
    cout << "In Progress       : " << inProgress << endl;
    cout << "Completed         : " << completed << endl;

    cout << "\n----- PRIORITY SUMMARY -----\n";
    cout << "High Priority     : " << high << endl;
    cout << "Medium Priority   : " << medium << endl;
    cout << "Low Priority      : " << low << endl;

    cout << "\n----- DEADLINE SUMMARY -----\n";
    cout << "Overdue           : " << overdue << endl;
    cout << "Due Today         : " << dueToday << endl;
    cout << "Due Soon          : " << dueSoon << endl;
    cout << "Upcoming          : " << upcoming << endl;

    cout << "\n----- DEADLINE DETAILS -----\n";

    bool found = false;

    for (const Assignment& assignment : assignments)
    {
        if (currentUser.role == "Teacher" || assignment.userId == currentUser.id)
        {
            cout << assignment.id
                 << " | "
                 << assignment.title
                 << " | "
                 << assignment.deadline
                 << " | "
                 << getDeadlineStatus(assignment)
                 << endl;

            found = true;
        }
    }

    if (!found)
        cout << "No assignments available.\n";

    cout << "\n========================================\n";
}

// =============================
// REPORT FUNCTIONS
// =============================

string getCurrentDateTime()
{
    time_t now = time(nullptr);
    tm localTime = *localtime(&now);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);

    return string(buffer);
}

string pdfEscape(const string& text)
{
    string result;

    for (char c : text)
    {
        if (c == '(' || c == ')' || c == '\\')
            result += '\\';

        if (c == '\n' || c == '\r')
            result += ' ';
        else if (static_cast<unsigned char>(c) >= 32 &&
                 static_cast<unsigned char>(c) <= 126)
            result += c;
        else
            result += ' ';
    }

    return result;
}

void createSimplePDF(const string& pdfFile, const vector<string>& lines)
{
    const int linesPerPage = 42;
    int pageCount = static_cast<int>((lines.size() + linesPerPage - 1) / linesPerPage);

    if (pageCount == 0)
        pageCount = 1;

    // Object numbers:
    // 1 = Catalog
    // 2 = Pages
    // 3 = Font
    // Then every page uses two objects: Page + Content
    int objectCount = 3 + pageCount * 2;

    vector<string> objects(objectCount + 1);

    objects[1] = "<< /Type /Catalog /Pages 2 0 R >>";

    string kids = "";
    for (int page = 0; page < pageCount; page++)
    {
        int pageObject = 4 + page * 2;
        kids += to_string(pageObject) + " 0 R ";
    }

    objects[2] = "<< /Type /Pages /Kids [" + kids + "] /Count " +
                 to_string(pageCount) + " >>";

    objects[3] = "<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>";

    for (int page = 0; page < pageCount; page++)
    {
        int pageObject = 4 + page * 2;
        int contentObject = pageObject + 1;

        ostringstream content;
        content << "BT\n";
        content << "/F1 10 Tf\n";
        content << "50 750 Td\n";

        int start = page * linesPerPage;
        int end = min(start + linesPerPage, static_cast<int>(lines.size()));

        for (int i = start; i < end; i++)
        {
            content << "(" << pdfEscape(lines[i]) << ") Tj\n";
            content << "0 -17 Td\n";
        }

        content << "ET\n";

        string contentText = content.str();

        objects[pageObject] =
            "<< /Type /Page /Parent 2 0 R "
            "/MediaBox [0 0 612 792] "
            "/Resources << /Font << /F1 3 0 R >> >> "
            "/Contents " + to_string(contentObject) + " 0 R >>";

        objects[contentObject] =
            "<< /Length " + to_string(contentText.size()) + " >>\n"
            "stream\n" + contentText + "endstream";
    }

    ofstream file(pdfFile, ios::binary);

    if (!file)
        return;

    file << "%PDF-1.4\n";

    vector<long long> offsets(objectCount + 1, 0);

    for (int i = 1; i <= objectCount; i++)
    {
        offsets[i] = static_cast<long long>(file.tellp());
        file << i << " 0 obj\n";
        file << objects[i] << "\n";
        file << "endobj\n";
    }

    long long xrefPosition = static_cast<long long>(file.tellp());

    file << "xref\n";
    file << "0 " << objectCount + 1 << "\n";
    file << "0000000000 65535 f \n";

    for (int i = 1; i <= objectCount; i++)
    {
        file.width(10);
        file.fill('0');
        file << offsets[i];
        file << " 00000 n \n";
    }

    file << "trailer\n";
    file << "<< /Size " << objectCount + 1 << " /Root 1 0 R >>\n";
    file << "startxref\n";
    file << xrefPosition << "\n";
    file << "%%EOF\n";

    file.close();
}

vector<string> buildReportLines(const string& reportTitle,
                                const User& currentUser,
                                bool teacherReport)
{
    vector<Assignment> assignments = loadAssignments();
    applySavedStatuses(assignments);

    vector<string> lines;

    lines.push_back("ASSIGNMENT DEADLINE TRACKER");
    lines.push_back(reportTitle);
    lines.push_back("User: " + currentUser.name);
    lines.push_back("Generated: " + getCurrentDateTime());
    lines.push_back("-----------------------------------------------");

    int total = 0;
    int pending = 0;
    int inProgress = 0;
    int completed = 0;
    int high = 0;
    int medium = 0;
    int low = 0;
    int overdue = 0;
    int dueToday = 0;
    int dueSoon = 0;
    int upcoming = 0;

    for (const Assignment& assignment : assignments)
    {
        if (!teacherReport && assignment.userId != currentUser.id)
            continue;

        total++;

        if (assignment.status == "Pending")
            pending++;
        else if (assignment.status == "In Progress")
            inProgress++;
        else if (assignment.status == "Completed")
            completed++;

        if (assignment.priority == "High")
            high++;
        else if (assignment.priority == "Medium")
            medium++;
        else if (assignment.priority == "Low")
            low++;

        string deadlineStatus = getDeadlineStatus(assignment);

        if (deadlineStatus == "OVERDUE")
            overdue++;
        else if (deadlineStatus == "DUE TODAY")
            dueToday++;
        else if (deadlineStatus == "DUE SOON")
            dueSoon++;
        else if (deadlineStatus == "UPCOMING")
            upcoming++;
    }

    lines.push_back("SUMMARY");
    lines.push_back("Total Assignments : " + to_string(total));
    lines.push_back("Pending           : " + to_string(pending));
    lines.push_back("In Progress       : " + to_string(inProgress));
    lines.push_back("Completed         : " + to_string(completed));
    lines.push_back("High Priority     : " + to_string(high));
    lines.push_back("Medium Priority   : " + to_string(medium));
    lines.push_back("Low Priority      : " + to_string(low));
    lines.push_back("Overdue           : " + to_string(overdue));
    lines.push_back("Due Today         : " + to_string(dueToday));
    lines.push_back("Due Soon          : " + to_string(dueSoon));
    lines.push_back("Upcoming          : " + to_string(upcoming));
    lines.push_back("-----------------------------------------------");
    lines.push_back("ASSIGNMENT DETAILS");

    bool found = false;

    for (const Assignment& assignment : assignments)
    {
        if (!teacherReport && assignment.userId != currentUser.id)
            continue;

        found = true;
        lines.push_back("ID: " + to_string(assignment.id));
        lines.push_back("Subject: " + assignment.subject);
        lines.push_back("Title: " + assignment.title);
        lines.push_back("Description: " + assignment.description);
        lines.push_back("Deadline: " + assignment.deadline);
        lines.push_back("Priority: " + assignment.priority);
        lines.push_back("Status: " + assignment.status);
        lines.push_back("Deadline Status: " + getDeadlineStatus(assignment));
        lines.push_back("-----------------------------------------------");
    }

    if (!found)
        lines.push_back("No assignments available.");

    return lines;
}

void generateTeacherReport(const User& currentUser)
{
    vector<string> lines = buildReportLines("TEACHER REPORT", currentUser, true);

    ofstream txtFile("teacher_report.txt");

    for (const string& line : lines)
        txtFile << line << endl;

    txtFile.close();

    cout << "\nTeacher report created: teacher_report.txt\n";
}

void generateTeacherPDF(const User& currentUser)
{
    vector<string> lines = buildReportLines("TEACHER REPORT", currentUser, true);

    createSimplePDF("teacher_report.pdf", lines);

    cout << "\nTeacher PDF created: teacher_report.pdf\n";
}

void generateStudentReport(const User& currentUser)
{
    vector<string> lines = buildReportLines("STUDENT REPORT", currentUser, false);

    ofstream txtFile("student_report.txt");

    for (const string& line : lines)
        txtFile << line << endl;

    txtFile.close();

    cout << "\nStudent report created: student_report.txt\n";
}

void generateStudentPDF(const User& currentUser)
{
    vector<string> lines = buildReportLines("STUDENT REPORT", currentUser, false);

    createSimplePDF("student_report.pdf", lines);

    cout << "\nStudent PDF created: student_report.pdf\n";
}

// =============================
// STUDENT MENU
// =============================

void studentMenu(const User& currentUser)
{
    int choice;

    while (true)
    {
        cout << "\n====================================\n";
        cout << "          STUDENT DASHBOARD\n";
        cout << "====================================\n";
        cout << "Logged in as: " << currentUser.name << endl;

        cout << "\n1. View Assignments\n";
        cout << "2. Search Assignment\n";
        cout << "3. Filter by Priority\n";
        cout << "4. Filter by Status\n";
        cout << "5. Update Assignment Status\n";
        cout << "6. Deadline Dashboard\n";
        cout << "7. Generate Student Report\n";
        cout << "8. Generate Student PDF\n";
        cout << "9. Logout\n";
        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1: displayUserAssignments(currentUser); break;
            case 2: searchAssignment(currentUser); break;
            case 3: filterByPriority(currentUser); break;
            case 4: filterByStatus(currentUser); break;
            case 5: updateAssignmentStatus(currentUser); break;
            case 6: deadlineDashboard(currentUser); break;
            case 7: generateStudentReport(currentUser); break;
            case 8: generateStudentPDF(currentUser); break;
            case 9:
                cout << "\nLogging out...\n";
                return;
            default:
                cout << "\nInvalid choice!\n";
        }
    }
}

// =============================
// TEACHER MENU
// =============================

void teacherMenu(const User& currentUser)
{
    int choice;

    while (true)
    {
        cout << "\n====================================\n";
        cout << "          TEACHER DASHBOARD\n";
        cout << "====================================\n";
        cout << "Logged in as: " << currentUser.name << endl;

        cout << "\n1. Add Assignment\n";
        cout << "2. View Assignments\n";
        cout << "3. Update Assignment Status\n";
        cout << "4. Delete Assignment\n";
        cout << "5. Deadline Dashboard\n";
        cout << "6. Generate Teacher Report\n";
        cout << "7. Generate Teacher PDF\n";
        cout << "8. Logout\n";
        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1: addAssignment(currentUser); break;
            case 2: displayUserAssignments(currentUser); break;
            case 3: updateAssignmentStatus(currentUser); break;
            case 4: deleteAssignment(currentUser); break;
            case 5: deadlineDashboard(currentUser); break;
            case 6: generateTeacherReport(currentUser); break;
            case 7: generateTeacherPDF(currentUser); break;
            case 8:
                cout << "\nLogging out...\n";
                return;
            default:
                cout << "\nInvalid choice!\n";
        }
    }
}

// =============================
// MAIN
// =============================

int main()
{
    createFiles();

    int choice;

    while (true)
    {
        cout << "\n========================================\n";
        cout << "      ASSIGNMENT DEADLINE TRACKER\n";
        cout << "========================================\n";
        cout << "\n1. Teacher Login\n";
        cout << "2. Student Registration\n";
        cout << "3. Student Login\n";
        cout << "4. Exit\n";
        cout << "\nEnter choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
            {
                User teacher;
                if (teacherLogin(teacher))
                    teacherMenu(teacher);
                break;
            }

            case 2:
                registerUser();
                break;

            case 3:
            {
                User student;
                if (loginUser(student))
                    studentMenu(student);
                break;
            }

            case 4:
                cout << "\nThank you for using Assignment Deadline Tracker!\n";
                return 0;

            default:
                cout << "\nInvalid choice!\n";
        }
    }

    return 0;
}
