#include <iostream>
#include "Stack.h"
#include "Queue.h"

using namespace std;

const int MAX_USER_INPUT_LENGTH = 100;
const int MAX_ARG_LENGTH = 100;
const char WHITESPACE = ' ';

typedef enum {
    PASS,
    INVALID_USER_INPUT,
    INVALID_ARGS,
    INVALID_OP,
    OUT_OF_RANGE,
    DIVISION_BY_ZERO,
} validation;


//******************************************* Function prototypes *******************************************\\

//------------------------------- Processing Input ------------------------------------//

bool compare(const char *a, const char *b);         // compare 2 char arrays
void trim(char charArr[]);                          // delete all whitespaces before and after user arguments
void promptUserInput(char userInput[]);             // take user input
bool userWantExit(char userInput[]);                // exit or not ? //read the next argument from input

//------------------------------- Validation Process ----------------------------------//
bool isValidArgs(char userInput[]);                 // check valid input arguments
bool isValidOpChar(char opCharArr[]);               // check valid input opChar
bool isValidInt(char number[]);                     // check valid input integer
bool isInRange(char arg[]);                         // check if input integer is in range
validation validate(char userInput[], char arg1CharArr[], char opCharArr[], char arg2CharArr[]);    // Validation process
void printErrorMessage(validation val);             // print error message to console


//--------------------------------- Infix to Postfix ----------------------------------//
//--------- Shunting yard algorithm ---------//
bool isOperator(char c);							// Check if the temporary token is an operator ?
bool isLeftParenthesis(char c);						// Check if the temporary token is an left parenthesis "("
bool isRightParenthesis(char c);					// Check if the temporary token is an right parenthesis ")"
bool isUnaryOperator(const char *infix);	        // Check if the temporary token is an unary operator "~" or "&"
char getOperatorAssociativity(char c);				// Get the Associativity value of an operator
int getOperatorPrecedence(char c);					// Get the Precedence value of an operator
void clearWhiteSpaces(const char *infix, int *currentIndex, int prevIndex); // Clear the whitespaces after the recently read token
void readNextInteger(char target[], const char *userInput, int* currentIndex); //Read the next integer token

//Process handling if the temporary token being considered is an left parenthesis "("
void leftParenthesisTokenHandling(const char *infix, Stack *operator_stack);

//Process handling if the temporary token being considered is an right parenthesis ")"
void rightParenthesisTokenHandling(Queue *postfix_queue, Stack *operator_stack);

//Process handling if the temporary token being considered is and operator
void operatorTokenHandling(const char *infix, Queue *postfix_queue, Stack *operator_stack);

//Process handling if the temporary token being considered is an integer
void integerTokenHandling(char *infix, Queue *postfix_queue, int *currentIndex);

//Process handling if the temporary token being considered is an unary operator
void unaryOperatorTokenHandling(const char *infix, Stack *operator_stack);

//Process converting the infix expression given by the user to the postfix expression
void convertInfixToPostfix(char *infix, Queue *postfix_queue);


//----------Calculation-----------//
int add(int arg1, int arg2);                        // addition
int subtract(int arg1, int arg2);                   // subtraction
int multiply(int arg1, int arg2);                   // multiplication
int divide(int arg1, int arg2);                     // division
int remainder(int arg1, int arg2);                  // Taking remainder
void printResult(int result);                       //Print result to console
bool isDividingByZero(int arg);                     //Check division by 0 error
void parseUserInput(int* arg1, char* op, int* arg2, char arg1CharArr[], char opCharArr[], char arg2CharArr[]);
int operateCalculation(int arg1, char op, int arg2);
void convertIntToCharArr(int num, char target[], bool isNegative);
char getDigitFromChar(unsigned int digit);
void evaluatePostfixExpression(Queue *postfix_queue);

//----------Exit------------//
void printExitMessage();

int main() {
    while (true)
    {
        char userInput[MAX_USER_INPUT_LENGTH] = {};
        validation validationResult;

        promptUserInput(userInput);     //Taking input
        if (userWantExit(userInput)) {  //Exit?
            break;
        }

        Queue postfix_queue;
        convertInfixToPostfix(userInput, &postfix_queue);

        //-------- Test print postfix ---------//
//        while (!postfix_queue.isEmpty()) {
//            cout << postfix_queue.peek() << " ";
//            postfix_queue.dequeue();
//        }
//        cout << endl;

        evaluatePostfixExpression(&postfix_queue);
//        if (validationResult != PASS) {
//            printErrorMessage(validationResult);
//            continue;
//        }


    }
    printExitMessage();
    return 0;
}

//---------------------------------- Processing Input --------------------------------//

bool compare(const char *a, const char *b) {
    for (int i = 0; a[i] != '\0'; i++) {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

void trim(char charArr[]) {
    int start = 0; // number of leading spaces
    char* buffer = charArr;

    //find index of the first non-whitespace char
    while (*charArr && *charArr++ == ' ') ++start;

    // move to end of string
    while (*charArr++);
    int end = charArr - buffer - 1;
    while (end > 0 && buffer[end - 1] == ' ') --end; // backup over trailing spaces
    buffer[end] = 0; // remove trailing spaces
    if (end <= start || start == 0) return; // exit if no leading spaces or string is now empty
    charArr = buffer + start;
    while ((*buffer++ = *charArr++));  // remove leading spaces: K&R

}

void promptUserInput(char userInput[]) {
    printf("Enter your equation: ");
    cin.getline(userInput, MAX_USER_INPUT_LENGTH);
    trim(userInput);
}

bool userWantExit(char userInput[]) {
    char Exit[] = "Exit";
    return compare(userInput, Exit);
}

//---------------------------------- End Processing Input --------------------------------//


//---------------------------------- Validation --------------------------------//
bool isValidArgs(char userInput[]) {
    int whitespaceCount = 0;

    while (*userInput) {
        if (*userInput == WHITESPACE) whitespaceCount++;
        userInput++;
    }

    return (whitespaceCount == 2);
}

bool isValidOpChar(char opCharArr[]) {
    int size = 0;
    int index = 0;
    while ( *(opCharArr + index) ) { size++; index++; }
    if (size > 1) return false;
    else {
        if (opCharArr[0] == '+' ||
            opCharArr[0] == '-' ||
            opCharArr[0] == '*' ||
            opCharArr[0] == '/' ||
            opCharArr[0] == '%')
            return true;
    }
    return false;
}

bool isValidDecimalPoints(char decimalPoints[], int index) {
    while ( *(decimalPoints + index) )
    {
        if (*(decimalPoints + index) != '0') return false;
        index++;
    }

    return true;
}

bool isValidInt(char number[]) {
    if (!isdigit(number[0]) && !isdigit(number[1]) ) {
        if (number[0] != '+' && number[0] != '-') return false;
    }

    int index = 1;
    while ( *(number + index) )
    {
        if (isdigit(*(number + index))) { index++; continue;  }
        else {
            if (*(number + index) == '.') return isValidDecimalPoints(number, index+1);
            else return false;
        }
    }
    return true;
}

bool isInRange(char arg[]) {
    int argInt;
    try {
        argInt = atoi(arg);
    }
    catch (out_of_range e) {
        //unable to convert to int due to
        //the arg (as type string) being
        //too big to store as int
        return false;
    }
    return (argInt >= -32768 && argInt <= 32767);
}

validation validate(char userInput[], char arg1CharArr[], char opCharArr[], char arg2CharArr[]) {
    if (!isValidArgs(userInput)) {
        return INVALID_ARGS;
    }
    if (!isValidOpChar(opCharArr)) {
        return INVALID_OP;
    }
    if (!isValidInt(arg1CharArr) || !isValidInt(arg2CharArr)) {
        return INVALID_USER_INPUT;
    }
    if (!isInRange(arg1CharArr) || !isInRange(arg2CharArr)) {
        return OUT_OF_RANGE;
    }
    return PASS;
}

void printErrorMessage(validation val) {
    switch (val) {
        case INVALID_ARGS: {
            printf("%s.\n%s\n", "You entered an invalid expression",
                   "The expression should looks like: <operand1>[whitespace]<operator>[whitespace]<operand2>");
            break;
        }
        case INVALID_OP: {
            printf("You entered invalid operator!\n");
            break;
        }
        case OUT_OF_RANGE: {
            printf("Your operand(s) is out of range [-32,768 to 32,767]!\n");
            break;
        }
        case DIVISION_BY_ZERO: {
            printf("You can't divide a number by 0!\n");
            break;
        }
        case INVALID_USER_INPUT: {
            printf("You entered the wrong format for the integer operands!\n");
            break;
        }
        default: {
            break;
        }
    }
}

//---------------------------------- End Validation --------------------------------//

//---------------------------------- Infix to Postfix --------------------------------//
void convertInfixToPostfix(char *infix, Queue *postfix_queue){
    Stack operator_stack;
    int currentIndex = 0, prevIndex = 0;

    while (*(infix)) {
        //Remember the previous pointer of infix
        prevIndex = currentIndex;

        //If the token being read is left parenthesis "("
        if (isLeftParenthesis(*(infix))) {
            leftParenthesisTokenHandling(infix, &operator_stack);
            clearWhiteSpaces(infix, &currentIndex, prevIndex);
        }

            // If the token being read is right parenthesis ")"
        else if (isRightParenthesis(*(infix))) {
            rightParenthesisTokenHandling(postfix_queue, &operator_stack);
            clearWhiteSpaces(infix, &currentIndex, prevIndex);
        }

            // If the token being read is an operator
        else if ( isOperator(*(infix)) ) {
            if (isUnaryOperator(infix)) unaryOperatorTokenHandling(infix, &operator_stack);
            else operatorTokenHandling(infix, postfix_queue, &operator_stack);
            clearWhiteSpaces(infix, &currentIndex, prevIndex);
        }

            //If the token being read is an integer
        else {
            integerTokenHandling(infix, postfix_queue, &currentIndex);
            clearWhiteSpaces(infix, &currentIndex, prevIndex);
        }

        //Move the 'infix' pointer from prevIndex to currentIndex position
        while (prevIndex != currentIndex) {
            infix++; prevIndex++;
        }
    }

    //While there is still operator in the operator stack, pop all of them to the output Postfix queue
    while (!operator_stack.isEmpty()) {
        //Enqueue the top operator
        postfix_queue->enqueue( operator_stack.peek() );
        operator_stack.pop(); // Pop the top operator
    }
}

bool isOperator(char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^' || c == '~' || c == '&'; }
bool isLeftParenthesis(char c) { return c == '('; }
bool isRightParenthesis(char c) { return c == ')'; }

bool isUnaryOperator(const char *infix) {
    if (*infix == '-' || *infix == '+') {
        infix++;
        if (isdigit(*(infix))) return true;
        else {
            //while (*infix == WHITESPACES) infix++;
            if (isLeftParenthesis(*infix)) return true;
        }
    }

    return false;
}

void clearWhiteSpaces(const char* infix, int *currentIndex, int prevIndex) {
    int moves = *currentIndex - prevIndex + 1;
    *currentIndex = *currentIndex + 1;
    while (*(infix + moves) == WHITESPACE) { moves++; *currentIndex = *currentIndex + 1; }
}

char getOperatorAssociativity(char c) {
    if (c == '^' || c == '~' || c == '&') return 'r';
    else return 'l';
}

int getOperatorPrecedence(char c) {
    if (c == '(') return 0;
    else if (c == '+' || c == '-') return 1;
    else if (c == '*' || c == '/' || c == '%') return 2;
    else if (c == '^') return 3;
    else return 4;
}

void readNextInteger(char target[], const char *userInput, int* currentIndex) {
    int moves = 0;
    //Find the next index of whitespace (nextIndex) from the current index
    while (*(userInput + moves) && *(userInput + moves) != WHITESPACE && isdigit(*(userInput + moves))) moves++;
    //Get the word from currentIndex to nextIndex
    int j = *currentIndex;
    int i = 0;
    while (j != *currentIndex + moves)
    {
        *(target + i) = userInput[j - *currentIndex];
        i++; j++;
    }
    *currentIndex = *currentIndex + moves - 1;
}

void leftParenthesisTokenHandling(const char *infix, Stack *operator_stack) {
    char tempToken[] = {*infix};
    operator_stack->push(tempToken);      //push to the operator stack
}

void rightParenthesisTokenHandling(Queue *postfix_queue, Stack *operator_stack) {
    while (operator_stack->peek()[0] != '(') { // While the first element of the stack being popped is not "("
        postfix_queue->enqueue( operator_stack->peek() ); // enqueue the operator
        operator_stack->pop(); //pop the operator
    }
    operator_stack->pop(); //Pop out the "(" and discard it
}

void operatorTokenHandling(const char *infix, Queue *postfix_queue, Stack *operator_stack) {
    while ((!operator_stack->isEmpty()) // While there are operators in the operator stack
           && ((getOperatorPrecedence(operator_stack->peek()[0]) > getOperatorPrecedence(*infix)) // Or there is an operator at the top of the stack with greater precedence
               // Or there is an operator at the top of the stack with equal precedence and 'left' associativity
               || (getOperatorPrecedence(operator_stack->peek()[0]) == getOperatorPrecedence(*infix) && getOperatorAssociativity(*infix) == 'l')
           )) {
        //Enqueue the top operator
        postfix_queue->enqueue( operator_stack->peek() );
        operator_stack->pop(); // Pop the top operator

    }

//    char tempToken[1] = {infix[0]};                           //Cac cac cac cac cac cac cac cac cac cac cac cac c++
    operator_stack->push(*infix); // Push the temp operator token to the stack

}

void integerTokenHandling(char *infix, Queue *postfix_queue, int *currentIndex) {
    char tempInt[MAX_ARG_LENGTH] = {};
    readNextInteger(tempInt, infix, currentIndex); //Read the next integer token
    postfix_queue->enqueue(tempInt);

}

void unaryOperatorTokenHandling(const char *infix, Stack *operator_stack) {
    if (*infix == '-') {
        char tempToken[] = {"~"};
        operator_stack->push(tempToken);
    }
    else {
        char tempToken[] = {"&"};
        operator_stack->push(tempToken);
    }
}

//-------------------------------- End Infix to Postfix ------------------------------//



//-------------------------------- Calculation ------------------------------//

int add(int arg1, int arg2) {
    return arg1 + arg2;
}

int subtract(int arg1, int arg2) {
    return arg1 - arg2;
}

int multiply(int arg1, int arg2) {
    return arg1 * arg2;
}

int divide(int arg1, int arg2) {
    return arg1 / arg2;
}

int remainder(int arg1, int arg2) {
    return arg1 % arg2;
}

void printResult(int result) {
    printf("Result of the equation is: %d\n", result);
}

bool isDividingByZero(int arg) {
    return (arg == 0);
}

void parseUserInput(int* arg1, char* op, int* arg2, char arg1CharArr[], char opCharArr[], char arg2CharArr[]) {
    *arg1 = atoi(arg1CharArr);
    *op = opCharArr[0];
    *arg2 = atoi(arg2CharArr);
}

int operateCalculation(int arg1, char op, int arg2) {
    int result = 0;
    switch (op) {
        case '+': {
            return  add(arg1, arg2);
        }
        case '-': {
            return subtract(arg1, arg2);
        }
        case '/': {
            if (isDividingByZero(arg2)) {
                printErrorMessage(DIVISION_BY_ZERO);
                return 0;
            }
            return divide(arg1, arg2);
        }
        case '*': {
            return multiply(arg1, arg2);
        }
        case '%': {
            return remainder(arg1, arg2);
        }
        default: {
            return 0;
        }
    }
}

//char getDigitFromChar(unsigned int digit){
//    switch (digit){
//        case 0:
//            return '0';
//        case 1:
//            return '1';
//        case 2:
//            return '2';
//        case 3:
//            return '3';
//        case 4:
//            return '4';
//        case 5:
//            return '5';
//        case 6:
//            return '6';
//        case 7:
//            return '7';
//        case 8:
//            return '8';
//        case 9:
//            return '9';
//        default: return ' ';
//    }
//
//}

//void convertIntToCharArr(int num, char target[], bool isNegative){
//    int index = 0;
//    if (isNegative){
//        target[0] = '-'; index++;
//        while (num) {
//            char tempDigit = getDigitFromChar(num % 10 );
//            target[index] = tempDigit; index++;
//            num /= 10;
//        }
//    } else {
//        while (num) {
//            char tempDigit = getDigitFromChar(num % 10 );
//            target[index] = tempDigit; index++;
//            num /= 10;
//        }
//    }
//}



void evaluatePostfixExpression(Queue *postfix_queue) {
    Stack cal_stack;

    while (!postfix_queue->isEmpty())
    {
        char tempToken = postfix_queue->peek()[0];
        if ( isOperator(tempToken) )
        {
            if (tempToken == '~') {
                int tempInt = cal_stack.peek(true); cal_stack.pop();
                tempInt = -tempInt;
                cal_stack.push(tempInt);
            }
            else if (tempToken == '&') {
                continue;
            }
            else{
                int arg2 = cal_stack.peek(true); cal_stack.pop();
                int arg1 = cal_stack.peek(true); cal_stack.pop();
                int result = operateCalculation(arg1, tempToken, arg2);
                cal_stack.push(result);

            }
            postfix_queue->dequeue();
        }
        else {
            cal_stack.push(atoi(postfix_queue->peek()));
            postfix_queue->dequeue();
        }
    }
    printResult(cal_stack.peek(true));
}

//-------------------------------- End calculation ------------------------------//
//---------Exit---------//
void printExitMessage() {
    const char* exitMessage =
            "LABORATORY GROUP 1\n"
            "s3748575,s3748575@rmit.edu.vn,ThanhLong,Bui\n"
            "s3742774,s3742774@rmit.edu.vn,QuangTrung,Ngo\n"
            "s3757281,s3757281@rmit.edu.vn,MinhQuang,Tran\n";
    printf("%s", exitMessage);
}

