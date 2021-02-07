#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <mysql.h>
#include <algorithm>
#include "MySqlInformation.h"//GitHub�� MySQL ������ �巯���� �� �����ϱ� ���Ͽ� ���� ����
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

	//������ ���� ����
	std::string season;
	int weeks;
	std::cout << "� �����Դϱ�? : ";
	std::cin >> season;
	std::cout << "�� �����Դϱ�? : ";
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
	}//Ʈ����� ����

	while (!std::cin.eof()) {
		std::cout << "������ �̸��� �Է��Ͽ� �ֽʽÿ�. �׸��ϰ� ������ END�� �Է��Ͽ� �ֽʽÿ�. : ";

		std::cin.ignore(100, '\n');
		std::string name;
		std::cin >> name;
		if (name == "END") break;

		Teacher& target_teacher = std::find_if(teachers.begin(), teachers.end(), [name](Teacher& teacher) -> bool {return teacher.get_name() == name; }).operator*();
		target_teacher.SetTeacherAttendance();

		std::cout << "�Ʒ� ������ �½��ϱ�? ������ 1, �ƴϸ� 0�� �Է��Ͽ� �ֽʽÿ�." << std::endl;
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
			std::cout << "�Է����� �ʽ��ϴ�." << std::endl;
		}
	}

	bool flag{ false };
	while (flag == false) {
		std::cout << "COMMIT or ROLLBACK? : ";
		std::cin.ignore(100, '\n');
		std::string str;
		std::cin >> str;
		if (str == "COMMIT") {
			std::cout << "���� Ŀ���մϱ�? ������ 1 , �ƴϸ� 0�� �Է��Ͽ� �ֽʽÿ�. : ";
			std::cin >> flag;
			if (flag) {
				if (mysql_query(cons, "COMMIT;") != 0) {
					std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
					return 1;
				}//Ʈ����� ����
			}
		}
		else {
			std::cout << "���� �ѹ��մϱ�? ������ 1, �ƴϸ� 0�� �Է��Ͽ� �ֽʽÿ�. : ";
			std::cin >> flag;
			if (flag) {
				if (mysql_query(cons, "ROLLBACK;") != 0) {
					std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
					return 1;
				}//Ʈ����� ����
			}
		}
	}

	system("pause");
	mysql_close(cons);
}


