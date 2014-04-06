/*
        CB.C

        Modified for Lattice C Ver 1.01
        by: John W. Kindschi Jr. (10-30-83)

        Swiped from CPIG'S UNIX system and modified to
        run under BDS C by William C. Colley, III

        Further modified by Bert Kleinman 9/30/86:

          File extension defaults to .c
          Indent defaults to 3
          Uses temp file cb.$$$, then renames input to .BAK
          Converstion to MSC 4.0

        Usage: cb filname [indent]

*/

#include <stdio.h>
#include <string.h>

#define INDENT_SPACES 4 	 /* default number of spaces to indent */

int slevel[10];
int clevel;
int spflg[20][10];
int sind[20][10];
int siflev[10];
int sifflg[10];
int iflev;
int ifflg;
int level;
int eflg;
int paren;
int aflg;
int ct;
int stabs[20][10];
int qflg;
int j;
int sflg;
int bflg;
int peek;
int tabs;
FILE *f1, *f2;

static int ind[10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static int pflg[10] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char lchar;
char pchar;
char *wif[2];
char *welse[2];
char *wfor[2];
char *wds[3];
char cc;
char string[200];
int lastchar;
int c;
int outvect;
char infile[75];
char bakfile[75];
int indent;

main(argc,argv)
int argc;
char *argv[];
{

    char tempfile[15];
    int result;
    char *p;

    int k;        /* Initialize everything here */

    outvect = 1;

    if (argc < 2 || argc > 3)
    {
        print("\nUsage: cb infile [indent]\n");
        exit(1);
    }


    /* set indent - either from cmd line or default */

    if( argc != 3 )
    {
        indent = INDENT_SPACES;
    }
    else
    {
        indent = atoi( argv[2] );
        if( indent > 10 || indent < 0)      /* if invalid indent */
        {
            print("Error: Valid indents are 0 - 10\n");
            exit(1);
        }
    }

    /* add default extension of .c if necessary */

    strcpy( infile, argv[1] );
    if( strchr( infile, '.' ) == NULL )
        strcat(infile, ".c");

    /* open input file */

    if( (f1 = fopen(infile,"r")) == NULL )
    {
        print("Error: input file can't be opened\n");
        exit(1);
    }

    /* open temp file */

    strcpy(tempfile,"cb.$$$");

    if( (f2 = fopen(tempfile,"w")) == NULL )
    {
        print("Error: temporary file can't be opened\n");
        exit(1);
    }

    outvect = 0;

    clevel = iflev = level = eflg = paren = 0;
    aflg = qflg = j = bflg = tabs = 0;
    ifflg = peek = -1;
    sflg = 1;
    wif[0] = "if";
    welse[0] = "else";
    wfor[0] = "for";
    wds[0] = "case";
    wds[1] = "default";
    wif[1] = welse[1] = wfor[1] = wds[2] = 0;

    /* End of Initialization */

    while ((c = getchr()) != EOF){
        switch(c){
        default:
            string[j++] = c;
            if (c != ',') lchar = c;
            break;
        case ' ':
        case '\t':
            if (lookup (welse) == 1){
                gotelse();
                if(sflg == 0 || j > 0)string[j++] = c;
                puts();
                sflg = 0;
                break;
            }
            if(sflg == 0 || j > 0) string[j++] = c;
            break;
        case '\n':
            if (eflg = lookup(welse) == 1) gotelse();
            puts();
            print ("\n");
            sflg = 1;
            if (eflg == 1){
                pflg[level]++;
                tabs++;
            }
            else
                if(pchar == lchar)
                    aflg = 1;
            break;
        case '{':
            if (lookup(welse) == 1) gotelse();
            siflev[clevel] = iflev;
            sifflg[clevel] = ifflg;
            iflev = ifflg = 0;
            clevel++;
            if (sflg == 1 && pflg[level] != 0){
                pflg[level]--;
                tabs--;
            }
            string[j++] = c;
            puts();
            getnl();
            puts();
            print("\n");
            tabs++;
            sflg = 1;
            if (pflg[level] > 0){
                ind[level] = 1;
                level++;
                slevel[level] = clevel;
            }
            break;
        case '}':
            clevel--;
            if ((iflev = siflev[clevel]-1) < 0) iflev = 0;
            ifflg = sifflg[clevel];
            puts();
            tabs--;
            ptabs();
            if ((peek = getchr()) == ';'){
                print("%c;",c);
                peek = -1;
            }
            else print("%c",c);
            getnl();
            puts();
            print("\n");
            sflg = 1;
            if (clevel < slevel[level]) if (level > 0) level--;
            if (ind[level] != 0){
                tabs -= pflg[level];
                pflg[level] = 0;
                ind[level] = 0;
            }
            break;
        case '"':
        case '\'':
            string[j++] = c;
            while((cc = getchr()) != c){
                string[j++] = cc;
                if(cc == '\\'){
                    string[j++] = getchr();
                }
                if(cc == '\n'){
                    puts();
                    sflg = 1;
                }
            }
            string[j++] = cc;
            if(getnl() == 1){
                lchar = cc;
                peek = '\n';
            }
            break;
        case ';':
            string[j++] = c;
            puts();
            if(pflg[level] > 0 && ind[level] == 0){
                tabs -= pflg[level];
                pflg[level] = 0;
            }
            getnl();
            puts();
            print("\n");
            sflg = 1;
            if(iflev > 0)
                if(ifflg == 1){
                    iflev--;
                    ifflg = 0;
                }
                else iflev = 0;
            break;
        case '\\':
            string[j++] = c;
            string[j++] = getchr();
            break;
        case '?':
            qflg = 1;
            string[j++] = c;
            break;
        case ':':
            string[j++] = c;
            if(qflg == 1){
                qflg = 0;
                break;
            }
            if(lookup(wds) == 0){
                sflg = 0;
                puts();
            }
            else{
                tabs--;
                puts();
                tabs++;
            }
            if((peek = getchr()) == ';'){
                print(";");
                peek = -1;
            }
            getnl();
            puts();
            print("\n");
            sflg = 1;
            break;
        case '/':
            string[j++] = c;
            if((peek = getchr()) != '*') break;
            string[j++] = peek;
            peek = -1;
            comment();
            break;
        case ')':
            paren--;
            string[j++] = c;
            puts();
            if(getnl() == 1){
                peek = '\n';
                if(paren != 0) aflg = 1;
                else if(tabs > 0){
                    pflg[level]++;
                    tabs++;
                    ind[level] = 0;
                }
            }
            break;
        case '#':
            string[j++] = c;
            while((cc = getchr()) != '\n') string[j++] = cc;
            string[j++] = cc;
            sflg = 0;
            puts();
            sflg = 1;
            break;
        case '(':
            string[j++] = c;
            paren++;
            if(lookup(wfor) == 1){
                while((c = getsc()) != ';');
                ct = 0;
cont:
                while((c = getsc()) != ')'){
                    if(c == '(') ct++;
                }
                if(ct != 0){
                    ct--;
                    goto cont;
                }
                paren--;
                puts();
                if(getnl() == 1){
                    peek = '\n';
                    pflg[level]++;
                    tabs++;
                    ind[level] = 0;
                }
                break;
            }
            if(lookup(wif) == 1){
                puts();
                stabs[clevel][iflev] = tabs;
                spflg[clevel][iflev] = pflg[level];
                sind[clevel][iflev] = ind[level];
                iflev++;
                ifflg = 1;
            }
        }
    }

    result = fcloseall();

    /* construct bakfile name */

    strcpy(bakfile, infile);
    p = strchr(bakfile, '.');
    *p = '\0';
    strcat(bakfile, ".BAK");

    /* erase bakfile if it exists */

    if( access(bakfile, 0) == 0)
    {
        result = unlink( bakfile );
        if(result != 0 )
        {
            print("Error: can erase old .BAK file\n");
            printf("Results in %s\n",tempfile);
            exit(1);
        }
    }

    /* rename input file to .BAK */

    result = rename(infile, bakfile);

    if( result != 0 )
    {
        printf("Error: can't rename infile to .BAK\n");
        printf("Results in %s\n",tempfile);
        exit(1);
    }

    result = rename(tempfile, infile);

    if( result != 0 )
    {
        printf("Error: can't rename tempefile to infile\n");
        printf("Results in %s\n",tempfile);
        exit(1);
    }


}


ptabs(){
    register int i;
    register int j;

    for (i=0; i < tabs; i++)
        for( j = 0; j < indent; j++ )
            putc(' ', f2 );

}
getchr(){
    if(peek<0 && lastchar != ' ' && lastchar != '\t')pchar = lastchar;
    lastchar = (peek < 0) ? getc(f1) : peek;
    peek = -1;
    return(lastchar == '\r' ? getchr() : lastchar);
}
puts(){
    register int i;

    if (j > 0)
    {
        if (sflg != 0)
        {
            ptabs();
            sflg = 0;
            if (aflg == 1)
            {
                aflg = 0;
                /* commented out to */
                /* prevent indentation of brace after else */
                /* if (tabs > 0) print ("    "); */
            }
        }
        string[j] = '\0';
        print("%s",string);
        j = 0;
    }
    else
    {
        if (sflg != 0)
        {
            sflg = 0;
            aflg = 0;
        }
    }
}
lookup(tab)
char *tab[];
{
    char r;
    register int i,k;
    int kk,l;

    if(j < 1) return(0);
    kk = 0;
    while (string[kk] == ' ') kk++;
    for (i = 0; tab[i] != 0; i++)
    {
        l = 0;
        for(k=kk;(r = tab[i][l++]) == string[k] && r != '\0';k++);
        if(r == '\0' && (string[k] < 'a' || string[k] > 'z'))return(1);
    }
    return(0);
}
getsc(){
    register char ch;
beg:
    if((ch = string[j++] = getchr()) == '\\')
    {
        string[j++] = getchr();
        goto beg;
    }
    if(ch == '\'' || ch == '"')
    {
        while((cc = string[j++] = getchr()) != ch) if(cc == '\\') string[j++] = getchr();
        goto beg;
    }
    if(ch == '\n'){
        puts();
        aflg = 1;
        goto beg;
    }
    else return(ch);
}
gotelse(){
    tabs = stabs[clevel][iflev];
    pflg[level] = spflg[clevel][iflev];
    ind[level] = sind[clevel][iflev];
    ifflg = 1;
}
getnl(){
    while ((peek = getchr()) == '\t' || peek == ' '){
        string[j++] = peek;
        peek = -1;
    }
    if((peek = getchr()) == '/'){
        peek = -1;
        if ((peek = getchr()) == '*'){
            string[j++] = '/';
            string[j++] = '*';
            peek = -1;
            comment();
        }
        else string[j++] = '/';
    }
    if((peek = getchr()) == '\n'){
        peek = -1;
        return(1);
    }
    return(0);
}
comment(){
rep:
    while ((c = string[j++] = getchr()) != '*')
        if (c == '\n'){
            puts();
            sflg = 1;
        }
gotstar:
    if ((c = string[j++] = getchr()) != '/'){
        if (c == '*') goto gotstar;
        goto rep;
    }
}
print(fmt,args)
char *fmt;
unsigned args;
{
    if (outvect == 1) printf(fmt,args);
    else if (outvect == 0) fprintf(f2,fmt,args);
}

_setenvp()

{
}

