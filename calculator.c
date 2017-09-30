#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define EXPRLEN 128

// Data structure for expression
struct expr {
	double n1;
	char op;
	double n2;
	double answer;
	bool error;
};

// Prototypes
int main_loop();
int main_iteration(char * expressionraw);

//// *****************
//// UTILITY FUNCTIONS
//// *****************

// Check if input character is a digit. If so, return true.
bool isnum(char charin)
{
	if ((charin >= '0' && charin <= '9') || charin == '.')
	{
		return true;
	} else {
		return false;
	}
}

// Find the power of base and exponent
double power(double bas, int exp)
{
	if (exp == 0) { return 1; }
	if (exp%2 == 0)
	{
		return power(bas*bas, exp/2);
	}
	else
	{
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

// Refine a input expression.
void refine(char * expressionin, char * expressionout)
{
	int inp = 0; int out = 0; bool inisdigit = true;
	if (!isnum(expressionin[0])) { expressionout[0]=0; return; }
	while(true)
	{
		if (expressionin[inp] == 0) { expressionout[out] = 0; break; }
		if (expressionin[inp] != ' ' && expressionin[inp] != '\n')
		{
			if (isnum(expressionin[inp]))
			{
				if (inisdigit == false)
				{
					expressionout[out] = ' ';
					out++;
				}
				inisdigit=true;
			}
			else
			{
				if (inisdigit == true)
				{
					expressionout[out] = ' ';
					out++;
				}
				inisdigit=false;
			}
			expressionout[out] = expressionin[inp];
			out++;
		}
		inp++;
	}
}

// Refine from a raw expression to formula
void refinetoexpr(char * expressionraw, struct expr * formula)
{
	char * operationraw = malloc(EXPRLEN);
	int expressionspaces;
	char * expression = malloc(EXPRLEN*2);
	refine(expressionraw, expression);
	// Get structure contents from input
	expressionspaces = numspaces(expression);
	if (strcmp(expression, "") == 0 || expressionspaces != 2)
	{
		// It's an error
		formula->n1 = 0;
		formula->n2 = 0;
		formula->op = '0';
	}
	else
	{
		// Scan refined input into data structure variables.
		sscanf(expression, "%lf %s %lf", &formula->n1, operationraw, &formula->n2);
		formula->op = decodeop(operationraw);
	}
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
	switch(inexpr->op)
	{
		case 'X': inexpr->answer = (double)((int)n1^(int)n2); break;
		case '^': inexpr->answer = power(n1, (int)n2); break;
		case '%': inexpr->answer = (double)((int)n1%(int)n2); break;
		case '/': inexpr->answer = n1/n2; break;
		case '*': inexpr->answer = n1*n2; break;
		case '-': inexpr->answer = n1-n2; break;
		case '+': inexpr->answer = n1+n2; break;
		default: inexpr->answer = 0; inexpr->error = true; break;
	}
}

//// **************
//// MAIN FUNCTIONS
//// **************

// Main function. Checks arguments. If no terminal arguments, starts main loop. Otherwise, calculates once based on terminal arguments.
int main(int argc, char * argv[])
{
	if (argc == 1)
	{
		main_loop();
	}
	else
	{
		char * input = malloc(EXPRLEN);
		strcpy(input, argv[1]);
		for (int i=2; i<argc; i++)
		{
			strcat(input, argv[i]);
		}
		return main_iteration(input);
	}
}

// The main loop. This prompts the user for inputs to calculate.
int main_loop()
{
	// Allocate all variables
	char * input = malloc(EXPRLEN);
	// Enter main loop
	while(1)
	{
		// Prompt for input, get input, refine input.
		printf("C_alculator\t> ");
		fgets(input, EXPRLEN-1, stdin);
		main_iteration(input);
	}
}

// One iteration of a main loop. Takes in an expression and prints the result.
int main_iteration(char * expressionraw)
{
	// Allocate all variables
	struct expr formula;
	// Refine input.
	refinetoexpr(expressionraw, &formula);
	// Calculate result and print it.
	printf("Result  \t> ");
	updateresult(&formula);
	if (formula.error == true) { printf("Syntax error\n"); }
	else { printf("%f\n", formula.answer); }
}