#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#define ERRORCODE -DBL_MAX
#define EXITVALUE DBL_MAX
#define DEBUG_MODE false

// Data structure for expression
struct expr {
	double n1;
	string op;
	double n2;
	double answer;
	bool error;
};

// Data structure used for macro
struct macro {
	string name;
	string value;
};

// Data structure used for variable
struct variable {
	string name;
	double value;
};

//// *****************
//// UTILITY FUNCTIONS
//// *****************

// Check if input character is a digit. If so, return true.
bool isnum(char charin)
{
	if ((charin >= '0' && charin <= '9') || charin == '.') {
		return true;
	} else {
		return false;
	}
}

// Find the power of base and exponent
double power(double bas, int exp)
{
	if (exp == 0) { return 1; }
	if (exp%2 == 0) {
		return power(bas*bas, exp/2);
	} else {
		return bas*power(bas, exp-1);
	}
}

// Function to decode operations from a raw string
char decodeop(char * opin)
{
	// Simple operations
	if (strcmp(opin, "+") == 0) { return '+'; }
	if (strcmp(opin, "-") == 0) { return '-'; }
	if (strcmp(opin, "*") == 0) { return '*'; }
	if (strcmp(opin, "/") == 0) { return '/'; }
	// Power
	if (strcmp(opin, "^") == 0) { return '^'; }
	if (strcmp(opin, "pow") == 0 || strcmp(opin, "^^") == 0) { return '^'; }
	// Modulo
	if (strcmp(opin, "%") == 0 || strcmp(opin, "mod") == 0) { return '%'; }
	// Xor
	if (strcmp(opin, "x") == 0 || strcmp(opin, "xor") == 0) { return 'X'; }
	if (strcmp(opin, "X") == 0) { return 'X'; }
	// Default
	return '0';
}

// Get number of spaces in a string
int numspaces(char * inputchar)
{
	// Iterations, spaces
	int i=0;
	int s=0;
	while(1)
	{
		if (inputchar[i]==' ') { s++; }
		if (inputchar[i] == 0) { break; }
		i++;
	}
	return s;
}

//// ****************
//// REFINE FUNCTIONS
//// ****************

// Refine an input expression.
string refine(string expressionin)
{
	string expressionout; // Output expression
	bool inisdigit = isnum(expressionin[0]); // Whether the previous char was a digit
	for (int inp=0; inp < expressionin.length(); inp++)
	{
		// Make sure this isn't whitespace
		if (expressionin[inp] != ' ') {
			// Swapped from digit to non-digit or from non-digit to digit
			if (isnum(expressionin[inp]) != inisdigit) {
				expressionout += ' '; // Add space
				inisdigit = !inisdigit;
			}
			expressionout += expressionin[inp];
		}
	}
	return expressionout;
}

// Refine from a refined expression to formula
expr refinetoexpr(char * expressionraw, int exprlen)
{
	expr formula; int expressionspaces = 0;
	char * operation = (char*)malloc(exprlen);
	for (int i=0; i<exprlen; i++) {
		if (expressionraw[i] == ' ') {
			expressionspaces++;
		}
	}
	if (strcmp(expressionraw, "") == 0 || expressionspaces != 2) {
		formula.op = "NULL"; // Error!
	} else {
		sscanf(expressionraw, "%lf %s %lf", &formula.n1, operation, &formula.n2);
		formula.op = operation;
	}
	return formula;
}

// Function to calculate the result of an inexpr struct.
void updateresult(struct expr * inexpr)
{
	// Set local variables after inexpr numbers
	double n1 = inexpr->n1;
	double n2 = inexpr->n2;
	// First, set error to false.
	inexpr->error = false;
	// Do operation specified
	if (inexpr->op == "X") {
		inexpr->answer = (double)((int)n1^(int)n2); // Xor
	} else if (inexpr->op == "^") {
		inexpr->answer = power(n1, (int)n2); // Power
	} else if (inexpr->op == "%") {
		inexpr->answer = (double)((int)n1%(int)n2); // Modulo
	} else if (inexpr->op == "/") {
		inexpr->answer = n1/n2; // Divide
	} else if (inexpr->op == "*") {
		inexpr->answer = n1*n2; // Multiply
	} else if (inexpr->op == "-") {
		inexpr->answer = n1-n2; // Subtract
	} else if (inexpr->op == "+") {
		inexpr->answer = n1+n2; // Add
	} else {
		inexpr->error = true; // Unrecognized!
	}
}

//// ***************
//// SOLVE FUNCTIONS
//// ***************

// A function that solves a single expression by refining.
double solve_single_expression(string expressionraw)
{
	if (DEBUG_MODE == true) { cout << "Solving: " << expressionraw << ", "; }
	// Allocate formula struct
	expr formula;
	// Refine input
	formula = refinetoexpr((char*)expressionraw.c_str(), expressionraw.length());
	// Calculate result and return it
	updateresult(&formula);
	if (DEBUG_MODE == true) { cout << "got " << formula.answer << "\n"; }
	if (formula.error == true) { return ERRORCODE; }
	else { return formula.answer; }
}

string concatexpr(string a, string b, string c)
{
	string out = a;
	out += " ";
	out += b;
	out += " ";
	out += c;
	return out;
}

// Solves functions
string solveFunctions(string expressionraw, string toparse[], unsigned short parselength)
{
	// Declare and initialize everything
	string subexpression, subsolution;
	string beforesub = "", aftersub;
	stringstream expressionstream; expressionstream.str(expressionraw);
	stringstream expressiondump; expressiondump.str("");
	subexpression = "";
	bool setp; // Subexpression Equals To Parse
	while (expressionstream >> subexpression) {
		// If at least one of the strings in toparse equals subexpression, set setp to true
		setp = false;
		for (int i=0; i<parselength; i++) {
			if (subexpression == toparse[i]) {
				setp = true;
		}}
		// If setp is true, parse the expression
		if (setp) {
			expressionstream >> aftersub; // Get what is directly after. If there is nothing, this will be empy
			subsolution = to_string(solve_single_expression(concatexpr(beforesub, subexpression, aftersub))); // Solves subexpression
			// Creates the new formula, stored in expressiondump
			expressiondump << subsolution;
			aftersub = ""; getline(expressionstream, aftersub);
			expressiondump << aftersub;
			// Update expressionraw and re-initialize expressionstream to the new value. Clear other variables.
			expressionraw = expressiondump.str();
			expressionstream.clear();
			expressionstream.str(expressionraw);
			expressiondump.clear();
			expressiondump.str("");
			beforesub = ""; subexpression = "";
		} else {
			// If this is not to be solved (or is a number), concatenate it with expressiondump and store it in beforesub just in
			// case it needs to be used in the next iteration.
			expressiondump << beforesub;
			beforesub = subexpression;
		}
	}

	return refine(expressionraw); // Refine and return
}

// An iteration of the main loop.
string main_iteration(string expressionraw)
{
	// Refine the expression
	expressionraw = refine(expressionraw);

	// Debug message
	if (DEBUG_MODE == true) { cout << "Iteration: " << expressionraw << ", "; }
	// Variables
	string subexpression, subsolution; // Subexpression, Solution to subexpression
	// First, parse parentheses
	for (int xx=0; xx<expressionraw.length(); xx++) {
		if (expressionraw[xx] == '(') {
			for (int yy=expressionraw.length()-1; yy>=0; yy--) {
				if (expressionraw[yy] == ')') {
					subexpression = expressionraw.substr(xx+1, yy-xx-1);
					subsolution = main_iteration(subexpression); // Recursion! (Leave out the parentheses though)
					expressionraw.replace(xx, yy-xx+1, subsolution); // Replace subexpression with its solution
					xx=0; yy=0;
					expressionraw = refine(expressionraw); // Re-refine expressionraw
	}}}}

	if (DEBUG_MODE == true) { cout << "After parentheses: " << expressionraw << ", "; }

	// Okay, now there are no parentheses. Parse powers.
	string power_parser[] = {string("^")};
	expressionraw = solveFunctions(expressionraw, power_parser, 1);

	expressionraw = refine(expressionraw);
	if (DEBUG_MODE == true) { cout << "After powers: " << expressionraw << ", "; }

	// Parse multiplication and division and modulo
	string times_parser[] = {string("*"), string("/"), string("%")};
	expressionraw = solveFunctions(expressionraw, times_parser, 3);

	expressionraw = refine(expressionraw);
	if (DEBUG_MODE == true) { cout << "After modulos: " << expressionraw << ", "; }

	// Parse addition and subtraction
	string plus_parser[] = {string("+"), string("-")};
	expressionraw = solveFunctions(expressionraw, plus_parser, 2);
	
	// Debug message
	if (DEBUG_MODE == true) { cout << "returning " << expressionraw << "\n"; }
	// Returns fully solved expression
	return expressionraw;
}

// Wrapper function fo main_iteration, returns a double instead of a string. Also parses variable/function declarations and system calls
long double main_iteration_wrapper(string expressionraw)
{
	if (expressionraw[0] == '!') {
		return system(expressionraw.c_str()+1); // System call
	} else if (expressionraw == "quit" || expressionraw == "exit") {
		return EXITVALUE;
	} else {
		string myResult = main_iteration(expressionraw);
		long double doubleResult = stold(myResult);
		return doubleResult;
	}
}

//// **************
//// MAIN FUNCTIONS
//// **************

int main_loop()
{
	// Allocate string input and double result
	string input;
	long double result;
	// Enter main loop
	while (1)
	{
		printf("C++alculator\t> ");
		getline(cin, input);
		result = main_iteration_wrapper(input);
		if (result == ERRORCODE) {
			puts("Result      \t> ERROR");
		} else if (result == EXITVALUE) {
			puts("Result      \t> EXITTING");
			break;
		} else {
			cout << "Result      \t> " << result << "\n";
		}
	}
}

int single_main(string input)
{
	// Allocate double result
	long double result;
	// Solve and print
	result = main_iteration_wrapper(input);
	cout << result << "\n";
}

int main(int argc, char * argv[])
{
	if (argc == 1) {
		main_loop();
	} else {
		string inputExpression = "";
		for (int i=1; i<argc; i++) {
			inputExpression += argv[i];
		}
		single_main(inputExpression);
	}
}
