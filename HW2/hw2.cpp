#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stack>
#include <string>
#include <cstring>
#include <cassert>
using namespace std;

// �� ������(@, #, &)�� ���� ���� ����� 2���� �迭
char result_at[3][3];
char result_hash[3][3];
char result_ampersand[3][3];

// operations.txt ������ �о �� �������� ���갪�� �迭�� ����
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
		// ������ @, #, &�� �о�鿩�� ������ type�� �з�
		if (strchr("@#&", readFile.peek()) != NULL) {
			operation = readFile.peek();
			readFile.ignore();
		}
		// ����, ����, �� ���ڴ� �о�鿩�� ignore
		else if (readFile.peek() == ' ' || readFile.peek() == '\n' || readFile.peek() == '\t')
			readFile.ignore();
		// ���� ��(a, b, c)�� �о�鿩�� �� �������� ���� �� ���(2���� �迭)�� ����
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

// �Է¹��� infix ���� postfix ������ ��ȯ. �Ŀ� ������ �ִ� ��� "Error!"�� ��ȯ
string infixToPostfix(string infix)
{
	string postfix;
	stack<char> operations;
	
	if (strchr("@#&", infix[0]) != NULL ||
		strchr("@#&", infix[infix.length() - 1]) != NULL)
		return "Error!";

	for (size_t i = 0; i < infix.length(); i++) {
		// �ǿ�����(a, b, c)�� �״�� ���
		if (strchr("abc", infix[i]) != NULL)
			postfix += infix[i];

		// ������(@, #, &)�� stack�� ���κ��� �켱������ ���ų� �� ���� �����ڰ� ������,
		// stack�� �ִ� �����ڸ� pop�ؼ� ����ϰ�, ������ push
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
		else if (strchr("&", infix[i]) != NULL) {		// & �����ڰ� �켱���� ���� ����
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

		// ���� ��ȣ���� �״�� stack�� push
		else if (strchr("({[", infix[i]) != NULL)
			operations.push(infix[i]);

		// ������ ��ȣ���� ��Ī�Ǵ� ���� ������ ���� ��ȣ�� ���ö� ���� �����ڵ��� pop�Ͽ� ���
		else if (infix[i] == ')') {
			while (true) {
				if (operations.empty())		// ��ȣ ���� - ������ ��ȣ�� �� ���� ���
					return "Error!";

				if (operations.top() == '(')	// ��Ī�Ǵ� ���� ��ȣ�� ���� ������, pop�ؼ� ���
					break;
				postfix += operations.top();
				operations.pop();
			}
			operations.pop();	// ��Ī�Ǵ� ���� ��ȣ ����
		}
		else if (infix[i] == '}') {
			while (true) {
				if (operations.empty())		// ��ȣ ���� - ������ ��ȣ�� �� ���� ���
					return "Error!";

				if (operations.top() == '{')	// ��Ī�Ǵ� ���� ��ȣ�� ���� ������, pop�ؼ� ���
					break;
				postfix += operations.top();
				operations.pop();
			}
			operations.pop();	// ��Ī�Ǵ� ���� ��ȣ ����
		}
		else if (infix[i] == ']') {
			while (true) {
				if (operations.empty())		// ��ȣ ���� - ������ ��ȣ�� �� ���� ���
					return "Error!";

				if (operations.top() == '[')	// ��Ī�Ǵ� ���� ��ȣ�� ���� ������, pop�ؼ� ���
					break;
				postfix += operations.top();
				operations.pop();
			}
			operations.pop();	// ��Ī�Ǵ� ���� ��ȣ ����
		}
	}

	// stack�� �����ִ� �����ڵ��� ��� pop�ؼ� ���
	while (!operations.empty()) {
		// ��ȣ ���� - ���� ��ȣ�� �� ���� ���
		if (strchr("({[", operations.top()) != NULL)
			return "Error!";

		postfix += operations.top();
		operations.pop();
	}

	return postfix;
}

void evaluate_stack(stack<char>& operands, char operation);

// postfix �Ŀ��� �ǿ����ڸ� stack�� �����鼭 �����ڿ� ���� ��� ����
// ���� ��� ���� operands stack�� �ϳ� ���� (operands.top())
void read_and_evaluate(string postfix, stack<char>& operands)
{
	for (size_t i = 0; i < postfix.length(); i++) {
		// �ǿ����ڴ� stack�� push
		if (strchr("abc", postfix[i]) != NULL)
			operands.push(postfix[i]);
		// �����ڰ� ������, stack���� �ǿ����� 2���� ������ ��� �� ��� ���� �ٽ� stack�� push
		else if (strchr("@#&", postfix[i]) != NULL)
			evaluate_stack(operands, postfix[i]);
	}
}

// 2���� �ǿ����ڸ� stack���� pop�Ͽ� ����ϰ�, ��� ���� �ٽ� stack�� push
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
	string expression;		// �Է¹��� ��(infix)
	string postfix;			// �Է¹��� �� infix�� postfix���� ��ȯ
	stack<char> operands;	// �ǿ����ڸ� ���� stack

	// �� �����ڵ��� ���� ���� 2���� �迭�� ����
	saveOperationResult(result_at, result_hash, result_ampersand);

	// ���� "EOI" �Է� ������, �� ������ �Է¹���.
	while (true) {
		cout << "Input: ";
		cin >> expression;
		if (expression == "EOI")
			break;

		// �Է¹��� ���� infix���� postfix�� ��ȯ
		postfix = infixToPostfix(expression);
		// �Ŀ� ������ �ִ� ���, Error �޽����� ����ϰ� ���� ���� �Է� ����
		if (postfix == "Error!") {
			cout << "Error!" << endl << endl;
			continue;
		}
		
		// ��ȯ�� postfix ���� stack�� �̿��Ͽ� ���
		read_and_evaluate(postfix, operands);

		cout << "Output: " << operands.top() << endl << endl;
		operands.pop();
	}

	return 0;
}