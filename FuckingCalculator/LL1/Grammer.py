class Grammer:
    def __init__(self):
        self.startSymbol = 'S'  # default start symbol
        self.nonTerminalSymbol = set()
        self.terminalSymbol = set()
        self.generatorExpression = {}

        # Temp variable
        self.nullable = {}
        self.firstSymbols = {}
        self.followSymbols = {}
        self.llTable = {}

    def get_unused_non_terminal_symbol(self):
        tmp = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
        for i in tmp:
            if i not in self.nonTerminalSymbol:
                self.nonTerminalSymbol.add(i)
                return i

    def solve_left_recursion(self):
        keys = list(self.generatorExpression.keys())
        for k in keys:
            v = self.generatorExpression[k]
            leftRecu = [i for i in v if i[0] == k]
            if leftRecu:
                newSymbol = self.get_unused_non_terminal_symbol()
                self.generatorExpression[k] = [
                    i + newSymbol for i in v if i[0] != k]
                self.generatorExpression[newSymbol] = [
                    i[1:] + newSymbol for i in leftRecu]
                self.generatorExpression[newSymbol].append('')

    def compute_first_and_follow_set(self):
        old_nullable = {}
        for k in self.nonTerminalSymbol:
            self.nullable[k] = False
        while old_nullable != self.nullable:
            old_nullable = self.nullable.copy()
            for k, v in self.generatorExpression.items():
                self.nullable[k] = False
                for expressions in v:
                    nullable = True
                    for i in expressions:
                        if not (i in self.nonTerminalSymbol and '' in self.generatorExpression[i]):
                            nullable = False
                    self.nullable[k] = self.nullable[k] or nullable

        old_first_set = set()
        for k in self.nonTerminalSymbol:
            self.firstSymbols[k] = set()
        for k, v in self.generatorExpression.items():
            for expressions in v:
                for i in expressions:
                    if i not in self.nonTerminalSymbol:
                        self.firstSymbols[i] = set(i)
        while(old_first_set != self.firstSymbols):
            old_first_set = self.firstSymbols.copy()
            for k, v in self.generatorExpression.items():
                for expressions in v:
                    add_new_first = True
                    if expressions == '':
                        self.firstSymbols[k].add('')
                    for i in expressions:
                        if add_new_first:
                            self.firstSymbols[k] = self.firstSymbols[k].union(
                                self.firstSymbols[i])
                            add_new_first = add_new_first and (
                                i in self.nonTerminalSymbol and self.nullable[i])

        old_follow_set = set()
        for k in self.nonTerminalSymbol:
            self.followSymbols[k] = set()
        for k, v in self.generatorExpression.items():
            for expressions in v:
                for i in expressions:
                    if i not in self.nonTerminalSymbol:
                        self.followSymbols[i] = set(i)
        self.followSymbols[self.startSymbol].add('$')
        while(old_follow_set != self.followSymbols):
            old_follow_set = self.followSymbols.copy()
            # print(self.generatorExpression.items())
            for key, v in self.generatorExpression.items():
                for expressions in v:
                    for i in range(len(expressions)):
                        add_follow = True
                        for j in range(i + 1, len(expressions)):
                            add_follow = add_follow and (
                                expressions[j] in self.nonTerminalSymbol and self.nullable[expressions[j]])
                        if add_follow:
                            self.followSymbols[
                                expressions[i]] = self.followSymbols[
                                    expressions[i]].union(
                                        self.followSymbols[key])
                        for j in range(i + 1, len(expressions)):
                            add_follow = True
                            for k in range(i + 1, j):
                                add_follow = add_follow and (
                                    expressions[k] in self.nonTerminalSymbol and self.nullable[expressions[k]])
                            if add_follow:
                                self.followSymbols[expressions[i]] = self.followSymbols[expressions[i]].union(
                                    self.firstSymbols[expressions[j]])
        keys = list(self.firstSymbols.keys())
        for k in keys:
            if k not in self.nonTerminalSymbol:
                self.firstSymbols.pop(k)
        keys = list(self.followSymbols.keys())
        for k in keys:
            if k not in self.nonTerminalSymbol:
                self.followSymbols.pop(k)
        for k, v in self.followSymbols.items():
            if '' in v:
                v.remove('')

    def _get_first(self, expression):
        if expression == '':
            return set('')
        result = set()
        for i in expression:
            if i in self.nonTerminalSymbol:
                result = result.union(self.firstSymbols[i])
                if not self.nullable[i]:
                    return result
            else:
                return result.union(i)
        return result

    def get_first(self, expression):
        result = self._get_first(expression)
        if '' in result:
            result.remove('')
        return result

    def get_nullable(self, expression):
        if expression == '':
            return True
        return '' in self._get_first(expression)

    def build_ll_table(self):
        for k in self.nonTerminalSymbol:
            self.llTable[k] = {}
        for k, v in self.generatorExpression.items():
            for expressions in v:
                for i in self.get_first(expressions):
                    self.llTable[k][i] = expressions
                if self.get_nullable(expressions):
                    for i in self.followSymbols[k]:
                        self.llTable[k][i] = expressions


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


class LLParser:
    def __init__(self, grammer, expression):
        self.grammer = grammer
        self.lexer = Lexer(expression)
        self.stack = ['$', self.grammer.startSymbol]

    def _parse(self):
        if self.stack[-1] not in self.grammer.nonTerminalSymbol:
            if self.stack[-1] == self.lexer.peek_token():
                self.stack.pop()
                self.lexer.next_token()
            else:
                raise RuntimeError
        else:
            expression = self.grammer.llTable[self.stack[-1]
                                              ][self.lexer.peek_token()]
            print(self.stack[-1] + ' -> ' +
                  ('\'\'' if expression == '' else expression))
            self.stack.pop()
            for x in expression[::-1]:
                self.stack.append(x)

    def parse(self):
        while self.stack[-1] != '$':
            self._parse()


if __name__ == '__main__':
    expression = "(1 + 2) + 2.5"
    grammer = Grammer()
    grammer.startSymbol = 'E'
    grammer.nonTerminalSymbol.add('E')
    grammer.generatorExpression['E'] = ['E+T', 'E-T', 'T']
    grammer.nonTerminalSymbol.add('T')
    grammer.generatorExpression['T'] = ['T*F', 'T/F', 'F']
    grammer.nonTerminalSymbol.add('F')
    grammer.terminalSymbol.add('n')
    grammer.generatorExpression['F'] = ['(E)', 'n']
    print(grammer.generatorExpression)
    grammer.solve_left_recursion()
    print(grammer.generatorExpression)
    grammer.compute_first_and_follow_set()
    print(grammer.nullable)
    print(grammer.firstSymbols)
    print(grammer.followSymbols)
    grammer.build_ll_table()
    print(grammer.llTable)
    ll = LLParser(grammer, expression)
    ll.parse()
