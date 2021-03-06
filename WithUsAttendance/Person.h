#pragma once
#include <iostream>
#include <iomanip>
#include "Attendance.h"

enum class CLASS {
	FIRST_WOORI,
	FIRST_MOODO,
	FIRST_HAMGGE,
	SECOND_WOORI,
	SECOND_HAMGGE,
	THIRD_WOORI,
	THIRD_MOODO,
	THIRD_HAMGGE,
	ENGLISH
};//존재하는 반

class Person : public std::exception {
public:
	Person(const int& id, const std::string name , const std::string _class) : id_(id), name_(name) , class_(_class) {}
	~Person() {};
	const int& get_id() const { return id_; }
	const std::string& get_name() const { return name_; }
	const std::string& get_class() const { return class_; }
	bool operator<(const Person& person) const;
protected:
	int id_;
	std::string name_;
	std::string class_;
};

class Teacher : public Person {
public:
	Teacher(const int& id, const std::string name, const std::string _class) : Person(id, name,_class) , attendance_() {}
	~Teacher() {};
public:
	friend std::ostream& operator<<(std::ostream& os, const Teacher& teacher);
	void SetTeacherAttendance() {
		std::cout << name_ << " 교사의 ";
		attendance_.SetAttendance();
	};
	void PrintTeacherAttendance() const { std::cout << name_ << " 교사의 출석 ->  " << attendance_; }
	const TeacherAttendance& get_attendance() const { return attendance_; }
private:
	TeacherAttendance attendance_;
};

