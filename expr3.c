//����ʽ������
//�ݹ��½�����
//�����ս�����������ƶ�tk

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#define MAXSIZE 1000

enum { INT };

int tki, *sp;
char *tks, *p;

void next() {
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
		else { //��������ʶ��ķ���
			p++;
		}
	}
}

void expr();
void atom() { //atom -> int | "(" expr ")"
	if(tki == INT) {
		*sp = atoi(tks);
		next();
	} else if(!strcmp(tks, "(")) {
		next();
		expr();
		if(strcmp(tks, ")")) { printf("error!\n"); exit(-1); } //"("�޷�ƥ�䵽")"
		next();
	} else { printf("error!\n"); exit(-1); }
}

void muldiv() { //muldiv -> atom ("*" atom | "/" atom)*
	atom();
	while(1) {
		if(!strcmp(tks, "*")) {
			sp++;
			next();
			atom();
			int opr2 = *sp;
			int opr1 = *--sp;
			*sp = opr1 * opr2;
		} else if(!strcmp(tks, "/")) {
			sp++;
			next();
			atom();
			int opr2 = *sp;
			int opr1 = *--sp;
			*sp = opr1 / opr2;
		} else break;
	}
}

void addsub() { //addsub -> muldiv ("+" muldiv | "-" muldiv)*
	muldiv();
	while(1) {
		if(!strcmp(tks, "+")) {
			sp++;
			next();
			muldiv();
			int opr2 = *sp;
			int opr1 = *--sp;
			*sp = opr1 + opr2;
		} else if(!strcmp(tks, "-")) {
			sp++;
			next();
			muldiv();
			int opr2 = *sp;
			int opr1 = *--sp;
			*sp = opr1 - opr2;
		} else break;
	}
}

void expr() { //expr -> addsub
	addsub();
}

int main(int argc, char *argv[]) {
	if(argc != 2) { printf("error!\n"); exit(-1); }
	sp = (int*)malloc(MAXSIZE * sizeof(int));
	p = argv[1];
	next();
	expr();
	if(strcmp(tks, ";") && strcmp(tks, "")) { printf("error!\n"); exit(-1); }
	printf("%d\n", *sp);
	return 0;
}