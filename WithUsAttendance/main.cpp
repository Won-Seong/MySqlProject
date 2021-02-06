#include <iostream>
#include <string>
#include <mysql.h>
#include "MySqlInformation.h"//GitHub에 MySQL 정보가 드러나는 걸 방지하기 위하여 따로 저장


#pragma comment(lib, "libmySQL.lib")



int main() {
	MYSQL* cons = NULL, conn;
	MYSQL_RES* sql_result;
	MYSQL_ROW sql_row;
	int query_stat;
	mysql_init(&conn);
	cons = mysql_real_connect(&conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 3306, (char*)NULL, 0);
	if (cons == NULL) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}

	mysql_set_character_set(cons, "euckr");
	std::cout << mysql_get_server_version(cons) << std::endl;
	query_stat = mysql_query(cons, "SELECT * FROM book");
	if (query_stat != 0) {
		std::cout << "Mysql connection error : " << mysql_error(&conn) << std::endl;
		return 1;
	}
	sql_result = mysql_store_result(cons);
	while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
		std::cout << sql_row[0] << '\t' << sql_row[1] << '\t' << sql_row[2] << std::endl;
	}

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