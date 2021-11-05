
/*
	calculator08buggy.cpp

	Helpful comments removed.

	We have inserted 3 bugs that the compiler will catch and 3 that it won't.
*/

#include "std_lib_facilities.h"
//class to hold name-value pair for every character in the calculator 
struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string n): kind(ch), name(n){}
};
//handles retrieving items from input
class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer = t; full = true; }

	void ignore(char);
};


const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
 const char squareR = 's';
const char findPow = 'p';

 const string declKey = "let";
const string quitKey = "exit";
const string sqrtKey = "sqrt";
const string powKey = "pow";
//handles categorising every character
Token Token_stream::get()
{
	if (full) { full = false; return buffer; } //check if we already have a token ready
	char ch;
	cin >> ch;//skips whitespace
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
		return Token(ch); //let each char represent itself
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	cin.unget();//put digit back into the input stream
	double val;
	cin >> val;// read a number
	return Token(number, val);// return number with a value
	}
	case '#':
                  return Token(let);
	default:
		if (isalpha(ch)) {//do this if ch is a letter
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) s = ch;//while there are still chars in cin, read them into a string
			cin.unget();
			//if string is equal to other commands defined below, return them
			if (s == declKey) return Token(let);
    			if (s == quitKey) return Token(quit);
			if (s == sqrtKey) return Token(squareR);
			if (s == powKey) return Token(findPow);
			return Token(name, s);
		}
		error("Bad token");//if the char does not equal with any command return an error
	}
}
//discard characters up to and including a c
//c represents the kind of token
void Token_stream::ignore(char c)
{
//first look in the buffer
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;
//now search input
	char ch;
	while (cin >> ch)
		if (ch == c) return;
}

struct Variable {
	string name;
	double value;
	Variable(string n, double v) :name(n), value(v) { }
};

vector<Variable> names;
//return the value of the s variable
double get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ", s);
}
//set the s variable to d;
void set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}
//is variable already declared?
bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}
 //add (var,val) to variable vector
double define_name(string var, double val)
{
 if (is_declared(var))
  error(var, " declared twice");

 names.push_back(Variable(var, val));

 return val;
}



Token_stream ts;

double expression();//forward declaration
//read in values that are accepted for first time use
double primary()
{
	Token t = ts.get(); //get character from stream
	//solve "(expression)"
	switch (t.kind) {
	case '(':
	{	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("')' expected");
	return d;
	}
	//solve "primary"
	case '-':
		return -primary();
	//solve number
	case number:
		return t.value;
	//solve "let name = value"
	case name:
		return get_value(t.name);
	  //solve "sqrt(expression)"
 	case squareR: 
	{
  	//get next char after 'sqrt' if not '(' then error  
  		t = ts.get();
  		if (t.kind != '(')
   		error("'(' expected");

  		//if expression is less than 0 print an error
  		double d = expression();
  		if (d < 0)
   		error("Cannot square root negative integers");

  		//get next char after expression, if not ')' then error
  		t = ts.get();
  		if (t.kind != ')')
   		error("')' expected");

  		//return square root of the expression taken from the tokenstream
  		return sqrt(d);

 	}
	//solve "pow(expression, expression)"
  	case findPow:
  	{
   		//get next char after 'pow' if not '(' then error   
   		t = ts.get();
   		if (t.kind != '(')
    		error("'(' expected");

   		//get the expression after '('
   		double d = expression();

   		//get next char after 'expression' if not ',' then error   
   		t = ts.get();
   		if (t.kind != ',')
    		error("',' expected");

   		//get the expression after ','
   		double i = expression();

   		//get next char after expression, if not ')' then error
   		t = ts.get();
   		if (t.kind != ')')
    		error("')' expected");

   		// return expression using pow() from <cmath>
   		return pow(d, i);

  }

	default:
		error("primary expected");
	}
}
//read in values that would normally come after a primary
double term()
{
	double left = primary();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}
		case '%':
   		{
    			double d = primary();
    			if (d == 0)
     				error("%:divide by zero");
   			 left = fmod(left, d);
    			break;
   		}
		default:
			ts.unget(t);
			return left;
		}
	}
}
//can be used before a primary
double expression()
{
	double left = term();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}
//check for name definition errors
double declaration()
{
	Token t = ts.get();
	if (t.kind != 'a') error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push_back(Variable(name, d));
	return d;
}

double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration();
	default:
		ts.unget(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while (true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get(); //first discard all "prints"
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

try {
	define_name("pi", 3.1415926535);
 	define_name("e", 2.7182818284);
 	define_name("k", 1000);
	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}
