#include <iostream>
#include "Stack.h"
#include "Queue.h"

using namespace std;

const int MAX_USER_INPUT_LENGTH = 100;
const int MAX_ARG_LENGTH = 100;
const char WHITESPACE = ' ';

typedef enum {
    PASS,
    INVALID_INTEGER_FORMAT,
    INVALID_ARGS,
    INVALID_OP,
    OUT_OF_RANGE,
    DIVISION_BY_ZERO,
    OPERATORS_OPERANDS_NUMBER_MISMATCH,
    WRONG_WHITESPACES_NO,
    INVALID_TOKEN,
    NUMBER_OF_LEFT_RIGHT_PAREN_MISMATCH,
    EMPTY_CONTENT_WITHIN_A_PAREN_PAIR
} validation;


//******************************************* Function prototypes *******************************************\\

//------------------------------- Processing Input ------------------------------------//

bool compare(const char *a, const char *b);         // compare 2 char arrays
void trim(char charArr[]);                          // delete all whitespaces before and after user arguments
void promptUserInput(char userInput[]);             // take user input
bool userWantExit(char userInput[]);                // exit or not ? //read the next argument from input

//------------------------------- Validation Process ----------------------------------//
bool isOperator(char c);							// Check if the temporary token is an operator ?
bool isLeftParenthesis(char c);						// Check if the temporary token is an left parenthesis "("
bool isRightParenthesis(char c);					// Check if the temporary token is an right parenthesis ")"
bool isUnaryOperator(const char *infix);	        // Check if the temporary token is an unary operator "~" or "&"
bool isValidInt(char number[]);                     // check valid input integer
bool isInRange(char arg[]);                         // check if input integer is in range
validation validate(const char *userInput);              // Validation process
void printErrorMessage(validation val);             // print error message to console


//--------------------------------- Infix to Postfix ----------------------------------//
//--------- Shunting yard algorithm ---------//
char getOperatorAssociativity(char c);				// Get the Associativity value of an operator
int  getOperatorPrecedence(char c);					// Get the Precedence value of an operator
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
validation operateCalculation(int arg1, char op, int arg2);
void evaluatePostfixExpression(Queue *postfix_queue); //Calculate the final result by evaluating the postfix expression,

//----------Exit------------//
void printExitMessage();

int main() {
    while (true)
    {
        char userInput[MAX_USER_INPUT_LENGTH] = {}; //Char array for storing user input(infix expression)

        promptUserInput(userInput);     //Taking input
        if (userWantExit(userInput)) {  //Exit?
            break;
        }

        validation validationResult = validate(userInput);  //Validate user input (infix expression)
        if (validationResult != PASS) {                     //If the input expression is invalid
            printErrorMessage(validationResult);            //print out the appropriate error message
            continue;
        }

        Queue postfix_queue;                                //Create a queue for storing converted postfix expression from user input (infix expression)
        convertInfixToPostfix(userInput, &postfix_queue);   //Infix to Postfix process, stores the result in "postfix_queue"

        //-------- Test print postfix ---------//
//        while (!postfix_queue.isEmpty()) {
//            cout << postfix_queue.peek() << " ";
//            postfix_queue.dequeue();
//        }
//        cout << endl;

        evaluatePostfixExpression(&postfix_queue);          //Evaluate the postfix expression (calculate the final result)

    }
    printExitMessage();
    return 0;
}

//---------------------------------- Processing Input --------------------------------//

//Function for comparing 2 char arrays
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
    while ((*buffer++ = *charArr++));  // remove leading spaces

}

void promptUserInput(char userInput[]) {
    printf("Enter your equation: ");
    cin.getline(userInput, MAX_USER_INPUT_LENGTH); //Take in user input from console
    trim(userInput); //Clear all the whitespaces before and after the user input
}

bool userWantExit(char userInput[]) {
    char Exit[] = "Exit";
    return compare(userInput, Exit); //Check if the user input expression is "Exit"
}

//---------------------------------- End Processing Input --------------------------------//


//---------------------------------- Validation --------------------------------//
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


bool isValidDecimalPoints(char decimalPoints[], int index) {
    while ( *(decimalPoints + index) )
    {
        if (*(decimalPoints + index) != '0') return false;
        index++;
    }

    return true;
}

void readNextInteger(char target[], const char *userInput, int* currentIndex) {
    int moves = 0;
    //Find the next index of whitespace (nextIndex) from the current index
    while (*(userInput + moves) && *(userInput + moves) != WHITESPACE
            && (isdigit(*(userInput + moves)) || *(userInput + moves) == '.')) moves++;
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

bool isValidInt(char number[]) {
    int index = 0;
    while ( *(number + index) )
    {
        if (isdigit(*(number + index))) index++;
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

//bool isValidToken(char token){
//    return isdigit(token)
//}

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
            printf("Division by 0 error!\n");
            break;
        }
        case INVALID_INTEGER_FORMAT: {
            printf("You entered the wrong format for the integer operand(s)!\n");
            break;
        }
        case OPERATORS_OPERANDS_NUMBER_MISMATCH: {
            printf("You entered an invalid sequence of expression, number of operators and operands mismatch !\n");
            printf("The format for 1 expression with a binary operator is: expr = [expr][whitespace][op][whitespace][expr] \n");
            printf("The format for 1 expression with a unary sign operator is: expr = [unary_sign_op][expr]\n");
            break;
        }
        case INVALID_TOKEN: {
            printf("Your input expression has invalid character(s) !\n");
            break;
        }
        case NUMBER_OF_LEFT_RIGHT_PAREN_MISMATCH:{
            printf("Your input expression has mismatch number of left and right parenthesises. More '(' than ')' or the opposite !\n");
            break;
        }
        case EMPTY_CONTENT_WITHIN_A_PAREN_PAIR:{
            printf("There is no expression between one (or many) pair(s) of parenthesises, expr = (expr)\n");
            break;
        }
        default: {
            break;
        }
    }
    printf("\n");
}

validation validate(const char *userInput){
    int currentIndex = 0, prevIndex = 0;        //previous and current value of user input pointer
    int leftParenCount = 0 , rightParenCount = 0;   //count the number of left and right parenthesises
    bool isContentWithinParen = true;
    bool isOperatorAfterInt = true;

    //Loop through the user input expression
    while (*(userInput)){
        prevIndex = currentIndex;               //Remember the current pointer of infix
        if (isLeftParenthesis(*userInput)){     //If the temporary token being read is a left parenthesis "("
            leftParenCount++;                   //Increase the left parenthesis count variable
            isContentWithinParen = false;       //assume that there is not yet any content inside the left and right parenthesis "()"
            clearWhiteSpaces(userInput, &currentIndex, prevIndex);
        }

        else if (isRightParenthesis(*userInput)){   //If the temporary token being read is a right parenthesis ")"
            rightParenCount++;                      //Increase the right parenthesis count variable
            clearWhiteSpaces(userInput, &currentIndex, prevIndex);
        }

        else if (isOperator(*userInput)){           //If the temporary token being read is an operator
            if (!isUnaryOperator(userInput)){
                isOperatorAfterInt = true;          //Note that there is an operator after the previous integer
            }
            clearWhiteSpaces(userInput, &currentIndex, prevIndex);
        }

        else if (isdigit(*userInput)){
            isContentWithinParen = true;         //Note that there is content within the temporary pair of parenthesises "()"
            char tempInt[MAX_ARG_LENGTH] = {};
            readNextInteger(tempInt, userInput, &currentIndex);         //Read the next integer token

            if (!isValidInt(tempInt)) return INVALID_INTEGER_FORMAT;    //Check for valid integer format
            if (!isInRange(tempInt)) return OUT_OF_RANGE;               //Check for out of range integer error

            if (!isOperatorAfterInt) return OPERATORS_OPERANDS_NUMBER_MISMATCH;
            else isOperatorAfterInt = false;
            clearWhiteSpaces(userInput, &currentIndex, prevIndex);
        }

        else {
            return INVALID_TOKEN;
        }

        //Move the 'infix' pointer from prevIndex to currentIndex position
        while (prevIndex != currentIndex) {
            userInput++; prevIndex++;
        }
    }

    //Check number of left and right parenthesis mismatch
    if (leftParenCount != rightParenCount) return NUMBER_OF_LEFT_RIGHT_PAREN_MISMATCH;

    //Check if there is content within each pair of
    if (!isContentWithinParen) return EMPTY_CONTENT_WITHIN_A_PAREN_PAIR;

    return PASS;
}

//---------------------------------- End Validation --------------------------------//

//---------------------------------- Infix to Postfix --------------------------------//
void convertInfixToPostfix(char *infix, Queue *postfix_queue){
    Stack operator_stack;
    int currentIndex = 0, prevIndex = 0;

    while (*infix) {
        //Remember the previous pointer of infix
        prevIndex = currentIndex;

        //If the token being read is left parenthesis "("
        if (isLeftParenthesis(*infix)) {
            leftParenthesisTokenHandling(infix, &operator_stack);
            clearWhiteSpaces(infix, &currentIndex, prevIndex);
        }

        // If the token being read is right parenthesis ")"
        else if (isRightParenthesis(*infix)) {
            rightParenthesisTokenHandling(postfix_queue, &operator_stack);
            clearWhiteSpaces(infix, &currentIndex, prevIndex);
        }

        // If the token being read is an operator
        else if ( isOperator(*infix) ) {
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

validation operateCalculation(int arg1, char op, int arg2, int* result) {
    switch (op) {
        case '+': {
            *result = add(arg1, arg2);
            break;
        }
        case '-': {
            *result = subtract(arg1, arg2);
            break;
        }
        case '/': {
            if (isDividingByZero(arg2)) {
                return DIVISION_BY_ZERO;
            }
            *result = divide(arg1, arg2);
            break;
        }
        case '*': {
            *result = multiply(arg1, arg2);
            break;
        }
        case '%': {
            *result = remainder(arg1, arg2);
            break;
        }
        default: {}
    }
    return PASS;
}

void evaluatePostfixExpression(Queue *postfix_queue) {
    Stack cal_stack;

    while (!postfix_queue->isEmpty())
    {
        char tempToken = postfix_queue->peek()[0];
        if ( isOperator(tempToken) )
        {
            if (tempToken == '~') {
                int tempInt = 0;
                if (cal_stack.isEmpty()) {printErrorMessage(OPERATORS_OPERANDS_NUMBER_MISMATCH); return;}
                else { tempInt = cal_stack.peek(true); cal_stack.pop();}
                tempInt = -tempInt;
                cal_stack.push(tempInt);
            }

            else if (tempToken == '&') {

            }

            else{
                int arg1, arg2, result = 0;

                if (cal_stack.isEmpty()) {printErrorMessage(OPERATORS_OPERANDS_NUMBER_MISMATCH); return;}
                else { arg2 = cal_stack.peek(true); cal_stack.pop(); }
                if (cal_stack.isEmpty()) {printErrorMessage(OPERATORS_OPERANDS_NUMBER_MISMATCH); return;}
                else { arg1 = cal_stack.peek(true); cal_stack.pop(); }

                if ( operateCalculation(arg1, tempToken, arg2, &result) == PASS ){
                    cal_stack.push(result);
                }
                else { printErrorMessage(DIVISION_BY_ZERO); return; }
            }

            postfix_queue->dequeue();
        }
        else {
            cal_stack.push(atoi(postfix_queue->peek()));
            postfix_queue->dequeue();
        }
    }

    int result = cal_stack.peek(true); cal_stack.pop();
    if (!cal_stack.isEmpty()) printErrorMessage(OPERATORS_OPERANDS_NUMBER_MISMATCH);
    else printResult(result);
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

