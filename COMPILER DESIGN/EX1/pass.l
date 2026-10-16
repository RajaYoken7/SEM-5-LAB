%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>
%}

%%
[^ \t\n]+ {
    int len = strlen(yytext);
    int i;
    int up = 0, low = 0, dig = 0, spec = 0, nspec = 1, flag = 0;

    // 1. Check if it starts with an alphabet
    if (!isalpha((unsigned char)yytext[0])) {
        printf("Invalid password: Must start with an alphabet\n");
        flag = 1;
    }

    // 2. Length check (8 to 64 characters)
    if (len < 8) {
        printf("Invalid password: Length less than 8\n");
        flag = 1;
    }
    if (len > 64) {
        printf("Invalid password: Length should not exceed 64\n");
        flag = 1;
    }

    // 3. Scan characters for groups and forbidden special characters
    for (i = 0; i < len; i++) {
        if (isupper((unsigned char)yytext[i])) {
            up = 1;
        } else if (islower((unsigned char)yytext[i])) {
            low = 1;
        } else if (isdigit((unsigned char)yytext[i])) {
            dig = 1;
        } else if (yytext[i] == '$' || yytext[i] == '@' || yytext[i] == '&' || yytext[i] == '-') {
            spec = 1;
        } else {
            // Any other character (like #, !, %, etc.) is invalid
            nspec = 0;
        }
    }

    // 4. Validate findings if basic length/start checks passed
    if (flag == 0) {
        if (!up) {
            printf("Invalid password: No uppercase letter\n");
            flag = 1;
        }
        if (!low) {
            printf("Invalid password: No lowercase letter\n");
            flag = 1;
        }
        if (!dig) {
            printf("Invalid password: No Digit\n");
            flag = 1;
        }
        if (!spec) {
            printf("Invalid password: No Special letter (@$&-)\n");
            flag = 1;
        }
        if (nspec == 0) {
            printf("Invalid password: Contains forbidden special characters\n");
            flag = 1;
        }
        
        // Final Output
        if (flag == 0) {
            printf("Valid Password\n");
        }
    }
    return 0;
}

\n      { /* Catch and ignore newline characters safely */ }
.       { /* Catch any other single stray character to prevent rule clashing and warnings */ }
%%

int main() {
    printf("Enter Password: ");
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}
