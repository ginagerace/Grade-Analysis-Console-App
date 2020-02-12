/*main.cpp*/

//
// <Gina Gerace>
// U. of Illinois, Chicago
// CS 341, Fall 2019
// Project #03: GradeUtil UI
// This program is C++11 dependent
//

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

// includes for gradeutil
#include "gradeutil.h"

College InputGradeData(string filename)
{
    College college;
    ifstream file(filename);
    string line, value;

    if (!file.good())
    {
        cout << "**Error: unable to open input file '" << filename << "'." << endl;
        return college;
    }

    // first line contains semester,year
    getline(file, line);
    stringstream ss(line);

    getline(ss, college.Name, ',');
    getline(ss, college.Semester, ',');
    getline(ss, value);
    college.Year = stoi(value);

    // second line contains column headers --- skip
    getline(file, line);

    //
    // now start inputting and parse course data:
    //

    while (getline(file, line))
    {
        Course c = ParseCourse(line);

        //
        // search for correct dept to ask course to, otherwise create a new dept:
        //
        auto dept_iter = std::find_if(college.Depts.begin(),
                                      college.Depts.end(),
                                      [&](const Dept &d) {
                                          return (d.Name == c.Dept);
                                      });

        if (dept_iter == college.Depts.end())
        {
            //
            // doesn't exist, so we have to create a new dept
            // and insert course:
            //
            Dept d(c.Dept);

            d.Courses.push_back(c);

            college.Depts.push_back(d);
        }
        else
        {
            // dept exists, so insert course into existing dept:
            dept_iter->Courses.push_back(c);
        }

    } //while

    //
    // done:
    //
    return college;
}

// TODO: define your own functions

//count the number of courses in a department
int countCourses(Dept &d){
  int count = 0;
  for(Course c : d.Courses)
    count++;
  return count;
}

//print the grade distribution for a course
void gradeDist(Course &c){
  GradeStats g = GetGradeDistribution(c);
  cout << g.PercentA << "%, " << g.PercentB << "%, "
       << g.PercentC << "%, " << g.PercentD << "%, "
       << g.PercentF << "%" << endl;
}

//print the grade distribution for a department
void gradeDist(Dept &d){
  GradeStats g = GetGradeDistribution(d);
  cout << g.PercentA << "%, " << g.PercentB << "%, "
       << g.PercentC << "%, " << g.PercentD << "%, "
       << g.PercentF << "%" << endl;
}

//print the grade distribution for a department
void collegeGradeDist(College &c){
  GradeStats g = GetGradeDistribution(c);
  cout << g.PercentA << "%, " << g.PercentB << "%, "
       << g.PercentC << "%, " << g.PercentD << "%, "
       << g.PercentF << "%" << endl;
}

//prints the search data for a course
void printCourse(Course &c){
  cout << c.Dept << " " << c.Number << " (section " << c.Section << "): " << c.Instructor << endl; 
  cout << " # students: " << c.getNumStudents() << endl;
  
  //determine course type
  string type;
  if(c.getGradingType() == Course::Letter)
      type = "letter";
  else if(c.getGradingType() == Course::Satisfactory)
      type = "satisfactory";
  else
      type = "unknown";
  
  cout << " course type: " << type << endl;
  cout << " grade distribution (A-F): "; gradeDist(c);
  int DFW, N;
  cout << " DFW rate: " << GetDFWRate(c, DFW, N) << "%" << endl;
}

//prints the summary data for a department
void printDept(Dept &d){
  cout << d.Name << ":" << endl;
  cout << " # courses taught: " << countCourses(d) << endl;
  cout << " # students taught: " << d.NumStudents() << endl;
  cout << " grade distribution (A-F): "; gradeDist(d);
  int DFW, N;
  cout << " DFW rate: " << GetDFWRate(d, DFW, N) << "%" << endl;
}

//search a department by course number or instructor prefix
void searchDept(Dept &d, string find){
  int num;
  vector<Course> courses;
  //determine if user input an instructor prefix or course number
  if(!isdigit(find[0]))       //input is a prefix
    courses = FindCourses(d, find);
  else {                      //input is a number
    num = stoi(find);
    courses = FindCourses(d, num);
  }
  if(courses.empty())
    cout << "**none found" << endl;
  else{
    for(Course c : courses)
      printCourse(c);
  }
}

//search a department by course number or instructor prefix
void searchCollege(College &col, string find){
  int num; 
  vector<Course> courses;
  //determine if user input an instructor prefix or course number
  if(!isdigit(find[0]))       //input is a prefix
    courses = FindCourses(col, find);
  else {                      //input is a number
    num = stoi(find);
    courses = FindCourses(col, num);
  }
  if(courses.empty())
    cout << "**none found" << endl;
  else{
    for(Course c : courses)
      printCourse(c);
  }
}

//sorts the courses in a department and prints all of type satisfactory
void printSatisfactory(Dept &d, bool print){
  
  //create a vector to sort all satisfactory courses
  vector<Course> courses;
  for(Course c : d.Courses){
    //if(((c.NumA + c.NumB + c.NumC + c.NumD + c.NumF) <= 0) && ((c.NumS + c.NumU) > 0))  
    if(c.getGradingType() == Course::Satisfactory) 
      courses.push_back(c);
  }
  
  //sort the satisfactory courses
  if(courses.size()>1){
    sort(courses.begin(), courses.end(),
         [](const Course& c1, const Course& c2)
         {
           if (c1.Dept < c2.Dept)
             return true;
           else if (c1.Dept > c2.Dept)
             return false;
           else{ // same dept, look at course #:
             if (c1.Number < c2.Number)
               return true;
             else if (c1.Number > c2.Number)
               return false;
             else{ // same course #, look at section #:
               if (c1.Section < c2.Section)
                 return true;
               else 
                 return false;
             }
           }
         });
   }
   if(courses.empty() && print)
    cout << "**none found" << endl;
   else{  
    //print the satisfactory courses data
    for(Course c : courses){
      cout << c.Dept << " " << c.Number << " (section " << c.Section << "): " << c.Instructor << endl; 
      cout << " # students: " << c.getNumStudents() << endl;
      cout << " course type: satisfactory" << endl;
    }
  }
}

//sort the courses by DFW rate, dept, course #, section #
void sortCourses(vector<Course> &courses){
  if(courses.size()>1){
    sort(courses.begin(), courses.end(),
         [](const Course& c1, const Course& c2)
         {
           int DFW, N;
           if(GetDFWRate(c1, DFW, N) > GetDFWRate(c2, DFW, N))
             return true;
           else if (GetDFWRate(c1, DFW, N) < GetDFWRate(c2, DFW, N))
             return false;
           else{
             if (c1.Dept < c2.Dept)
               return true;
             else if (c1.Dept > c2.Dept)
               return false;
             else{ // same dept, look at course #:
               if (c1.Number < c2.Number)
                 return true;
               else if (c1.Number > c2.Number)
                 return false;
               else{ // same course #, look at section #:
                 if (c1.Section < c2.Section)
                   return true;
                 else 
                   return false;
               }
              }
            }
         });
   }
}

//sort the courses by letterb %, dept, course #, section #
void sortCourses2(vector<Course> &courses){
  if(courses.size()>1){
    sort(courses.begin(), courses.end(),
         [](const Course& c1, const Course& c2)
         {
           GradeStats G1 = GetGradeDistribution(c1);
           GradeStats G2 = GetGradeDistribution(c2);
           if(G1.PercentB > G2.PercentB)
             return true;
           else if (G1.PercentB < G2.PercentB)
             return false;
           else{
             if (c1.Dept < c2.Dept)
               return true;
             else if (c1.Dept > c2.Dept)
               return false;
             else{ // same dept, look at course #:
               if (c1.Number < c2.Number)
                 return true;
               else if (c1.Number > c2.Number)
                 return false;
               else{ // same course #, look at section #:
                 if (c1.Section < c2.Section)
                   return true;
                 else 
                   return false;
               }
              }
            }
         });
   }
}

//print all courses in a department that have a DFW rate higher than the user input
void dfwDept(Dept &d, string input){
  double bound;
  int DFW, N;
  vector<Course> courses;
  if(!isdigit(input[0]))
    cout << "**invalid threshold";
  else{
    bound = stod(input);
    for(Course c : d.Courses){
      if(GetDFWRate(c, DFW, N) > bound)
        courses.push_back(c);
    }
    if(courses.empty())
      cout << "**none found" << endl;
    else{
      sortCourses(courses);
      for(Course c : courses)
        printCourse(c);
    }
  }
}

//print all courses in a college that have a DFW rate higher than the user input
void dfwCollege(College &coll, string input){
  double bound;
  int DFW, N;
  vector<Course> courses;
  if(!isdigit(input[0]))
   cout << "**invalid threshold";
  else{
   bound = stod(input);
   for(Dept d : coll.Depts){ 
       for(Course c : d.Courses){
          if(GetDFWRate(c, DFW, N) > bound)
            courses.push_back(c);
        }
    }
    if(courses.empty())
      cout << "**none found" << endl;
    else{
      sortCourses(courses);
      for(Course c : courses)
        printCourse(c);
    }
  }
}

//print all courses in a department that have a % of B grades rate higher than the user input
void letterBDept(Dept &d, string input){
  double bound;
  GradeStats G;
  vector<Course> courses;
  if(!isdigit(input[0]))
    cout << "**invalid threshold";
  else{
    bound = stod(input);
    for(Course c : d.Courses){
      G = GetGradeDistribution(c);
      if(G.PercentB > bound)
        courses.push_back(c);
    }
    if(courses.empty())
      cout << "**none found" << endl;
    else{
      sortCourses2(courses);
      for(Course c : courses)
        printCourse(c);
    }
  }
}

//print all courses in a college that have a % of B grades rate higher than the user input
void letterBCollege(College &coll, string input){
  double bound;
  GradeStats G;
  vector<Course> courses;
  if(!isdigit(input[0]))
    cout << "**invalid threshold";
  else{
    bound = stod(input);
    for(Dept d : coll.Depts){ 
      for(Course c : d.Courses){
        G = GetGradeDistribution(c);
        if(G.PercentB > bound)
          courses.push_back(c);
      }
    }
    if(courses.empty())
      cout << "**none found" << endl;
    else{
      sortCourses2(courses);
      for(Course c : courses)
        printCourse(c);
    }
  }
}

//calculate the GPA for a course
double gpaCourse(Course c){
  int total = c.NumA + c.NumB + c.NumC + c.NumD + c.NumF;
  double num = (c.NumA * 4.0) + (c.NumB * 3.0) + (c.NumC * 2.0) + (c.NumD * 1.0);
  return num/total;
}

//sort the courses by gpa, dept, course #, section #
void sortCourses3(vector<Course> &courses){
  if(courses.size()>1){
    sort(courses.begin(), courses.end(),
         [](const Course& c1, const Course& c2)
         {
           if(gpaCourse(c1) > gpaCourse(c2))
             return true;
           else if (gpaCourse(c1) < gpaCourse(c2))
             return false;
           else{
             if (c1.Dept < c2.Dept)
               return true;
             else if (c1.Dept > c2.Dept)
               return false;
             else{ // same dept, look at course #:
               if (c1.Number < c2.Number)
                 return true;
               else if (c1.Number > c2.Number)
                 return false;
               else{ // same course #, look at section #:
                 if (c1.Section < c2.Section)
                   return true;
                 else 
                   return false;
               }
              }
            }
         });
   }
}

//calulate the GPA for a department
//print GPA for each course if print = true
double gpaDept(Dept &d, bool print){
  vector<Course> courses;
  double one = 0.0;
  double sum = 0.0;
  for(Course c : d.Courses){
    if(c.getGradingType() == Course::Letter){
      one = gpaCourse(c);
      sum += one;
      courses.push_back(c);
    }
  }
  if(print){
    if(courses.empty())
      cout << "**none found" << endl;
    else{
      sortCourses3(courses);
      for(Course c : courses){
        cout << "Overall GPA for " << c.Dept << " " << c.Number 
             << "(" << c.Section << ") : " << gpaCourse(c) << endl; 
      }
    }
  }
  return (sum/courses.size());
}

//sort the departments by GPA and dept name
void sortDepts(vector<Dept> &d){
  if(d.size()>1){
    sort(d.begin(), d.end(),
         [&](Dept d1, Dept d2)
         {
           if(gpaDept(d1, false) > gpaDept(d2, false))
             return true;
           else if (gpaDept(d1, false) < gpaDept(d2, false))
             return false;
           else{
             if (d1.Name < d2.Name)
               return true;
             else 
               return false;
           }
          });
   }
}

//print GPA for each department in a college
void gpaCollege(College &c){
  vector<Dept> depts;
  for(Dept d : c.Depts){
    if(gpaDept(d, false) > 0.0)
      depts.push_back(d);
  }
  sortDepts(depts);
  for(Dept d: depts)
    cout << "Overall GPA for " << d.Name << " : " << gpaDept(d, false) << endl; 
}

//prints the summary data for a college
void sortCollege(College &college, string command){
  if(!college.Depts.empty()){
    //sort college by dept name
    std::sort(college.Depts.begin(), college.Depts.end(),
             [&](Dept a, Dept b)
              {
                if(a.Name < b.Name)
                  return true;
                else
                  return false;
              });
      if(command == "summary"){
        for(Dept d : college.Depts)
            printDept(d);
      }
      else if (command == "satisfactory"){
        if(college.Depts.empty())
          cout << "**none found" << endl;
        else{
          for(Dept d : college.Depts)
                printSatisfactory(d, false);
        }
      }
  }
}

//check if the user input is a valid department in the college
void findDept(College &college, string userIn, string command, string find) {
  auto d = std::find_if(college.Depts.begin(), college.Depts.end(),
                             [=](const Dept d)
                              {
                                if(d.Name == userIn)
                                  return true;
                                else
                                  return false;
                              });
        if(d == college.Depts.end())
          cout << "**dept not found" << endl;
        else if (command == "summary")
          printDept(*d);
        else if (command == "search")
          searchDept(*d, find);
        else if (command == "satisfactory")
          printSatisfactory(*d, true);
        else if (command == "dfw")
          dfwDept(*d, find);
        else if (command == "letterB")
          letterBDept(*d, find);
        else if (command == "average")
          gpaDept(*d, true);
}



int main()
{
    string filename;

    cout << std::fixed;
    cout << std::setprecision(2);

    //
    // 1. Input the filename and then the grade data:
    //
    cin >> filename;
    // filename = "fall-2018.csv";

    College college = InputGradeData(filename);

    string userIn, command;
    // 2. TODO: print out summary of the college
    // DEFINE your own functions
    
    cout << "** College of " << college.Name << ", " << college.Semester << " " << college.Year << " **" << endl;
    cout << "# of courses taught: " << college.NumCourses() << endl;
    cout << "# of students taught: " << college.NumStudents() << endl;
    cout << "grade distribution (A-F): "; collegeGradeDist(college);
    int DFW, N;
    cout << "DFW rate: " << GetDFWRate(college, DFW, N) << "%" << endl << endl;
  
    //
    // 3. TODO: Start executing commands from the user:
    // DEFINE your own functions
    string find;
  
    do {
      cout << "Enter a command> ";
      cin >> command;
      
      // (enclosing loop will) exit the program
      if(command == "#")
        break;
      
      // 1. summary
      else if(command == "summary"){
        cout << "dept name, or all? ";
        cin >> userIn;
        if(userIn == "all")
          sortCollege(college, command);
        else
          findDept(college, userIn, command, find);
      }

      // 2. search
      else if(command == "search"){
        cout << "dept name, or all? ";
        cin >> userIn;
                
        cout << "course # or instructor prefix? ";
        cin >> find;
        if(userIn == "all")
          searchCollege(college, find);
        else
          findDept(college, userIn, command, find);
      }
      
      // 3. satisfactory
      else if(command == "satisfactory"){
        cout << "dept name, or all? ";
        cin >> userIn;
        
        if(userIn == "all")
          sortCollege(college, command);
        else
          findDept(college, userIn, command, find);
      }
      
      // 4. dfw
      else if(command == "dfw"){
        cout << "dept name, or all? ";
        cin >> userIn;
        cout << "dfw threshold? ";
        cin >> find;
        
        if(userIn == "all")
          dfwCollege(college, find);
        else
          findDept(college, userIn, command, find);
      }
      
      // 5. letterB
      else if(command == "letterB"){
        cout << "dept name, or all? ";
        cin >> userIn;
        cout << "letter B threshold? ";
        cin >> find;
        
        if(userIn == "all")
          letterBCollege(college, find);
        else
          findDept(college, userIn, command, find);
      }
      
      // 6. average
      else if(command == "average"){
        cout << "dept name, or all? ";
        cin >> userIn;
        
        if(userIn == "all")
          gpaCollege(college);
        else
          findDept(college, userIn, command, find);
      }
            
      else
        cout << "**unknown command" << endl;
      
    } while(command != "#");
          
    cout << endl;
    //
    // done:
    //
    return 0;
}