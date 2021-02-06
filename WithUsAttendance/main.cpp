#include <iostream>
#include <string>
#include <vector>
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
	int query_stat;
	mysql_init(&conn);
	cons = mysql_real_connect(&conn, DB_HOST.c_str(), DB_USER.c_str(), DB_PASS.c_str(), DB_NAME.c_str(), 3306, NULL, 0);
	if (cons == NULL) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}
	mysql_set_character_set(cons, "euckr");

	std::vector<Teacher> teachers;
	teachers.reserve(50);

	std::string season;
	int weeks;
	std::cout << "어떤 계절입니까? : ";
	std::cin >> season;
	std::cout << "몇 주차입니까? : ";
	std::cin >> weeks;
	std::cout << std::endl;
	Attendance::set_season(season);
	Attendance::set_weeks(weeks);


	query_stat = mysql_query(cons, "SELECT * FROM class_2021_winter");
	if (query_stat != 0) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}
	sql_result = mysql_store_result(cons);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		teachers.push_back(Teacher(std::atoi(sql_row[0]) , sql_row[1] , sql_row[2]));
	}

	std::sort(teachers.begin(), teachers.end());

	teachers.front().SetTeacherAttendance();
	teachers.front().PrintTeacherAttendance();

	/*std::cout << "\nInsert Value" << std::endl;
	std::string id, bookshop, name, price;
	std::cin >> id >> bookshop >> name >> price;

	std::string query{ "INSERT INTO book VALUES " };
	query += "(\'";
	query += id;
	query += R"(' , ')";
	query += bookshop;
	query += R"(' , ')";
	query += name;
	query += R"(' , ')";
	query += price;
	query += R"(' );)";
	query_stat = mysql_query(cons, query.c_str());

	if (query_stat != 0) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}*/

	mysql_close(cons);

}