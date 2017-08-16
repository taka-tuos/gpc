#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define GPC_BVAL "b@[%%r31:%d]"
#define GPC_WVAL "w@[%%r31:%d]"
#define GPC_DVAL "d@[%%r31:%d]"

#define GPC_BPTR "b[%%r28]"
#define GPC_WPTR "w[%%r28]"
#define GPC_DPTR "d[%%r28]"

#define GPC_MOV "    mov(%%r30,%s)\n"
#define GPC_ADD "    add(%%r30,%s)\n"
#define GPC_SUB "    sub(%%r30,%s)\n"
#define GPC_MUL "    imul(%%r30,%s)\n"
#define GPC_DIV "    idiv(%%r30,%s)\n"
#define GPC_AND "    and(%%r30,%s)\n"
#define GPC_OR  "    or(%%r30,%s)\n"
#define GPC_XOR "    xor(%%r30,%s)\n"
#define GPC_SAR "    sar(%%r30,%s)\n"
#define GPC_SAL "    sal(%%r30,%s)\n"
#define GPC_MOD "    mov(%%r29,%%r30)\n    mov(%%r27,%s)\n    idiv(%%r30,%%r3)\n    mov(%%r28,%%r27)\n    imul(%%r28,%%r30)\n    sub(%%r30,%%r28)\n"
#define GPC_ERR "    err!"

#define GPC_END "    mov(%s,%%r30)\n"

#define GPC_IMM "$%s"

#define GPC_USEV 1
#define GPC_USEF 1

struct GPC_VAL {
	char name[256];
	char func[256];
	char asms[256];
	int flag;
	int ptr;
	int sp;
};

struct GPC_FUNC {
	char name[64];
	int asp[64];
	int nsp;
	int flag;
};

struct GPC_CONVTBL {
	char ope;
	char *asms;
};

struct GPC_CONVTBL tbl[] = {
	{'=',GPC_MOV},
	{'+',GPC_ADD},
	{'-',GPC_SUB},
	{'*',GPC_MUL},
	{'/',GPC_DIV},
	{'&',GPC_AND},
	{'|',GPC_OR},
	{'^',GPC_XOR},
	{'<',GPC_SAL},
	{'>',GPC_SAR},
	{'%',GPC_MOD},
	{0,GPC_ERR},
};

struct GPC_CTL {
	char nfunc[256];
	struct GPC_VAL vals[16384];
	struct GPC_FUNC funcs[16384];
	char defstring[128][65536];
	int ifs[512];
	int ifsp;
	int fos[512];
	int fosp;
	int nsp;
	int nsp2;
	int nif;
	int nlop;
	int ndef;
	int vals_addr;
};

struct CHUNK_STACK {
	char string[512][1024];
	int sp;
};