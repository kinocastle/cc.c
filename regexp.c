﻿//正则表达式

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

typedef struct AstNode AstNode;
typedef struct NfaNode NfaNode;
typedef struct NfaEdge NfaEdge;

enum { //AST kind
	ATOM,
	CHOICE, CONNECT, CLOSURE
};

struct AstNode {
	int kind;
	char value;
	AstNode *child[2];
};

struct NfaNode {
	NfaEdge *edge;
};

struct NfaEdge {
	char accept;
	NfaNode *node;
	NfaEdge *next;
};

char *p;

AstNode* newAstNode(int kind) {
	AstNode *n = (AstNode*)malloc(sizeof(AstNode));
	n->kind = kind;
	n->child[0] = n->child[1] = NULL;
	return n;
}

NfaNode* newNfaNode(void) {
	NfaNode *n = (NfaNode*)malloc(sizeof(NfaNode));
	n->edge = NULL;
	return n;
}

void addNfaEdge(char accept, NfaNode *bgn, NfaNode *end) {
	NfaEdge *p = bgn->edge;
	bgn->edge = (NfaEdge*)malloc(sizeof(NfaEdge));
	bgn->edge->accept = accept;
	bgn->edge->node = end;
	bgn->edge->next = p;
}

void printAst(AstNode *n, int indent) {
	for(int i = 0; i < indent * 2; i++) printf(" ");
	switch(n->kind) {
	case ATOM: printf("%c\n", n->value); return;
	case CLOSURE: printf("*\n"); printAst(n->child[0], indent + 1); break;
	case CHOICE: printf("|\n"); printAst(n->child[0], indent + 1); printAst(n->child[1], indent + 1); break;
	case CONNECT: printf(".\n"); printAst(n->child[0], indent + 1); printAst(n->child[1], indent + 1); break;
	default: assert(0);
	}
}

int lev(char opr) {
	if(opr == '\0' || opr == ')') return 0;
	else if(opr == '|') return 1;
	else if(opr == '*') return 3;
	else return 2;
}

AstNode* expr(char last_opr) {
	AstNode *n;
	if(*p == '(') {
		p++;
		n = expr(')');
		if(*p == ')') p++; else { printf("error!\n"); exit(-1); }
	} else {
		n = newAstNode(ATOM);
		n->value = *p++;
	}

	while(lev(*p) > lev(last_opr)) {
		AstNode *_n = n;
		n = newAstNode(CONNECT);
		n->child[0] = _n;
		if(*p == '|') { n->kind = CHOICE; char opr = *p++; n->child[1] = expr(opr); }
		else if(*p == '*') { n->kind = CLOSURE; p++; }
		else { n->child[1] = expr(*p); }
	}
	return n;
}

void genNfa(AstNode *ast, NfaNode *bgn, NfaNode *end) {
	if(ast->kind == ATOM) {
		addNfaEdge(ast->value, bgn, end);
	} else if(ast->kind == CHOICE) {
		genNfa(ast->child[0], bgn, end);
		genNfa(ast->child[1], bgn, end);
	} else if(ast->kind == CONNECT) {
		NfaNode *tmp = newNfaNode();
		genNfa(ast->child[0], bgn, tmp);
		genNfa(ast->child[1], tmp, end);
	} else if(ast->kind == CLOSURE) {
		NfaNode *tmp = newNfaNode();
		addNfaEdge('\0', bgn, tmp);
		addNfaEdge('\0', tmp, end);
		genNfa(ast->child[0], tmp, tmp);
	} else assert(0);
}

int runNfa(char *str, NfaNode *bgn, NfaNode *end) {
	int r = 0;
	if(*str == '\0' && bgn == end) return 1;
	for(NfaEdge *i = bgn->edge; i; i = i->next) {
		if(i->accept == '\0' && bgn != end) {
			r = runNfa(str, i->node, end);
		} else if(i->accept == *str) {
			r = runNfa(str + 1, i->node, end);
		}
		if(r) break;
	}
	return r;
}

int match(char *exp, char *str) {
	p = exp;
	AstNode *ast = expr('\0');
	NfaNode *bgn = newNfaNode();
	NfaNode *end = newNfaNode();
	genNfa(ast, bgn, end);
	return runNfa(str, bgn, end);
}

int main(int argc, char *argv[]) {
	if(argc != 3) { printf("error!\n"); exit(-1); }
	printf("%s", match(argv[1], argv[2])? "true": "false");
}
