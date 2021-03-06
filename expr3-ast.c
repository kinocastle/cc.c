//表达式计算器
//递归下降分析
//遇到终结符必须主动移动tk

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#define MAXSIZE 1000

enum {
	//type
	INT,
	//node kind
	ADD, SUB, MUL, DIV, ATOM
};

typedef struct Node {
	int kind;
	int value;
	struct Node *child[2];
} Node;

int tki, *sp;
char *tks, *p;

Node* expr(void);

void next(void) {
	tks = ""; tki = -1;
	while(*p) {
		if(*p >= '0' && *p <= '9') {
			int len = 0; char *_p = p;
			while(*p >= '0' && *p <= '9') {
				len++; p++;
			}
			tki = INT;
			tks = (char*)malloc(sizeof(char) * (len+1));
			strncpy(tks, _p, len);
			tks[len] = '\0';
			return;
		}
		else if(*p == '+') { tks = "+"; p++; return; }
		else if(*p == '-') { tks = "-"; p++; return; }
		else if(*p == '*') { tks = "*"; p++; return; }
		else if(*p == '/') { tks = "/"; p++; return; }
		else if(*p == '(') { tks = "("; p++; return; }
		else if(*p == ')') { tks = ")"; p++; return; }
		else { //跳过不能识别的符号
			p++;
		}
	}
}

Node* newNode(int kind) {
	Node *n = (Node*)malloc(sizeof(Node));
	n->kind = kind;
	n->child[0] = n->child[1] = NULL;
	return n;
}

Node* atom(void) { //atom->int | "(" expr ")"
	Node *n;
	if(tki == INT) {
		n = newNode(ATOM);
		n->value = atoi(tks);
		next();
	} else if(!strcmp(tks, "(")) {
		next();
		n = expr();
		if(!strcmp(tks, ")")) next(); else { printf("error!\n"); exit(-1); } //"("无法匹配到")"
	} else if(!strcmp(tks, "-")) {
		next();
		n = newNode(SUB);
		n->child[0] = newNode(ATOM);
		n->child[0]->value = 0;
		n->child[1] = atom();
	} else { printf("error!\n"); exit(-1); }
	return n;
}

Node* muldiv(void) { //muldiv->atom ("*" atom | "/" atom)*
	Node *n = atom();
	while(!strcmp(tks, "*") || !strcmp(tks, "/")) {
		char *opr = tks;
		next();
		Node *_n = n;
		n = !strcmp(opr, "*")? newNode(MUL): newNode(DIV);
		n->child[0] = _n;
		n->child[1] = atom();
	}
	return n;
}

Node* addsub(void) { //addsub->muldiv ("+" muldiv | "-" muldiv)*
	Node *n = muldiv();
	while(!strcmp(tks, "+") || !strcmp(tks, "-")) {
		char *opr = tks;
		next();
		Node *_n = n;
		n = !strcmp(opr, "+")? newNode(ADD): newNode(SUB);
		n->child[0] = _n;
		n->child[1] = muldiv();
	}
	return n;
}

Node* expr(void) { //expr->addsub
	return addsub();
}

int runNode(Node *n) {
	if(n->kind == ATOM) return n->value;
	int lopr = runNode(n->child[0]);
	int ropr = runNode(n->child[1]);
	switch(n->kind) {
	case ADD: return lopr + ropr;
	case SUB: return lopr - ropr;
	case MUL: return lopr * ropr;
	case DIV: return lopr / ropr;
	}
}

void printNode(Node *n, int indent) {
	for(int i = 0; i < indent * 2; i++) printf(" ");
	switch(n->kind) {
	case ATOM: printf("%d\n", n->value); return;
	case ADD: printf("+\n"); break;
	case SUB: printf("-\n"); break;
	case MUL: printf("*\n"); break;
	case DIV: printf("/\n"); break;
	}
	printNode(n->child[0], indent + 1);
	printNode(n->child[1], indent + 1);
}

int main(int argc, char *argv[]) {
	if(argc != 2) { printf("error!\n"); exit(-1); }
	p = argv[1];
	next();
	Node *n = expr();
	if(strcmp(tks, ";") && strcmp(tks, "")) { printf("error!\n"); exit(-1); }
	printf("%d\n", runNode(n));
	printNode(n, 0);
	return 0;
}
