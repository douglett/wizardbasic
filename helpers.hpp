#pragma once
#include <string>
#include <vector>
#include <sstream>
using namespace std;  // watch it...


// ----------------------------------------
// Helpful stuff
// ----------------------------------------


// useful defines
typedef  int32_t  i32;


// Errors and Exceptions
enum WIZ_ERROR_CODE {
	WIZERR_NONE  = 0,
	WIZERR_ERROR = 1,
	WIZERR_EXPECT_TOKEN,
	WIZERR_REDIM,
	WIZERR_LET_UNDEFINED,
	WIZERR_SET_UNDEFINED,
};

string wiz_error_generate(WIZ_ERROR_CODE err, const string& info) {
	switch (err) {
	case WIZERR_EXPECT_TOKEN:  return "syntax error; expected [" + info + "]";
	case WIZERR_REDIM:         return "redefinition of '" + info + "'";
	case WIZERR_NONE:
	case WIZERR_ERROR: 
	default:                   return "unknown error " + to_string(err);
	}
}

struct WizError : std::exception {
	string msg = "WizardBasic error: unknown error";
	virtual void buildmsg() { }
	virtual const char* what() const noexcept { return msg.c_str(); }
};

struct WizParseError : WizError {
	WIZ_ERROR_CODE  error_code = WIZERR_NONE;
	string          error_text;
	int             line_no = -1;
	virtual void buildmsg() {
		msg = string("ParseError")
			+ " :: error code " + to_string(error_code) + ", on line " + to_string(line_no+1)
			+ " :: " + error_text;
	}
	WizParseError() { }
	WizParseError(WIZ_ERROR_CODE err, const string& info, int lno=-1) {
		error_code = err;
		error_text = wiz_error_generate(err, info);
		line_no    = lno;
		buildmsg();
	}
};


// generally useful methods
struct Helpers {
	static int is_alpha    (char c) { return (c>='A' && c<='Z') || (c>='a' && c<='z') || c=='_'; }
	static int is_num      (char c) { return c>='0' && c<='9'; }
	static int is_alphanum (char c) { return is_alpha(c) || is_num(c); }
	
	static vector<string> splitws(const string& str) {
		stringstream ss(str);
		string s;
		vector<string> vs;
		while (ss >> s)  vs.push_back(s);
		return vs;
	}
	static string join(const vector<string>& vs, const string& sep=" ") {
		string s;
		for (int i = 0; i < vs.size(); i++)
			s += (i > 0 ? sep : "") + vs[i];
		return s;
	}
};