%{
#include<stdio.h>
#include<string.h>
int max = 0;
char longest[100][100];
int count = 0;
%}
%%
[a-zA-Z][a-zA-Z]* {
    int len = strlen(yytext);
    if(len > max){
        max = len;
        count = 0;
        strcpy(longest[count],yytext);
        count++;
    }
    else if(len == max){
        strcpy(longest[count],yytext);
        count++;
    }
}
[\t\n]+;
.;
%%
int yywrap(){
    return 1;
}
int main(){
    yyin = fopen("input1.txt","r");
    if(yyin == NULL){
        printf("Cannot Open file");
        return 1;
    }
    yylex();
    printf("Longest String Length: %d\n",max);
    printf("Longest String:\n");
    int i = 0;
    for(i = 0; i<count; i++){
        printf("%s\n",longest[i]);
    }
    fclose(yyin);
    return 0;
}
