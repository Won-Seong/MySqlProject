#include "Person.h"

bool Person::operator<(const Person& person) const
{
	return class_ < person.class_;
}

std::ostream& operator<<(std::ostream& os, const Teacher& teacher) {
	os << std::left << "ID : " << std::setw(10) << teacher.id_ << "Name : " << std::setw(10) << teacher.name_ << "Class : " << std::setw(15) << teacher.class_ << "Teacher";
	return os;
}