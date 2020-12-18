# Data Structure HW2 - Stack Calculator

## Calculator Algorithm - 계산기 구현 알고리즘

### 1. 입력받은 Infix 식을 Postfix 식으로 변환

Infix 식을 앞에서부터 차례대로 읽으면서

- 피연산자(operand)는 그대로 출력
- 연산자(operation)는

  1 ) Stack이 empty하면, 연산자를 그대로 Stack에 push

  2 ) Stack이 empty하지 않으면, Stack의 top부터 본인보다 우선순위가 같거나 더 높은
  연산자를 모두 pop해서 출력하고(본인보다 우선순위 낮은 연산자 나올때까지), 본인을 push

### 2. 변환한 Postfix 식을 Stack을 이용하여 계산

Postfix 식을 앞에서부터 차례대로 읽으면서

- 피연산자(operand)는 Stack에 push
- 연산자(operation)를 만나면 Stack에 있는 피연산자 2개를 pop해서
  계산하고, 계산 결과 값을 다시 Stack에 push
- 최종 계산 결과 값은 Stack에 1개 남아있는 피연산자
