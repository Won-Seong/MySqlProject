#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <mysql.h>
#include <algorithm>
#include "MySqlInformation.h"//GitHub에 MySQL 정보가 드러나는 걸 방지하기 위하여 따로 저장
#include "Attendance.h"
#include "Person.h"

#pragma comment(lib, "libmySQL.lib")

int main() {
	MYSQL* cons = NULL, conn;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;
	mysql_init(&conn);
	cons = mysql_real_connect(&conn, DB_HOST.c_str(), DB_USER.c_str(), DB_PASS.c_str(), DB_NAME.c_str(), 3306, NULL, 0);
	if (cons == NULL) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}
	mysql_set_character_set(cons, "euckr");

	std::vector< Teacher > teachers;

	//주차와 계절 결정
	std::string season;
	int weeks;
	std::cout << "어떤 계절입니까? : ";
	std::cin >> season;
	std::cout << "몇 주차입니까? : ";
	std::cin >> weeks;
	std::cout << std::endl;
	Attendance::set_season(season);
	Attendance::set_weeks(weeks);

	if (mysql_query(cons, "SELECT * FROM class_2021_winter;") != 0) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}
	sql_result = mysql_store_result(cons);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		teachers.push_back(Teacher{ std::atoi(sql_row[0]) , sql_row[1] , sql_row[2] });
	}

	if (mysql_query(cons, "START TRANSACTION;") != 0) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}//트랜잭션 시작

	while (!std::cin.eof()) {
		std::cout << "교사의 이름을 입력하여 주십시오. 그만하고 싶으면 END를 입력하여 주십시오. : ";

		std::cin.ignore(100, '\n');
		std::string name;
		std::cin >> name;
		if (name == "END") break;

		Teacher& target_teacher = std::find_if(teachers.begin(), teachers.end(), [name](Teacher& teacher) -> bool {return teacher.get_name() == name; }).operator*();
		target_teacher.SetTeacherAttendance();

		std::cout << "아래 사항이 맞습니까? 맞으면 1, 아니면 0을 입력하여 주십시오." << std::endl;
		std::cin.ignore(100, '\n');

		target_teacher.PrintTeacherAttendance();
		bool flag;
		std::cin >> flag;
		if (flag) {
			std::cout << "Insert..." << std::endl;
			std::string query{ "INSERT INTO attendance_2021(weeks, teacher_id, attend_date, entrance_time, exit_time, lateness) VALUES (" };
			query += std::to_string(Attendance::get_weeks()) + R"( , )";
			query += std::to_string(target_teacher.get_id()) + R"( , )";
			query += "\"" + target_teacher.get_attendance().get_date() + R"(" , )";
			query += "\"" + target_teacher.get_attendance().get_entrance_time() + R"(" , )";
			query += "\"" + target_teacher.get_attendance().get_exit_time() + R"(" , )";
			query += std::to_string(target_teacher.get_attendance().get_lateness()) + " );";
			if (mysql_query(cons, query.c_str()) != 0) {
				std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
				mysql_query(cons, "ROLLBACK;");
				return 1;
			}
		}
		else {
			std::cout << "입력하지 않습니다." << std::endl;
		}
	}

	bool flag{ false };
	while (flag == false) {
		std::cout << "COMMIT or ROLLBACK? : ";
		std::cin.ignore(100, '\n');
		std::string str;
		std::cin >> str;
		if (str == "COMMIT") {
			std::cout << "정말 커밋합니까? 맞으면 1 , 아니면 0을 입력하여 주십시오. : ";
			std::cin >> flag;
			if (flag) {
				if (mysql_query(cons, "COMMIT;") != 0) {
					std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
					return 1;
				}//트랜잭션 종료
			}
		}
		else {
			std::cout << "정말 롤백합니까? 맞으면 1, 아니면 0을 입력하여 주십시오. : ";
			std::cin >> flag;
			if (flag) {
				if (mysql_query(cons, "ROLLBACK;") != 0) {
					std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
					return 1;
				}//트랜잭션 종료
			}
		}
	}

	system("pause");
	mysql_close(cons);
}


