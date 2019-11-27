class Lexer:
    def __init__(self, expression):
        self.token = None
        self.expression = (expression + '$').replace(" ", "")

    def next_token(self):
        if self.token:
            temp = self.token
            self.token = None
            return temp
        else:
            return self._next_token()

    def peek_token(self):
        if not self.token:
            self.token = self._next_token()
        return self.token

    def _next_token(self):
        if self.expression == '':
            return None
        c = self.expression[0]
        if c == '(' or c == ')' or c == '+' or c == '-' or c == '*' or c == '/' or c == '$':
            self.expression = self.expression[1:]
            return c
        else:
            result = None
            i = 1
            try:
                while i < len(self.expression):
                    if self.expression[i - 1] == '.':
                        i += 1
                    result = float(self.expression[:i])
                    i += 1
            except:
                self.expression = self.expression[i - 1:]
                return 'n'  # result
        t = len(self.expression)
        self.expression = self.expression[i - 1:]
        return 'n' if i == t else result


class LR1Table:
    def __init__(self):
        self.grammer = {}
        self.action = {}
        self.goto = {}


class LR1Parser:
    def __init__(self, table, expression):
        self.stack = []
        self.table = table
        self.lexer = Lexer(expression)
        self.stack.append((0, ''))

    def _parse(self):
        print(self.stack)
        s = self.stack[-1][0]
        a = self.lexer.peek_token()
        print(str(s) + ',' + self.stack[-1][1] + ' -- ' + a)
        action = self.table.action[s][a]
        if action[0] == 'S':
            self.stack.append((action[1], a))
            self.lexer.next_token()
            return True
        elif action[0] == 'R':
            r_expression = self.table.grammer[action[1]]
            for i in range(len(r_expression[1])):
                self.stack.pop()
            if action[1] != 0:
                self.stack.append((
                    self.table.goto[self.stack[-1][0]][r_expression[0]], r_expression[0]))
            print(r_expression[0] + '->' +
                  r_expression[1])
            return action[1] != 0
        else:
            print('Error in parsing')
            return False

    def parse(self):
        result = True
        while (result):
            result = self._parse()


if __name__ == '__main__':
    table = LR1Table()
    table.grammer = {
        0: ("S", "E"),
        1: ("E", "E+T"),
        2: ("E", "E-T"),
        3: ("E", "T"),
        4: ("T", "T*F"),
        5: ("T", "T/F"),
        6: ("T", "F"),
        7: ("F", "(E)"),
        8: ("F", "n")
    }
    table.goto = {
        0: {
            'E': 1,
            'T': 2,
            'F': 3
        },
        4: {
            'E': 10,
            'T': 2,
            'F': 3
        },
        6: {
            'T': 11,
            'F': 3
        },
        7: {
            'T': 12,
            'F': 3
        },
        8: {
            'F': 13
        },
        9: {
            'F': 14
        }
    }
    table.action = {
        0: {
            '(': ('S', 4),
            'n': ('S', 5)
        },
        1: {
            '+': ('S', 6),
            '-': ('S', 7),
            '$': ('R', 0)
        },
        2: {
            '+': ('R', 3),
            '-': ('R', 3),
            '*': ('S', 8),
            '/': ('S', 9),
            ')': ('R', 3),
            '$': ('R', 3)
        },
        3: {
            '+': ('R', 6),
            '-': ('R', 6),
            '*': ('R', 6),
            '/': ('R', 6),
            ')': ('R', 6),
            '$': ('R', 6)
        },
        4: {
            '(': ('S', 4),
            'n': ('S', 5)
        },
        5: {
            '+': ('R', 8),
            '-': ('R', 8),
            '*': ('R', 8),
            '/': ('R', 8),
            ')': ('R', 8),
            '$': ('R', 8)
        },
        6: {
            '(': ('S', 4),
            'n': ('S', 5)
        },
        7: {
            '(': ('S', 4),
            'n': ('S', 5)
        },
        8: {
            '(': ('S', 4),
            'n': ('S', 5)
        },
        9: {
            '(': ('S', 4),
            'n': ('S', 5)
        },
        10: {
            '+': ('S', 6),
            '-': ('S', 7),
            ')': ('S', 15)
        },
        11: {
            '+': ('R', 1),
            '-': ('R', 1),
            '*': ('S', 8),
            '/': ('S', 9),
            ')': ('R', 1),
            '$': ('R', 1)
        },
        12: {
            '+': ('R', 2),
            '-': ('R', 2),
            '*': ('S', 8),
            '/': ('S', 9),
            ')': ('R', 2),
            '$': ('R', 2)
        },
        13: {
            '+': ('R', 4),
            '-': ('R', 4),
            '*': ('R', 4),
            '/': ('R', 4),
            ')': ('R', 4),
            '$': ('R', 4)
        },
        14: {
            '+': ('R', 5),
            '-': ('R', 5),
            '*': ('R', 5),
            '/': ('R', 5),
            ')': ('R', 5),
            '$': ('R', 5)
        },
        15: {
            '+': ('R', 7),
            '-': ('R', 7),
            '*': ('R', 7),
            '/': ('R', 7),
            ')': ('R', 7),
            '$': ('R', 7)
        }
    }
    parser = LR1Parser(table, "(1 + 2) * 3 - 4")
    parser.parse()
