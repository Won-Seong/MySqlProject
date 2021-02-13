#include <iostream>
#include <string>
#include <vector>
#include <mysql.h>
#include <algorithm>
#include "MySqlInformation.h"//GitHub에 MySQL 정보가 드러나는 걸 방지하기 위하여 따로 저장
#include "Attendance.h"
#include "Person.h"

#pragma comment(lib, "libmySQL.lib")

void SetSeasonAndWeeks() {
	std::string season;
	int weeks;
	std::cout << "어떤 계절입니까? : ";
	std::cin >> season;
	std::cout << "몇 주차입니까? : ";
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
			std::cout << "정말 커밋합니까? 맞으면 y , 아니면 n을 입력하여 주십시오. : ";
			std::cin >> str;
			if (str == "y") {
				return true;
			}
			else if (str == "n") continue;
			else {
				std::cout << "잘못 입력하셨습니다! 다시 처음부터 진행합니다." << std::endl;
			}
		}
		else {
			std::cout << "정말 롤백합니까? 맞으면 y, 아니면 n을 입력하여 주십시오. : ";
			std::cin >> str;
			if (str == "y") {
				return false;
			}
			else if (str == "n") continue;
			else {
				std::cout << "잘못 입력하셨습니다! 다시 처음부터 진행합니다." << std::endl;
			}
		}
	}
	return false;
}

int main() {
	//MySQL 데이터베이스 연결
	MYSQL* cons = NULL, conn;
	mysql_init(&conn); //MySQL 연결 초기화
	cons = mysql_real_connect(&conn, DB_HOST.c_str(), DB_USER.c_str(), DB_PASS.c_str(), DB_NAME.c_str(), 3306, NULL, 0);
	if (cons == NULL) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}
	mysql_set_character_set(cons, "euckr");
	//연결 후 문자 집합 설정까지 성공

	//주차와 계절 결정
	SetSeasonAndWeeks();

	//교사의 정보 불러오기 시도
	std::vector< Teacher > teacher_array;
	if (StoreTeacherData(teacher_array, cons) == false) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}

	//트랜잭션 시작
	if (mysql_query(cons, "START TRANSACTION;") != 0) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}

	while (!std::cin.eof()) {
		std::cout << "교사의 이름을 입력하여 주십시오. Commit 혹은 Rollback하고 싶으면 END를 입력하여 주십시오. : ";

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
			std::cout << "입력을 더 하시겠습니까? 더 하시려면 y, 끝내고 싶으면 n을 입력하세요 : ";
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
			}//y일 경우 트랜잭션 다시 시작하고 반복
			else if (str == "n") {
				break;
			}
		}//END가 입력되었을 시 Commit 혹은 Rollback하는 단계

		std::vector< Teacher >::iterator iter = std::find_if(teacher_array.begin(), teacher_array.end(), [name](Teacher& teacher) -> bool {return teacher.get_name() == name; });
		if (iter == teacher_array.end()) {
			std::cout << "해당 이름을 가진 교사는 존재하지 않습니다!" << std::endl;
			continue;
		}
		Teacher& target_teacher = iter.operator*();
		target_teacher.SetTeacherAttendance();

		std::cout << "아래 사항이 맞습니까? 맞으면 y, 아니면 n을 입력하여 주십시오." << std::endl;
		std::cin.ignore(100, '\n');
		target_teacher.PrintTeacherAttendance();

		std::string flag;

			std::cin >> flag;

			while (!(flag == "y" || flag == "n"))
			{
				std::cout << "잘못 입력하셨습니다! 다시 입력해 주세요! : ";
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
				std::cout << "입력하지 않고 다음으로 넘어갑니다." << std::endl;
			}
		
		std::cout << std::endl;
	}

	//트랜잭션 종료

	system("pause");
	mysql_close(cons);
}


