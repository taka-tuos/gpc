#include "gpc.h"

int nst = 0;

struct GPC_CTL *gpc_initctl(struct GPC_CTL *ctl)
{
	int i;
	strcpy(ctl->nfunc,"global");
	ctl->nsp = 0;
	ctl->nsp2 = 0;
	ctl->nlop = 0;
	ctl->nif = 0;
	ctl->ndef = 0;
	ctl->ifsp = 0;
	ctl->fosp = 0;

	for(i = 0; i < 16384; i++) {
		ctl->vals[i].flag = 0;
		ctl->funcs[i].flag = 0;
	}

	return ctl;
}

char *gpc_getcalcst(char *s) {
	for(;*s != '=';*s++);
	return s;
}

char *gpc_getcalcval(char *s) {
	char *ret = (char *)malloc(256);
	int i;
	s++;
	for(i = 0;*s != '=';*s++,i++) {
		ret[i] = *s;
	}
	ret[i] = 0;
	return ret;
}

char *gpc_getcalcvaln(char *s) {
	char *ret = (char *)malloc(256);
	int i;
	s++;
	for(i = 0;strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_",*s) != 0;*s++,i++) {
		ret[i] = *s;
	}
	ret[i] = 0;
	return ret;
}

char *gpc_getop(char *s)
{
	char *str = (char *)malloc(512);

	int i;
	char *p = s;

	for(;*p==' '||*p==0x09;p++);
	for(i = 0;*p!='(';p++,i++) {
		str[i]=*p;
	}

	str[i]=0;

	return str;
}

char *gpc_getargstr(char *s,int num)
{
	char *str = (char *)malloc(512);

	int i;
	char *p = s;

	for(;*p==' '||*p==0x09;p++);
	for(i = 0;*p!='(';p++,i++);

	p++;

	for(i=0;i<num;i++) {
		for(;*p!=','&&*p!=')';p++);
		p++;
	}

	for(i=0;*p!=','&&*p!=')';p++,i++) {
		str[i]=*p;
	}

	str[i]=0;

	return str;
}

char *gpc_getargstr_c(char *s,int num)
{
	char *str = (char *)malloc(512);
	int i;
	char *p = s;

	for(;*p==' '||*p==0x09;p++);
	for(i = 0;*p!='[';p++,i++);

	p++;

	for(i=0;i<num;i++) {
		for(;*p!=' '&&*p!=']';p++);
		p++;
	}

	for(i=0;*p!=' '&&*p!=']';p++,i++) {
		str[i]=*p;
	}

	str[i]=0;

	return str;
}

int gpc_numarg(char *s)
{
	int i = 0;
	if(strstr(s,"()") != 0) return 0;
	for(;*s != 0; s++) if(*s == ',') i++;
	return i+1;
}

char *gpc_getstring(char *typestr, ...)
{
	char *work = (char *)malloc(16384);
	va_list lst;
	va_start(lst, typestr);

	vsprintf(work,typestr,lst);

	va_end(lst);

	return work;
}

int gpc_findval(char *name, char *func, struct GPC_VAL *vals)
{
	int i;

	for(i = 0; i < 16384; i++) {
		if(strcmp(vals[i].name, name) == 0 && (strcmp(vals[i].func, func) == 0 || strcmp(vals[i].func, "global") == 0) && (vals[i].flag & GPC_USEV)) return i;
	}

	printf("error : val \'%s,\' not found", name);
	exit(-1);
	return -1;
}

int gpc_findfunc(char *name, struct GPC_FUNC *funcs)
{
	int i;

	for(i = 0; i < 16384; i++) {
		if(strcmp(funcs[i].name, name) == 0 && (funcs[i].flag & GPC_USEF)) return i;
	}
	printf("error : func \'%s,\' not found", name);
	exit(-1);
	return -1;
}

int gpc_allocval(char *name, char *func, struct GPC_VAL *vals)
{
	int i;

	for(i = 0; i < 16384; i++) {
		if(!(vals[i].flag & GPC_USEV)) {
			strcpy(vals[i].func, func);
			vals[i].flag |= GPC_USEV;
			strcpy(vals[i].name, name);
			return i;
		}
	}

	printf("error : over 16387 vals", name);
	exit(-1);
	return -1;
}

int gpc_allocfunc(char *name, struct GPC_FUNC *funcs)
{
	int i;

	for(i = 0; i < 16384; i++) {
		if(!(funcs[i].flag & GPC_USEF)) {
			funcs[i].flag |= GPC_USEF;
			strcpy(funcs[i].name, name);
			return i;
		}
	}

	printf("error : over 16387 funcs", name);
	exit(-1);
	return -1;
}

char *gpc_getskipspt(char *s)
{
	char *ans = (char *)malloc(256);
	int a, b;

	for(a = 0, b = 0; s[b] != 0;) {
		if(s[b] != ' ' && s[b] != '\t') {
			ans[a] = s[b];
			a++;
		}
		b++;
	}
	ans[a] = 0;

	return ans;
}

char *gpc_getcalcmember(char *s)
{
	char *ans = (char *)malloc(256);
	char *bf = gpc_getskipspt(s);
	int i;

	for(i = 0;strchr("+-*/&|^<>%()",*bf) == 0 && *bf != 0;*bf++) {
		ans[i] = *bf;
		i++;
	}

	ans[i] = 0;

	return ans;
}

char *gpc_modify(char *str)
{
	char s[256];
	strcpy(s,str);
	char *rs = (char *)malloc(256);

	for(;;) {
		char *p;
		char *b;
		int o = 0;
		int e = 0;
		int d = 0;
		int f = 0;
		for(p = s; *p != 0; p++) {
			if(*p == '(' && e == 0) {
				e++;
				b = p;
			} else if (*p == '(' && e != 0) {
				int c = 0;
				int k = 0;
				char x[256];
				strcpy(x,p+1);
				char *z = p+1;
				for(;*p != 0;p++,k++) {
					if(*p == '(') { c++; f++; }
					if(*p == ')') { c--; f--; }
					if(*p == ')' && c == 0) break;
				}
				x[k-1] = 0;
				nst++;
				char *f = gpc_modify(x);
				nst--;
				memcpy(z,f,strlen(x));
			}

			if(strchr("=+-*/&|^<>%",*p) != 0 && e != 0) {
				o++;
			}



			if(*p == ')' && e != 0 && o == 0) {
				*p = ' ';
				*b = ' ';
				e = 0;
				o = 0;
				d++;
			} else if(*p == ')' && e != 0) {
				e = 0;
				o = 0;
			}
		}

		if(d == 0) {
			strcpy(rs,s);
			if(nst == 0) {
				return gpc_getskipspt(rs);
			}
			return rs;
		}
	}
}

char *gpc_convcalcs(char *s)
{
	char *rs = (char *)malloc(512);
	char *p;

	rs[0] = 0;

	strcat(rs,"(((");

	for(p = s; *p != 0; p++) {
			 if(*p == '+') strcat(rs,"))+((");
		else if(*p == '-') strcat(rs,"))-((");
		else if(*p == '/') strcat(rs,")/(");
		else if(*p == '*') strcat(rs,")*(");
		else if(*p == '%') strcat(rs,")))%(((");
		else if(*p == '&') strcat(rs,")))&(((");
		else if(*p == '^') strcat(rs,")))^(((");
		else if(*p == '|') strcat(rs,")))|(((");
		else if(*p == '<') strcat(rs,")))<(((");
		else if(*p == '>') strcat(rs,")))>(((");
		else {
			char a[2];
			a[0] = *p;
			a[1] = 0;
			strcat(rs,a);
		}
	}

	strcat(rs,")))");

	return rs;
}

struct CHUNK_STACK *stack_alloc(void)
{
	return (struct CHUNK_STACK *) malloc(sizeof(struct CHUNK_STACK));
}

void stack_init(struct CHUNK_STACK *stack)
{
	stack->sp = 0;
}

void stack_push(struct CHUNK_STACK *stack, char *chunk)
{
	stack->sp++;
	if(stack->sp == 512) stack->sp = 511;
	memcpy(stack->string[1], stack->string[0], 1024*512-512);
	strcpy(stack->string[0],chunk);
}

char *stack_pop(struct CHUNK_STACK *stack)
{
	char pop[1024];
	strcpy(pop,stack->string[0]);
	if(stack->sp != 0) {
		stack->sp--;
		memcpy(stack->string[0], stack->string[1], 1024*512-512);
	}
	return pop;
}

void stack_free(struct CHUNK_STACK *stack)
{
	free(stack);
}

char *gpc_storeval_s(char *s, char *reg, struct GPC_CTL *ctl, FILE *compile)
{
	char *p = s;
	char *a = (char *)malloc(512);
	a[0] = 0;
	
	if(*p == '.') {
		int n = gpc_findval(s+1,ctl->nfunc,ctl->vals);
		strcat(a,gpc_getstring("    mov(%%r%s,%s)\n",reg,gpc_getstring(ctl->vals[n].asms,ctl->vals[n].sp)));
	} else if(*p == '$') {
		strcat(a,gpc_getstring("    mov(%%r%s,.%s)\n",reg,p+1));
	} else if(*p == '#') {
		strcat(a,gpc_getstring("    mov(%%r%s,$%d)\n",reg,p+1));
	} else if(*p == ':') {
		int n = gpc_findval(s+1,ctl->nfunc,ctl->vals);
		strcat(a,gpc_getstring("    mov(%%r28,%s)\n",gpc_getstring(GPC_DVAL,ctl->vals[n].sp)));
		strcat(a,gpc_getstring("    mov(%%r%s,%s)\n",reg,gpc_getstring(ctl->vals[n].asms)));
	} else if(*p == '@') {
		int n = gpc_findval(s+1,ctl->nfunc,ctl->vals);
		strcat(a,gpc_getstring("    mov(%%r%s,%s)\n",reg,gpc_getstring(GPC_DVAL,ctl->vals[n].sp)));
	} else {
		strcat(a,gpc_getstring("    mov(%%r%s,$%s)\n",reg,p));
	}

	return a;
}

char *gpc_loadval_s(char *s, char *reg, struct GPC_CTL *ctl, FILE *compile)
{
	char *p = s;
	char *a = (char *)malloc(512);
	a[0] = 0;
	
	if(*p == '.') {
		int n = gpc_findval(s+1,ctl->nfunc,ctl->vals);
		strcat(a,gpc_getstring("    mov(%s,%%r%s)\n",gpc_getstring(ctl->vals[n].asms,ctl->vals[n].sp),reg));
	} else if(*p == ':') {
		int n = gpc_findval(s+1,ctl->nfunc,ctl->vals);
		strcat(a,gpc_getstring("    mov(%%r28,%s)\n",gpc_getstring(GPC_DVAL,ctl->vals[n].sp)));
		strcat(a,gpc_getstring("    mov(%s,%%r%s)\n",gpc_getstring(ctl->vals[n].asms),reg));
	} else if(*p == '@') {
		int n = gpc_findval(s+1,ctl->nfunc,ctl->vals);
		strcat(a,gpc_getstring("    mov(%s,%%r%s)\n",gpc_getstring(GPC_DVAL,ctl->vals[n].sp),reg)); 
	} else {
		
	}

	return a;
}

void gpc_storeval(char *s, char *reg, struct GPC_CTL *ctl, FILE *compile)
{
	fputs(gpc_storeval_s(s,reg,ctl,compile),compile);
}

void gpc_loadval(char *s, char *reg, struct GPC_CTL *ctl, FILE *compile)
{
	fputs(gpc_loadval_s(s,reg,ctl,compile),compile);
}

char *gpc_getcalcstring(char *s, struct GPC_VAL *vals, struct GPC_CTL *ctl)
{
	char *ans = (char *)malloc(16384);
	char *work = (char *)malloc(16384);
	char *work2 = (char *)malloc(256);

	ans[0] = 0;

	for(;*s != 0;) {
		int i;
		for(i = 0; tbl[i].ope != 0; i++) {
			if(*s == tbl[i].ope) {
				char *n = gpc_getskipspt(s+1);
				if(n[0] == '.'|| n[0] == ':' || n[0] == '@') {
					char *valn = gpc_getcalcvaln(gpc_getskipspt(s+1));
					int vaind = gpc_findval(valn,ctl->nfunc,vals);
					if(n[0] == '.') {
						strcat(ans,gpc_getstring(tbl[i].asms,gpc_getstring(vals[vaind].asms,vals[vaind].sp)));
					} else if(n[0] == '@') {
						strcat(ans,gpc_getstring(tbl[i].asms,gpc_getstring(GPC_DVAL,vals[vaind].sp)));
					} else if(n[0] == ':') {
						strcat(ans,gpc_getstring("    mov(%%r28,%s)\n",gpc_getstring(vals[vaind].asms,vals[vaind].sp)));
						strcat(ans,gpc_getstring(tbl[i].asms,vals[vaind] .asms));
					}
				} else if(n[0] == '$') {
					char *s3 = gpc_getskipspt(s+2);
					int j;
					char *p = s3;
					for(j = 0; j < 256; j++) {
						if(p[j] == 0x0a || p[j] == 0x0d) {
							p[j] = 0;
							break;
						}
					}
					for(j = 0; j < 256; j++) {
						if(strchr("+-*/&|^<>%",p[j])) {
							p[j] = 0;
							break;
						}
					}
					*p = '.';
					strcat(ans,gpc_getstring(tbl[i].asms,p));
				} else if(n[0] == '(') {
					int j;
					int ns = 1;
					for(j = 1; n[j] != 0; j++) {
						work[j-1] = n[j];
						if(n[j] == '(') ns++;
						if(n[j] == ')') ns--;
						if(n[j] == ')' && ns == 0) break;
					}
					work[j-1] = 0;
					work2[0] = 0;
					strcat(work2,"= ");
					strcat(work2,work);
					strcat(ans,"    push(%r30)\n");
					strcat(ans,"    mov(%r30,$0)\n");
					char *n2 = gpc_getcalcstring(work2,vals,ctl);
					strcat(ans,n2);
					strcat(ans,"    mov(%r29,%r30)\n");
					strcat(ans,"    pop(%r30)\n");
					strcat(ans,gpc_getstring(tbl[i].asms,"%r29"));
					ns = -1;
					for(;*s != 0;*s++) {
						if(*s == ')') ns--;
						if(*s == ')' && ns == -1) break;
						if(*s == '(') ns++;
					}
					//s++;
				} else if(n[0] == '#') {
					strcat(ans,gpc_getstring(tbl[i].asms,gpc_getstring("$%d",n[1])));
				} else {
					int j;
					char *p = s+1;
					for(j = 0; j < 256; j++) {
						if(p[j] == 0x0a || p[j] == 0x0d) {
							p[j] = 0;
							break;
						}
					}
					char *string1 = gpc_getstring(GPC_IMM,gpc_getcalcmember(p));
					char *string2 = gpc_getstring(tbl[i].asms,string1);
					strcat(ans,string2);
					free(string1);
					free(string2);
				}
			}
		}
		s++;
		for(;strchr("+-*/&|^<>%",*s) == 0;*s++);
	}

	return ans;
}

char *gpc_calc2asm(char *s, struct GPC_VAL *vals, struct GPC_CTL *ctl, FILE *compile)
{
	char *work = (char *)malloc(16384);
	char *st = gpc_getcalcst(s);
	char *vname = gpc_getskipspt(gpc_getcalcval(s));
	work[0] = 0;
	strcat(work,"    mov(%r30,$0)\n");
	char *spst = gpc_getskipspt(st);
	spst[0] = ' ';
	char *c = gpc_convcalcs(spst);
	char *cst = gpc_modify(c);
	char wk[256];
	wk[0] = 0;
	strcat(wk,"=");
	strcat(wk,cst);
	strcat(work,gpc_getcalcstring(wk,vals,ctl));
	strcat(work,gpc_loadval_s(gpc_getstring("%c%s",gpc_getskipspt(s)[0],vname),"30",ctl,compile));
	return work;
}

void gpc_allocvalstring(char *s, struct GPC_CTL *ctl)
{
	char *p = s;
	for(;*p != '.';p++);
	p++;
	int spn;

	int n = gpc_allocval(p,ctl->nfunc,ctl->vals);
	if(strncmp(s,"val8",4) == 0) { strcpy(ctl->vals[n].asms,GPC_BVAL); spn = 1; ctl->vals[n].ptr = 0; }
	if(strncmp(s,"val16",5) == 0) { strcpy(ctl->vals[n].asms,GPC_WVAL); spn = 2; ctl->vals[n].ptr = 0; }
	if(strncmp(s,"val32",5) == 0) { strcpy(ctl->vals[n].asms,GPC_DVAL); spn = 4; ctl->vals[n].ptr = 0; }
	if(strncmp(s,"ptr8",4) == 0) { strcpy(ctl->vals[n].asms,GPC_BPTR); spn = 1; ctl->vals[n].ptr = 1; }
	if(strncmp(s,"ptr16",5) == 0) { strcpy(ctl->vals[n].asms,GPC_WPTR); spn = 2; ctl->vals[n].ptr = 1; }
	if(strncmp(s,"ptr32",5) == 0) { strcpy(ctl->vals[n].asms,GPC_DPTR); spn = 4; ctl->vals[n].ptr = 1; }
	ctl->vals[n].sp = ctl->nsp2;
	ctl->nsp2 += spn;
	ctl->nsp++;
}

char *gpc_getifstring(char *p2, struct GPC_CTL *ctl, FILE *compile)
{
	char *p = gpc_getargstr_c(p2,0);
	gpc_storeval(p,"30",ctl,compile);
	p = gpc_getargstr_c(p2,2);
	gpc_storeval(p,"29",ctl,compile);
	fputs("    cmp(%r30,%r29)\n",compile);
	p = gpc_getargstr_c(p2,1);
	char *fop = (char *)malloc(4);
	
	if(strcmp(p,"==") == 0) strcpy(fop,"je");
	if(strcmp(p,"!=") == 0) strcpy(fop,"jne");
	if(strcmp(p,"<=") == 0) strcpy(fop,"jbe");
	if(strcmp(p,">=") == 0) strcpy(fop,"jae");
	if(strcmp(p,"<") == 0) strcpy(fop,"jb");
	if(strcmp(p,">") == 0) strcpy(fop,"ja");
	if(strcmp(p,"=0") == 0) strcpy(fop,"jz");
	if(strcmp(p,"!0") == 0) strcpy(fop,"jnz");

	return fop;
}

void gpc_fetch(char *lin, struct CHUNK_STACK *stack, struct GPC_CTL *ctl, FILE *compile)
{
	char *ln = lin;
	for(;*ln == '\t' || *ln == ' ';ln++);
	if(*ln == '.' || *ln == ':' || *ln == '@') {
		if(strcmp(ctl->nfunc,"global") == 0) {
			puts("cant calc global area");
			exit(-1);
		} else {
			char *s = gpc_calc2asm(ln,ctl->vals,ctl,compile);
			fputs(s,compile);
		}
	}

	if(strncmp(ln,"val8",4) == 0 || strncmp(ln,"val16",5) == 0 || strncmp(ln,"val32",5) == 0 || strncmp(ln,"ptr8",4) == 0 || strncmp(ln,"ptr16",5) == 0 || strncmp(ln,"ptr32",5) == 0) {
		char *p = ln;
		for(;*p != '.';p++);
		p++;
		int i;
		for(i = 0; i < 256; i++) {
			if(p[i] == 0x0a || p[i] == 0x0d) {
				p[i] = 0;
				break;
			}
		}
		gpc_allocvalstring(ln,ctl);
	}

	if(strncmp(ln,"function",8) == 0) {
		char *p = ln + 9;
		char *p2;
		char *p3 = gpc_getop(p);
		int an;
		int i;
		fprintf(compile,".%s\n",p3);
		stack_push(stack,ctl->nfunc);
		strcpy(ctl->nfunc,p3);
		int fn = gpc_allocfunc(p3,ctl->funcs);
		p2 = gpc_getargstr(p,0);
		an = gpc_numarg(p);
		ctl->funcs[fn].nsp = an;
		for(i = 0; i < an; i++) {
			p3 = gpc_getargstr(p,i);
			for(;*p3 != '.';p3++);
			p3++;
			if(gpc_findval(p3,gpc_getop(p),ctl->vals) == -1) {
				gpc_allocvalstring(gpc_getargstr(p,i),ctl);
				ctl->funcs[fn].asp[i] = gpc_findval(p3,ctl->nfunc,ctl->vals);
			} else {
				ctl->funcs[fn].asp[i] = gpc_findval(p3,ctl->nfunc,ctl->vals);
			}
		}
	}

	if(strncmp(ln,"if",2) == 0) {
		char *fop = gpc_getifstring(ln,ctl,compile);

		fprintf(compile,"    %s(.ifstart%d)\n",fop,ctl->nif);
		fprintf(compile,"    jmp(.ifend%d)\n",ctl->nif);
		fprintf(compile,".ifstart%d\n",ctl->nif);
		ctl->ifs[ctl->ifsp++] = ctl->nif;
		ctl->nif++;
	}
	
	if(strncmp(ln,"fi",2) == 0) {
		fprintf(compile,".ifend%d\n",ctl->ifs[--ctl->ifsp]);
	}
		

	if(strncmp(ln,"#define_f",9) == 0) {
		char *p = ln + 10;
		char *p2;
		char *p3 = gpc_getop(p);
		int an;
		int i;
		int fn = gpc_allocfunc(p3,ctl->funcs);
		p2 = gpc_getargstr(p,0);
		an = gpc_numarg(p);
		ctl->funcs[fn].nsp = an;
		for(i = 0; i < an; i++) {
			char bks[256];
			strcpy(bks,ctl->nfunc);
			strcpy(ctl->nfunc,gpc_getop(p));
			gpc_allocvalstring(gpc_getargstr(p,i),ctl);
			strcpy(ctl->nfunc,bks);
			p3 = gpc_getargstr(p,i);
			for(;*p3 != '.';p3++);
			p3++;
			ctl->funcs[fn].asp[i] = gpc_findval(p3,gpc_getop(p),ctl->vals);
		}
	}

	if(strncmp(ln,"endf",4) == 0) {
		fputs("    ret()\n",compile);
		strcpy(ctl->nfunc,stack_pop(stack));
	}

	if(strncmp(ln,"lreg",4) == 0) {
		char *p = gpc_getargstr(ln,1);
		gpc_storeval(p,gpc_getargstr(ln,0),ctl,compile);
	}

	if(strncmp(ln,"sreg",4) == 0) {
		char *p = gpc_getargstr(ln,1);
		gpc_loadval(p,gpc_getargstr(ln,0),ctl,compile);
	}

	if(strncmp(ln,"asm",3) == 0) {
		char *p = ln + 4;
		int i;
		for(i = 0; i < 256; i++) {
			if(p[i] == 0x0a || p[i] == 0x0d) {
				p[i] = 0;
				break;
			}
		}
		fprintf(compile, "    %s\n",p);
	}

	if(strncmp(ln,"#define_s",9) == 0) {
		char *p = ln + 10;
		char ln[256];
		char *dn = (char *)malloc(65536);
		int i;
		for(i = 0; *p != ' '; i++, p++) {
			ln[i] = *p;
		}
		ln[i] = 0;
		p++;
		for(i = 0; *p != 0x0d && *p != 0x0a && *p != 0x0; i++, p++) {
			dn[i] = *p;
		}
		dn[i] = 0;
		sprintf(ctl->defstring[ctl->ndef],".%s\n    text(%s)\n    nop()\n",ln,dn);
		ctl->ndef++;

	}
	if(strncmp(ln,"for",3) == 0) {
		fputs(gpc_calc2asm(gpc_getargstr(ln,0),ctl->vals,ctl,compile),compile);
		fprintf(compile,"    jmp(.lopstart%d)\n",ctl->nlop);
		fprintf(compile,".lopcall%d\n",ctl->nlop);
		fputs(gpc_calc2asm(gpc_getargstr(ln,2),ctl->vals,ctl,compile),compile);

		char *p2 = gpc_getargstr(ln,1);

		char *fop = gpc_getifstring(p2,ctl,compile);

		fprintf(compile,"    %s(.lopret%d)\n",fop,ctl->nlop);
		fprintf(compile,"    jmp(.lopend%d)\n",ctl->nlop);
		fprintf(compile,".lopstart%d\n",ctl->nlop);
		ctl->fos[ctl->fosp++] = ctl->nlop;
		ctl->nlop++;
	}

	if(strncmp(ln,"next",4) == 0) {
		int lopn = ctl->fos[--ctl->fosp];
		fprintf(compile,"    jmp(.lopcall%d)\n",lopn);
		fprintf(compile,".lopret%d\n",lopn);
		fprintf(compile,"    jmp(.lopstart%d)\n",lopn);
		fprintf(compile,".lopend%d\n",lopn);
	}

	if(ln[0] == '$') {
		char *p = ln + 1;
		int fuind = gpc_findfunc(gpc_getop(p),ctl->funcs);
		int i;
		for(i = 0; i < ctl->funcs[fuind].nsp; i++) {
			gpc_storeval(gpc_getargstr(p,i),"30",ctl,compile);
			fprintf(compile, "    mov(%s,%%r30)\n",gpc_getstring(ctl->vals[ctl->funcs[fuind].asp[i]].asms,ctl->vals[ctl->funcs[fuind].asp[i]].sp));
		}
		fprintf(compile,"    call(.%s)\n",ctl->funcs[fuind].name);
	}

}

//------------------------------------------------------

int main(int argc, char *argv[])
{
	puts("GP32v3(mipv1,gp3200+,gp3200+x2) C like compiler");
		puts("2009-2015 wsg project");

	if(argc < 5) {
		puts("usage>gpc <sourcefile> <outfile> <orgaddr> <stackaddr>");
	}

	FILE *source = fopen(argv[1],"rt");
	FILE *compile = fopen(argv[2],"wt");

	if(source == NULL) {
		puts("cant open source file");
		exit(2);
	}

	if(compile == NULL) {
		puts("cant open compile file");
		exit(3);
	}

	struct GPC_CTL *ctl = gpc_initctl((struct GPC_CTL *)malloc(sizeof(struct GPC_CTL)));
	struct CHUNK_STACK *stack = stack_alloc();
	stack_init(stack);

	ctl->vals_addr = strtol(argv[4],&argv[4],0);

	fprintf(compile,"    org($0x%08x)\n", strtol(argv[3],&argv[3],0));
	fprintf(compile,"    mov(%%r31,$0x%08x)\n", ctl->vals_addr);
	fputs("    call(.entry)\n",compile);
	fputs(".fin\n",compile);
	fputs("    jmp(.fin)\n",compile);
	stack_push(stack,"global");

	char lin[1024];

	do {
		fgets(lin,1024,source);
		gpc_fetch(lin,stack,ctl,compile);
	} while(!feof(source));

	int i;
	for(i = 0; i < ctl->ndef; i++) {
		fputs(ctl->defstring[i],compile);
	}

	fclose(source);
	fclose(compile);

	return 0;
}
