#pragma once
#include <iostream>

class Attendance : public std::exception {
public:
	Attendance(const std::string& date, bool lateness) : date_(date), lateness_(lateness) {};
	Attendance() : date_(std::string()), lateness_(0) {};
	~Attendance() {}
	static void set_season(const std::string& season) { season_ = season; }
	static void set_weeks(int weeks) { weeks_ = weeks; }
	std::string get_season() const { return season_; }
	int get_weeks() const { return weeks_; }
	std::string get_date() const { return date_; }
	bool get_lateness() const { return lateness_; }
	virtual void SetAttendance() = 0;
protected:
	static std::string season_;
	static int weeks_;
	std::string date_;
	bool lateness_;
};

class TeacherAttendance : public Attendance {
public:
	TeacherAttendance(const std::string& date, bool lateness, const std::string& entrance_time, const std::string& exit_time)
		:Attendance(date, lateness) , entrance_time_(entrance_time), exit_time_(exit_time) {};
	TeacherAttendance() : Attendance(), entrance_time_(std::string()), exit_time_(std::string()) {};
public:
	friend std::ostream& operator<<(std::ostream& os, const TeacherAttendance& attendance);
	void SetAttendance() override;
private:
	std::string entrance_time_;
	std::string exit_time_;
};
