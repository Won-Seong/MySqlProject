#include <iostream>
#include <string>
#include <vector>
#include <mysql.h>
#include <algorithm>
#include "MySqlInformation.h"//GitHub�� MySQL ������ �巯���� �� �����ϱ� ���Ͽ� ���� ����
#include "Attendance.h"
#include "Person.h"

#pragma comment(lib, "libmySQL.lib")

void SetSeasonAndWeeks() {
	std::string season;
	int weeks;
	std::cout << "� �����Դϱ�? : ";
	std::cin >> season;
	std::cout << "�� �����Դϱ�? : ";
	std::cin >> weeks;
	std::cout << std::endl;
	Attendance::set_season(season);
	Attendance::set_weeks(weeks);
}

bool StoreTeacherData(std::vector< Teacher >& teacher, MYSQL* connection) {
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;
	if (mysql_query(connection, "SELECT * FROM class_2021_winter;") != 0) {
		return false;
	}
	sql_result = mysql_store_result(connection);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		teacher.push_back(Teacher{ std::atoi(sql_row[0]) , sql_row[1] , sql_row[2] });
	}
	return true;
}

bool CommitOrRollback() {
	bool flag{ false };
	while (flag == false) {
		std::cout << "\n==================================================\n" << std::endl;
		std::cout << "COMMIT or ROLLBACK? : ";
		std::cin.ignore(100, '\n');
		std::string str;
		std::cin >> str;
		if (str == "COMMIT") {
			std::cout << "���� Ŀ���մϱ�? ������ y , �ƴϸ� n�� �Է��Ͽ� �ֽʽÿ�. : ";
			std::cin >> str;
			if (str == "y") {
				return true;
			}
			else if (str == "n") continue;
			else {
				std::cout << "�߸� �Է��ϼ̽��ϴ�! �ٽ� ó������ �����մϴ�." << std::endl;
			}
		}
		else {
			std::cout << "���� �ѹ��մϱ�? ������ y, �ƴϸ� n�� �Է��Ͽ� �ֽʽÿ�. : ";
			std::cin >> str;
			if (str == "y") {
				return false;
			}
			else if (str == "n") continue;
			else {
				std::cout << "�߸� �Է��ϼ̽��ϴ�! �ٽ� ó������ �����մϴ�." << std::endl;
			}
		}
	}
	return false;
}

int main() {
	//MySQL �����ͺ��̽� ����
	MYSQL* cons = NULL, conn;
	mysql_init(&conn); //MySQL ���� �ʱ�ȭ
	cons = mysql_real_connect(&conn, DB_HOST.c_str(), DB_USER.c_str(), DB_PASS.c_str(), DB_NAME.c_str(), 3306, NULL, 0);
	if (cons == NULL) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}
	mysql_set_character_set(cons, "euckr");
	//���� �� ���� ���� �������� ����

	//������ ���� ����
	SetSeasonAndWeeks();

	//������ ���� �ҷ����� �õ�
	std::vector< Teacher > teacher_array;
	if (StoreTeacherData(teacher_array, cons) == false) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}

	//Ʈ����� ����
	if (mysql_query(cons, "START TRANSACTION;") != 0) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}

	while (!std::cin.eof()) {
		std::cout << "������ �̸��� �Է��Ͽ� �ֽʽÿ�. Commit Ȥ�� Rollback�ϰ� ������ END�� �Է��Ͽ� �ֽʽÿ�. : ";

		std::cin.ignore(100, '\n');
		std::string name;
		std::cin >> name;
		if (name == "END") {
			if (CommitOrRollback() == true) {
				if (mysql_query(cons, "COMMIT;") != 0) {
					std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
					return 1;
				}
			}
			else {
				if (mysql_query(cons, "ROLLBACK;") != 0) {
					std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
					return 1;
				}
			}
			std::cout << "�Է��� �� �Ͻðڽ��ϱ�? �� �Ͻ÷��� y, ������ ������ n�� �Է��ϼ��� : ";
			std::cin.ignore(100, '\n');
			std::string str;
			std::cin >> str;
			if (str == "y") {
				if (mysql_query(cons, "START TRANSACTION;") != 0) {
					std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
					return 1;
				}
				std::cout << "\n==================================================\n" << std::endl;
				continue;
			}//y�� ��� Ʈ����� �ٽ� �����ϰ� �ݺ�
			else if (str == "n") {
				break;
			}
		}//END�� �ԷµǾ��� �� Commit Ȥ�� Rollback�ϴ� �ܰ�

		std::vector< Teacher >::iterator iter = std::find_if(teacher_array.begin(), teacher_array.end(), [name](Teacher& teacher) -> bool {return teacher.get_name() == name; });
		if (iter == teacher_array.end()) {
			std::cout << "�ش� �̸��� ���� ����� �������� �ʽ��ϴ�!" << std::endl;
			continue;
		}
		Teacher& target_teacher = iter.operator*();
		target_teacher.SetTeacherAttendance();

		std::cout << "�Ʒ� ������ �½��ϱ�? ������ y, �ƴϸ� n�� �Է��Ͽ� �ֽʽÿ�." << std::endl;
		std::cin.ignore(100, '\n');
		target_teacher.PrintTeacherAttendance();

		std::string flag;

			std::cin >> flag;

			while (!(flag == "y" || flag == "n"))
			{
				std::cout << "�߸� �Է��ϼ̽��ϴ�! �ٽ� �Է��� �ּ���! : ";
				std::cin >> flag;
			}

			if (flag == "y") {
				std::cout << "Insert..." << std::endl;
				std::string query{ "INSERT INTO attendance_2021(weeks, teacher_id, attend_date, entrance_time, exit_time) VALUES (" };
				query += std::to_string(Attendance::get_weeks()) + R"( , )";
				query += std::to_string(target_teacher.get_id()) + R"( , )";
				query += "\"" + target_teacher.get_attendance().get_date() + R"(" , )";
				query += "\"" + target_teacher.get_attendance().get_entrance_time() + R"(" , )";
				query += "\"" + target_teacher.get_attendance().get_exit_time() + "\" );";
				if (mysql_query(cons, query.c_str()) != 0) {
					std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
					mysql_query(cons, "ROLLBACK;");
					return 1;
				}
			}
			else if (flag == "n") {
				std::cout << "�Է����� �ʰ� �������� �Ѿ�ϴ�." << std::endl;
			}
		
		std::cout << std::endl;
	}

	//Ʈ����� ����

	system("pause");
	mysql_close(cons);
}


