#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stack>
#include <string>
#include <cstring>
#include <cassert>
using namespace std;

// 각 연산자(@, #, &)의 연산 값이 저장될 2차원 배열
char result_at[3][3];
char result_hash[3][3];
char result_ampersand[3][3];

// operations.txt 파일을 읽어서 각 연산자의 연산값을 배열에 저장
void saveOperationResult(char at[][3], char hash[][3], char ampersand[][3])
{
	char operation = 0, result;
	int row = 0, col = 0;

	ifstream readFile;
	readFile.open("operations.txt");
	if (!readFile.is_open()) {
		assert("File open() error!");
		exit(-1);
	}

	while (!readFile.eof()) {
		// 연산자 @, #, &는 읽어들여서 연산자 type을 분류
		if (strchr("@#&", readFile.peek()) != NULL) {
			operation = readFile.peek();
			readFile.ignore();
		}
		// 공백, 개행, 탭 문자는 읽어들여서 ignore
		else if (readFile.peek() == ' ' || readFile.peek() == '\n' || readFile.peek() == '\t')
			readFile.ignore();
		// 연산 값(a, b, c)는 읽어들여서 각 연산자의 연산 값 행렬(2차원 배열)에 저장
		else if (strchr("abc", readFile.peek()) != NULL) {
			if (row == 3 && col == 3) {
				row = 0;
				col = 0;
			}
			if (col == 3) {
				row = (row + 1) % 3;
				col = 0;
			}

			readFile >> result;
			switch (operation) {
			case '@':
				at[row][col++] = result;
				break;

			case '#':
				hash[row][col++] = result;
				break;

			case '&':
				ampersand[row][col++] = result;
				break;
			}
		}
	}

	readFile.close();
}

// 입력받은 infix 식을 postfix 식으로 변환. 식에 오류가 있는 경우 "Error!"을 반환
string infixToPostfix(string infix)
{
	string postfix;
	stack<char> operations;
	
	if (strchr("@#&", infix[0]) != NULL ||
		strchr("@#&", infix[infix.length() - 1]) != NULL)
		return "Error!";

	for (size_t i = 0; i < infix.length(); i++) {
		// 피연산자(a, b, c)는 그대로 출력
		if (strchr("abc", infix[i]) != NULL)
			postfix += infix[i];

		// 연산자(@, #, &)는 stack에 본인보다 우선순위가 같거나 더 높은 연산자가 있으면,
		// stack에 있는 연산자를 pop해서 출력하고, 본인을 push
		else if (strchr("@#", infix[i]) != NULL) {
			if (strchr("@#&", infix[i + 1]) != NULL)
				return "Error!";

			while (true) {
				if (operations.empty() || strchr("@#&", operations.top()) == NULL)
					break;
				postfix += operations.top();
				operations.pop();
			}
			operations.push(infix[i]);
		}
		else if (strchr("&", infix[i]) != NULL) {		// & 연산자가 우선순위 제일 높음
			if (strchr("@#&", infix[i + 1]) != NULL)
				return "Error!";

			while (true) {
				if (operations.empty() || operations.top() != '&')
					break;
				postfix += operations.top();
				operations.pop();
			}
			operations.push(infix[i]);
		}

		// 왼쪽 괄호들은 그대로 stack에 push
		else if (strchr("({[", infix[i]) != NULL)
			operations.push(infix[i]);

		// 오른쪽 괄호들은 매칭되는 같은 종류의 왼쪽 괄호가 나올때 까지 연산자들을 pop하여 출력
		else if (infix[i] == ')') {
			while (true) {
				if (operations.empty())		// 괄호 오류 - 오른쪽 괄호가 더 많은 경우
					return "Error!";

				if (operations.top() == '(')	// 매칭되는 왼쪽 괄호가 나올 때까지, pop해서 출력
					break;
				postfix += operations.top();
				operations.pop();
			}
			operations.pop();	// 매칭되는 왼쪽 괄호 제거
		}
		else if (infix[i] == '}') {
			while (true) {
				if (operations.empty())		// 괄호 오류 - 오른쪽 괄호가 더 많은 경우
					return "Error!";

				if (operations.top() == '{')	// 매칭되는 왼쪽 괄호가 나올 때까지, pop해서 출력
					break;
				postfix += operations.top();
				operations.pop();
			}
			operations.pop();	// 매칭되는 왼쪽 괄호 제거
		}
		else if (infix[i] == ']') {
			while (true) {
				if (operations.empty())		// 괄호 오류 - 오른쪽 괄호가 더 많은 경우
					return "Error!";

				if (operations.top() == '[')	// 매칭되는 왼쪽 괄호가 나올 때까지, pop해서 출력
					break;
				postfix += operations.top();
				operations.pop();
			}
			operations.pop();	// 매칭되는 왼쪽 괄호 제거
		}
	}

	// stack에 남아있는 연산자들을 모두 pop해서 출력
	while (!operations.empty()) {
		// 괄호 오류 - 왼쪽 괄호가 더 많은 경우
		if (strchr("({[", operations.top()) != NULL)
			return "Error!";

		postfix += operations.top();
		operations.pop();
	}

	return postfix;
}

void evaluate_stack(stack<char>& operands, char operation);

// postfix 식에서 피연산자를 stack에 담으면서 연산자에 따라 계산 수행
// 최종 결과 값은 operands stack에 하나 남음 (operands.top())
void read_and_evaluate(string postfix, stack<char>& operands)
{
	for (size_t i = 0; i < postfix.length(); i++) {
		// 피연산자는 stack에 push
		if (strchr("abc", postfix[i]) != NULL)
			operands.push(postfix[i]);
		// 연산자가 나오면, stack에서 피연산자 2개를 꺼내서 계산 후 결과 값을 다시 stack에 push
		else if (strchr("@#&", postfix[i]) != NULL)
			evaluate_stack(operands, postfix[i]);
	}
}

// 2개의 피연산자를 stack에서 pop하여 계산하고, 계산 값을 다시 stack에 push
void evaluate_stack(stack<char>& operands, char operation)
{
	char operand1, operand2;
	operand2 = operands.top();
	operands.pop();
	operand1 = operands.top();
	operands.pop();

	switch (operation) {
	case '@':
		operands.push(result_at[operand1 - 'a'][operand2 - 'a']);
		break;
	case '#':
		operands.push(result_hash[operand1 - 'a'][operand2 - 'a']);
		break;
	case '&':
		operands.push(result_ampersand[operand1 - 'a'][operand2 - 'a']);
		break;
	}
}



int main()
{
	string expression;		// 입력받을 식(infix)
	string postfix;			// 입력받은 식 infix를 postfix으로 변환
	stack<char> operands;	// 피연산자를 담을 stack

	// 각 연산자들의 연산 값을 2차원 배열에 저장
	saveOperationResult(result_at, result_hash, result_ampersand);

	// 식을 "EOI" 입력 전까지, 줄 단위로 입력받음.
	while (true) {
		cout << "Input: ";
		cin >> expression;
		if (expression == "EOI")
			break;

		// 입력받은 식을 infix에서 postfix로 변환
		postfix = infixToPostfix(expression);
		// 식에 오류가 있는 경우, Error 메시지를 출력하고 식을 새로 입력 받음
		if (postfix == "Error!") {
			cout << "Error!" << endl << endl;
			continue;
		}
		
		// 변환한 postfix 식을 stack을 이용하여 계산
		read_and_evaluate(postfix, operands);

		cout << "Output: " << operands.top() << endl << endl;
		operands.pop();
	}

	return 0;
}