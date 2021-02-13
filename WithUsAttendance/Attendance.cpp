#include "Attendance.h"
#include <iomanip>

std::string Attendance::season_ = std::string();
int Attendance::weeks_ = int();

void TeacherAttendance::SetAttendance()
{
	std::cout << "날짜 , 시작 시간, 종료 시간을 삽입해 주십시오." << std::endl;
	std::cin >> date_ >> entrance_time_ >> exit_time_;
	date_ = "2021-" + date_;
}

std::ostream& operator<<(std::ostream& os, const TeacherAttendance& attendance) {
	os << std::left <<"Week : " << std::setw(3) << attendance.weeks_ << "Date : " << std::setw(14) <<attendance.date_ << "Ent time : " << std::setw(8)<<attendance.entrance_time_ << "Exit time : "<< std::setw(8) << attendance.exit_time_ << std::endl;
	return os;
}