#include<cstdio>
#include<cstring>
#include<cstdlib>
using namespace std;

/*
key
(0:机内指令)(1:标识符)(2:整数)(3:if)(4:else)
(5:while)(6:+)(7:-)(8:*)(9:/)
(10:<)(11:<=)(12:>)(13:>=)(14:==)
(15:!=)(16:&)(17:|)(18::=)(19:()
(20:))(21:{)(22:})(23:;)(24:)
*/

/*
instruction
#行结束

*/

/*
rule
G[A]:
A->I:=E|while(B)Z|if(B)Z (else Z|ε)
E->T{(+|-)T}
T->F{(*|/)F}
F->I|num|(E)
I->string
B->V{|V}
V->W{&W}
W->H{(==|!=|>|>=|<|<=)H}
H->I|num|(B)
Z->{Y}|A
Y->{A}

*/

struct type{//单词的机内表示
	int c;
	char val[128];
};

type get_type(type*temp_type){
	type my_type;
	my_type.c=temp_type->c;
	strcpy(my_type.val,temp_type->val);
	return my_type;
}

type get_type(int cc,char*str){
	type my_type;
	my_type.c=cc;
	strcpy(my_type.val,str);
	return my_type;
}

struct node{//语法分析树结点
	type value,p_val;
	node*left,*right;
	int ad[2],tf[2],ist;
	node(){
		value=p_val=get_type(0,"");
		left=right=0;
		tf[0]=tf[1]=ad[0]=ad[1]=ist=0;
	}
	void rename(type vvalue){
		value=vvalue;
		return;
	}
};

node*get_node(type vvalue=get_type(0,"")){
	node*temp_node=(node*)malloc(sizeof(node));
	temp_node->value=vvalue;
	temp_node->left=temp_node->right=0;
	temp_node->p_val=get_type(0,"");
	temp_node->tf[0]=temp_node->tf[1]=temp_node->ad[0]=temp_node->ad[1]=temp_node->ist=0;
	return temp_node;
}

char reserved_word_list[][15]=//保留字表
{"","","","if","else",
"while","+","-","*","/",
"<","<=",">",">=","==",
"!=","&","|",":=","(",
")","{","}",";"};

char now_line[512];//当前行
char now_word[128];//当前单词
char next_word[128];//下一单词

int length_of_line;//当前行长度
int length_of_word;//当前词长度
int now_line_index;//当前读取句子位置
int next_word_index;//当前读取词结尾位置
int temp_counter;//临时变量计数器
int line_counter;//行数计数器

type end_type=get_type(0,"#");//结束
type now_word_rep=get_type(0,"#");//当前单词的机内表示
type next_word_rep=get_type(0,"#");//下一单词的机内表示

node*tree_end;//语法分析树头指针

void error(char*str=""){//输出错误信息
	printf("error:%s is error\n",str);
	exit(0);
}

//词法分析部分
void get_next_line(){//获得下一行
	gets(now_line);
	next_word_index=0;
	length_of_line=strlen(now_line);
	return;
}

void skip(){//跳过空白
	if(now_line[now_line_index]==0)
		get_next_line();
	now_line_index=next_word_index;
	while(now_line[now_line_index]==' '
		||now_line[now_line_index]=='\n'
		||now_line[now_line_index]=='\t')
		++now_line_index;
	next_word_index=now_line_index;
	return;
}

void get_next_word(){//获得下一单词
	skip();
	while(now_line[next_word_index]>='0'&&now_line[next_word_index]<='9'
		||now_line[next_word_index]>='a'&&now_line[next_word_index]<='z'
		||now_line[next_word_index]>='A'&&now_line[next_word_index]<='Z')
		++next_word_index;
	if(next_word_index==now_line_index){
		++next_word_index;
		if(now_line[now_line_index]=='<'
			||now_line[now_line_index]=='>'
			||now_line[now_line_index]=='='
			||now_line[now_line_index]=='!'
			||now_line[now_line_index]==':')
			if(now_line[next_word_index]=='=')
				++next_word_index;
	}
	strncpy(next_word,now_line+now_line_index,next_word_index-now_line_index);
	next_word[next_word_index-now_line_index]=0;
	return;
}

void get_next_rep(){//获得下一机内表示
	now_word_rep=next_word_rep;
	strcpy(now_word,next_word);
	get_next_word();
	for(int i=3;i<=23;++i)
		if(strcmp(next_word,reserved_word_list[i])==0){
			next_word_rep=get_type(i,next_word);
			return;
		}
	if(next_word[0]==0)
		next_word_rep=get_type(0,"#");
	else if(next_word[0]>='0'&&next_word[0]<='9')
		next_word_rep=get_type(2,next_word);
	else
		next_word_rep=get_type(1,next_word);
	return;
}

void safe_get(){//使now_word_rep跳过行结束符获得下一机内表示
	get_next_rep();
	if(now_word_rep.val[0]=='#')
		get_next_rep();
	return;
}

//语法分析部分
node*A();//A->I:=E|while(B)Z|if(B)Z (else Z|ε)
node*E();//E->T{(+|-)T}
node*T();//T->F{(*|/)F}
node*F();//F->I|(E)
node*B();//B->V{|V}
node*V();//V->W{&W}
node*W();//W->H{(==|!=|>|>=|<|<=)H}
node*H();//H->I|num|(B)
node*Z();//Z->{Y}|A
node*Y();//Y->{A;}

node*I(){
	node*now_node;
	switch(now_word_rep.c){
	case 1:
		now_node=get_node(now_word_rep);
		break;
	default:
		error();
	}
	return now_node;
}

node*num(){
	node*now_node;
	switch(now_word_rep.c){
	case 2:
		now_node=get_node(now_word_rep);
		break;
	default:
		error();
	}
	return now_node;
}

node*F(){//F->I|num|(E)
	node*now_node;
	switch(now_word_rep.c){
	case 1:
		now_node=I();
		break;
	case 2:
		now_node=num();
		break;
	case 19:
		safe_get();
		now_node=E();
		safe_get();
		break;
	default:
		error("illegal identifier/syntax");
	}
	return now_node;
}

node*TO(node*front_node){//{(*|/)F}
	node*now_node;
	switch(next_word_rep.c){
	case 8:
	case 9:
		safe_get();
		now_node=get_node(now_word_rep);
		now_node->left=front_node;
		safe_get();
		now_node->right=F();
		break;
	default:
		return front_node;
	}
	now_node=TO(now_node);
	return now_node;
}

node*T(){//T->F{(*|/)F}
	node*now_node=F();
	now_node=TO(now_node);
	return now_node;
}

node*EO(node*front_node){//{(+|-)T}
	node*now_node;
	switch(next_word_rep.c){
	case 6:
	case 7:
		safe_get();
		now_node=get_node(now_word_rep);
		now_node->left=front_node;
		safe_get();
		now_node->right=T();
		break;
	default:
		return front_node;
	}
	now_node=EO(now_node);
	return now_node;
}

node*E(){//E->T{(+|-)T}
	node*now_node=T();
	now_node=EO(now_node);
	return now_node;
}

node*Y(){//Y->{A}
	node*now_node;
	switch(now_word_rep.c){
	case 1:
	case 3:
	case 5:
		now_node=A();
		break;
	case 22:
		return 0;
	default:
		error("format");
	}
	safe_get();
	now_node->right=Y();
	return now_node;
}

node*Z(){//Z->{Y}|A
	node*now_node;
	switch(now_word_rep.c){
	case 1:
	case 3:
	case 5:
		now_node=A();
		break;
	case 21:
		safe_get();
		now_node=Y();
		break;
	default:
		error("format");
	}
	return now_node;
}

node*H(){//H->I|num|(B)
	node*now_node;
	switch(now_word_rep.c){
	case 1:
		now_node=I();
		break;
	case 2:
		now_node=num();
		break;
	case 19:
		safe_get();
		now_node=B();
		safe_get();
		if(now_word_rep.c!=20)
			error("parenthesis matching");
		break;
	default:
		error("format");
	}
	return now_node;
}

node*WO(node*front_node){//{(==|!=|>|>=|<|<=)H}
	node*now_node;
	switch(next_word_rep.c){
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		safe_get();
		now_node=get_node(now_word_rep);
		now_node->left=front_node;
		safe_get();
		now_node->right=H();
		break;
	default:
		return front_node;
	}
	now_node=WO(now_node);
	return now_node;
}

node*W(){//W->H{(==|!=|>|>=|<|<=)H}
	node*now_node=H();
	now_node=WO(now_node);
	return now_node;
}

node*VO(node*front_node){//{&W}
	node*now_node;
	switch(next_word_rep.c){
	case 16:
		safe_get();
		now_node=get_node(now_word_rep);
		now_node->left=front_node;
		safe_get();
		now_node->right=W();
		break;
	default:
		return front_node;
	}
	now_node=VO(now_node);
	return now_node;
}

node*V(){//V->W{&W}
	node*now_node=W();
	now_node=VO(now_node);
	return now_node;
}

node*BO(node*front_node){//{|V}
	node*now_node;
	switch(next_word_rep.c){
	case 17:
		safe_get();
		now_node=get_node(now_word_rep);
		now_node->left=front_node;
		safe_get();
		now_node->right=V();
		break;
	default:
		return front_node;
	}
	now_node=BO(now_node);
	return now_node;
}

node*B(){//B->V{|V}
	node*now_node=V();
	now_node=BO(now_node);
	return now_node;
}

node*A(){//A->I:=E|while(B)Z|if(B)Z (else Z|ε)
	node*now_node,*temp_node;
	switch(now_word_rep.c){
	case 1:
		now_node=get_node();
		now_node->left=I();
		safe_get();
		if(now_word_rep.c!=18)
			error("illegal symbol");
		now_node->rename(now_word_rep);
		safe_get();
		now_node->right=E();
		temp_node=get_node(get_type(23,";"));
		temp_node->left=now_node;
		now_node=temp_node;
		break;
	case 3:
		now_node=get_node(now_word_rep);
		safe_get();
		safe_get();
		now_node->left=B();
		safe_get();
		safe_get();
		now_node->right=Z();
		if(next_word_rep.c==4){
			safe_get();
			temp_node=now_node->right;
			now_node->right=get_node(now_word_rep);
			now_node->right->left=temp_node;
			safe_get();
			now_node->right->right=Z();
		}
		temp_node=get_node(get_type(23,";"));
		temp_node->left=now_node;
		now_node=temp_node;
		break;
	case 5:
		now_node=get_node(now_word_rep);
		safe_get();
		safe_get();
		now_node->left=B();
		safe_get();
		safe_get();
		now_node->right=Z();
		temp_node=get_node(get_type(23,";"));
		temp_node->left=now_node;
		now_node=temp_node;
		break;
	default:
		error("expression");
	}
	return now_node;
}

void update_backfill(node*now_node,int tf,node*front,int f_tf){//连接两条回填链
	if(now_node->ad[tf]!=0){
		update_backfill((node*)now_node->ad[tf],now_node->tf[tf],front,f_tf);
		return;
	}
	now_node->ad[tf]=(int)front;
	now_node->tf[tf]=f_tf;
	return;
}

void backfill(node*now_node,int value,int tf){//回填
	if(now_node){
		backfill((node*)now_node->ad[tf],value,now_node->tf[tf]);
		now_node->ad[tf]=value;
	}
	return;
}

void parse_tree(node*now_node,bool ist=0){//解析语法分析树
	if(now_node==0)
		return;
	switch(now_node->value.c){
	case 0:

		break;
	case 1:
		now_node->p_val=now_node->value;
		break;
	case 2:
		now_node->p_val=now_node->value;
		break;
	case 3:
		parse_tree(now_node->left);
		backfill(now_node->left,line_counter+1,0);
		parse_tree(now_node->right);
		if(now_node->right->value.c==4)
			backfill(now_node->left,now_node->right->ad[1],1);
		else
			backfill(now_node->left,line_counter+1,1);
		break;
	case 4:
		parse_tree(now_node->left);
		++line_counter;
		now_node->ad[1]=line_counter+1;
		parse_tree(now_node->right);
		now_node->ad[0]=line_counter+1;
		break;
	case 5:
		now_node->ad[0]=line_counter+1;
		parse_tree(now_node->left);
		backfill(now_node->left,line_counter+1,0);
		parse_tree(now_node->right);
		++line_counter;
		backfill(now_node->left,line_counter+1,1);
		break;
	case 6:
	case 7:
	case 8:
	case 9:
		parse_tree(now_node->left);
		parse_tree(now_node->right);
		++line_counter;
		now_node->p_val.c=1;
		sprintf(now_node->p_val.val,"T%d",++temp_counter);
		break;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		if(ist==0){
			if(now_node->left->value.c!=1
				&&now_node->left->value.c!=2
				||now_node->right->value.c!=1
				&&now_node->right->value.c!=2){
				parse_tree(now_node->left,1);
				parse_tree(now_node->right,1);
			}
			else{
				parse_tree(now_node->left);
				parse_tree(now_node->right);
			}
			line_counter+=2;
		}
		else{
			parse_tree(now_node->left,1);
			parse_tree(now_node->right,1);
			now_node->ist=1;
			++line_counter;
			sprintf(now_node->p_val.val,"T%d",++temp_counter);
		}
		break;
	case 16:
		if(ist==0){
			parse_tree(now_node->left);
			backfill(now_node->left,line_counter+1,0);
			parse_tree(now_node->right);
			update_backfill(now_node->right,1,now_node->left,1);
			now_node->ad[0]=now_node->ad[1]=(int)now_node->right;
			now_node->tf[0]=0;
			now_node->tf[1]=1;
			strcpy(now_node->p_val.val,now_node->value.val);
		}
		else{
			parse_tree(now_node->left,1);
			parse_tree(now_node->right,1);
			now_node->ist=1;
			++line_counter;
			sprintf(now_node->p_val.val,"T%d",++temp_counter);
		}
		break;
	case 17:
		if(ist==0){
			parse_tree(now_node->left);
			backfill(now_node->left,line_counter+1,1);
			parse_tree(now_node->right);
			update_backfill(now_node->right,0,now_node->left,0);
			now_node->ad[0]=now_node->ad[1]=(int)now_node->right;
			now_node->tf[0]=0;
			now_node->tf[1]=1;
			strcpy(now_node->p_val.val,now_node->value.val);
		}
		else{
			parse_tree(now_node->left,1);
			parse_tree(now_node->right,1);
			now_node->ist=1;
			++line_counter;
			sprintf(now_node->p_val.val,"T%d",++temp_counter);
		}
		break;
	case 18:
		parse_tree(now_node->left);
		parse_tree(now_node->right);
		++line_counter;
		break;
	case 21:

		break;
	case 22:

		break;
	case 23:
		parse_tree(now_node->left);
		parse_tree(now_node->right);
		break;
	default:
		error("output parsing");
	}
	return;
}

void print(node*now_node){//根据语法分析树输出四元式
	if(now_node==0)
		return;
	switch(now_node->value.c){
	case 0:

		break;
	case 1:

		break;
	case 2:

		break;
	case 3:
		print(now_node->left);
		print(now_node->right);

		break;
	case 4:
		print(now_node->left);
		printf("%03d(J,-,-,%d)\n",
			++line_counter,
			now_node->ad[0]
			);
		print(now_node->right);

		break;
	case 5:
		print(now_node->left);
		print(now_node->right);
		printf("%03d(J,-,-,%d)\n",
			++line_counter,
			now_node->ad[0]
			);
		break;
	case 6:
	case 7:
	case 8:
	case 9:
		print(now_node->left);
		print(now_node->right);
		printf("%03d(%s,%s,%s,%s)\n",
			++line_counter,
			now_node->value.val,
			now_node->left->p_val.val,
			now_node->right->p_val.val,
			now_node->p_val.val);
		break;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		print(now_node->left);
		print(now_node->right);
		if(now_node->ist==0){
				printf("%03d(J%s,%s,%s,%d)\n",
					++line_counter,
					now_node->value.val,
					now_node->left->p_val.val,
					now_node->right->p_val.val,
					now_node->ad[0]
					);
				printf("%03d(J,-,-,%d)\n",
					++line_counter,
					now_node->ad[1]
				);
			}
		else
			printf("%03d(%s,%s,%s,%s)\n",
				++line_counter,
				now_node->value.val,
				now_node->left->p_val.val,
				now_node->right->p_val.val,
				now_node->p_val.val
				);
		break;
	case 16:
		print(now_node->left);
		print(now_node->right);
		if(now_node->ist)
			printf("%03d(&,%s,%s,%s)\n",
				++line_counter,
				now_node->left->p_val.val,
				now_node->right->p_val.val,
				now_node->p_val.val
				);
		break;
	case 17:
		print(now_node->left);
		print(now_node->right);
		if(now_node->ist)
			printf("%03d(|,%s,%s,%s)\n",
				++line_counter,
				now_node->left->p_val.val,
				now_node->right->p_val.val,
				now_node->p_val.val
			);
		break;
	case 18:
		print(now_node->left);
		print(now_node->right);
		printf("%03d(:=,%s,-,%s)\n",
			++line_counter,
			now_node->right->p_val.val,
			now_node->left->p_val.val
			);
		break;
	case 21:

		break;
	case 22:

		break;
	case 23:
		print(now_node->left);
		print(now_node->right);
		break;
	default:
		error("output");
	}
	return;
}

int main(){
	safe_get();
	tree_end=A();
	parse_tree(tree_end);
	line_counter=0;
	print(tree_end);
	return 0;
}