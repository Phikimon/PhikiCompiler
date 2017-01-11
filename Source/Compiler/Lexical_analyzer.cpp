#include "./Lexical_analyzer.hpp"

TokenListNode* tokenRetCtor(size_t line, LangKeywordType type, double data, TokenListNode* prev, TokenListNode* next, char* idName)
{
    TokenListNode* newNode = (TokenListNode*) calloc(1, sizeof(*newNode));
    
    newNode->line = line;
    newNode->type = type;
    newNode->data = data;
    newNode->next = next;
    newNode->prev = prev;
    newNode->idName = idName;
    
    return newNode;
}

void tokenDtor(TokenListNode* node)
{
    assert(node);
    
    node->data = (unsigned)POISON_INT;
    node->type = LANGKWT_nil;
    node->next = node->prev = NULL;
    if (node->idName)
    {
        free(node->idName);
        node->idName = NULL;
    }
    
    free(node);
}

TokenListNode* lexAnal(charBuf_t* inBuf)
{
    assert(inBuf);
    
    TokenListNode* tokenList = (TokenListNode*)calloc(1, sizeof(*tokenList));
    TokenListNode* endOfTokenList = tokenList;
    TokenListNode* newToken = NULL;
    size_t line = 1;
    size_t i = 0;
    
    while (i < inBuf->Size)
    {
        #ifndef NDEBUG
        //Check for infinite loop
            static size_t u = 0;
            if (u > 3000)
            {
                printf("\nchar %c\n", inBuf->buf[i]);
                assert(0);
                
            }
            u++;
        #endif
        
        if ( isspace(inBuf->buf[i]) )
        {
            if (inBuf->buf[i] == '\n')
                line++;
            i++;
            continue;
        }
        
        if ( (inBuf->buf[i] == '/') && (inBuf->buf[i+1] == '/') )
        {
            while ( (inBuf->buf[i] != '\n') && (i < inBuf->Size) )
                i++;
            continue;
        }
        
        if ( (inBuf->buf[i] == '/') && (inBuf->buf[i+1] == '*') )
        {
            while ( (! ( (inBuf->buf[i] == '*') && (inBuf->buf[i+1] == '/') ) ) && (i < inBuf->Size) )
                i++;
            continue;
        }
        
        switch (inBuf->buf[i])
        {
            #define UNK_CHAR \
            { \
                prln(); \
                printf("Unrecognized char %c in lexical analyzer", inBuf->buf[i]); \
                throw (COMPERR_UNKCHAR); \
            }
            case 'A'...'z': {
                                char identifier[MAX_LINE_LEN] = "";
                                
                                assert(sscanf(inBuf->buf + i, "%[A-z]", identifier));
                                
                                size_t idLen = strlen(identifier);
                                i+=idLen;
                                
                                LangKeywordType tokenType = LANGKWT_nil;
                                double tokenValue = -1;
                                char* idName = NULL;
                                
                                bool isUniqueId = true;
                                
                                #define LANGKWT_KEYWORD(keywd, type)   \
                                if (!strcmp(#keywd, identifier))    \
                                {                                   \
                                    tokenType = LANGKWT_##type;        \
                                    tokenValue = LANGKWD_##keywd;      \
                                    isUniqueId = false;             \
                                }
                                
                                #define holy_Keywords
                                
                                #include "./Lists/Language_keywords.hpp"
                                
                                #undef holy_Keywords
                                
                                #undef LANGKWT_KEYWORD
                                
                                if (isUniqueId)
                                {
                                    tokenType = LANGKWT_id;
                                    idName = (char*)calloc(idLen + 1, sizeof(char));
                                    strcpy(idName, identifier);
                                }
                                
                                newToken = tokenRetCtor(line, tokenType, tokenValue, endOfTokenList, NULL, idName);
                            }; break;
            case '(':
            case ')':       {
                                newToken = tokenRetCtor(line, LANGKWT_num_bracket, inBuf->buf[i++], endOfTokenList); 
                            }; break;
            case '-':
            case '/':
            case '*':
            case '+':       {
                                double oper = inBuf->buf[i++];
                                
                                newToken = tokenRetCtor(line, LANGKWT_mathOperator, oper, endOfTokenList); 
                            }; break;
                            
            case '=':       {
                                double oper = -0.5;
                                
                                if (inBuf->buf[i+1] == '=')
                                {
                                    i++;
                                    oper = 'e';//equal
                                } else
                                {
                                    oper = 'a';//assign
                                }
                                i++;
                                
                                newToken = tokenRetCtor(line, LANGKWT_mathOperator, oper, endOfTokenList); 
                            }; break;
            case '~':       {
                                newToken = tokenRetCtor(line, LANGKWT_mathOperator, inBuf->buf[i++], endOfTokenList); 
                            }; break;
                            
            case '!':       {
                                double oper = -0.5;
                                
                                if (inBuf->buf[i+1] == '=')
                                {
                                    i++;
                                    oper = 'n';
                                } else
                                {
                                    oper = 'f';
                                }
                                i++;
                                
                                newToken = tokenRetCtor(line, LANGKWT_mathOperator, oper, endOfTokenList); 
                            }; break;
                            
            case '>':       
            case '<':       {
                                double oper = -0.5;
                                
                                if (inBuf->buf[i+1] == '=')
                                {
                                    oper = (inBuf->buf[i] == '>') ? 'G' : 'L';
                                } else
                                {
                                    oper = (inBuf->buf[i] == '>') ? '>' : '<';
                                };
                                i++;
                                
                                newToken = tokenRetCtor(line, LANGKWT_mathOperator, oper, endOfTokenList); 
                            }; break;
                            
            case ',':
            case '.':       {
                                newToken = tokenRetCtor(line, LANGKWT_delim, inBuf->buf[i], endOfTokenList);
                                i++;
                            }; break;
                            
            case '0'...'9': {
                                double number = 0;
                                sscanf(inBuf->buf + i, "%lg", &number);
                                
                                while ( isdigit(inBuf->buf[++i]) );
                                
                                newToken = tokenRetCtor(line, LANGKWT_double, (unsigned)number, endOfTokenList); 
                            }; break;
                            
            default:        {
                                if ( ispunct(inBuf->buf[i]) )
                                {
                                    printf("punct %c\n", inBuf->buf[i]);
                                    i++;
                                    break;
                                }
                                UNK_CHAR;
                            }; break;
            #undef UNC_CHAR
        };
        
        if (newToken)
        {
            endOfTokenList->next = newToken;
            endOfTokenList = endOfTokenList->next;
        }
    }
    
    tokenList = tokenList->next;
    tokenDtor(tokenList->prev); tokenList->prev = NULL;
    return tokenList;
}

void tokenListDump(TokenListNode* firstNode, bool simple)
{
    assert(firstNode);
    if (!firstNode)
        return;
    
    static bool isCalled = false;
    
    FILE* dumpFile = NULL;
    FOPEN(dumpFile = fopen("./temp/tokenListDump.gv", "w"));
    
    if (!isCalled)
    {
        fprintf(dumpFile, "digraph G {\n"
                          "graph [ dpi = 300 ]");
    }
    
    TokenListNode* currentNode = firstNode;
    while (currentNode)
    {
        fprintf(dumpFile, "listNode_%p [label=\"", currentNode);
        
        tokenDump(currentNode, dumpFile, simple);
        
        fprintf(dumpFile, "\"]\n");
        
        if (currentNode->next)
        {
            fprintf(dumpFile, "listNode_%p -> listNode_%p\n", currentNode, currentNode->next);
        }
        currentNode = currentNode->next;
    }
    fprintf(dumpFile, "\n}");
    
    fclose(dumpFile);
    
    system("dot ./temp/tokenListDump.gv -Tpng -o ./temp/tokenListDump.png");
    
    system("xdot ./temp/tokenListDump.gv");
    
    isCalled = true;
}

void tokenDump(TokenListNode* node, FILE* stream, bool simple)
{
    assert(stream); assert(node);
    
    if (!simple)
    {
        fprintf(stream, 
                        "ListNode[0x%p]\n"
                        "{\n"
                        "\tvalue = ", node);
    }
                        
    if (node->idName)
    {
        fprintf(stream, "'%s'",  node->idName);
    } else
    {
        switch (node->type)
        {
            case LANGKWT_double:           {
                                            fprintf(stream, "%lg (%s)", node->data, LangKeywordsTypesStrings[node->type]);
                                        }; break;
            case LANGKWT_mathOperator:
            case LANGKWT_num_bracket:
            case LANGKWT_delim:            {
                                            fprintf(stream, "'%c' (%s)", (unsigned char)node->data, LangKeywordsTypesStrings[node->type]);
                                        }; break;
                                        
            case LANGKWT_operator_bracket:
            case LANGKWT_conditional:
            case LANGKWT_io:
            case LANGKWT_new_program:
            case LANGKWT_new_function:
            case LANGKWT_new_variable:
            case LANGKWT_return:
            case LANGKWT_assign:
            case LANGKWT_id:
            case LANGKWT_loop:             {
                                            fprintf(stream, "'%s' ( '%s' )", LangKeywordsStrings[(unsigned)node->data], LangKeywordsTypesStrings[node->type]);
                                        }; break;
            
            case LANGKWT_nil:              {
                                            fprintf(stream, "ERROR");
                                        }; break;
                                        
            default:                    {
                                            prln();
                                            printf("Unknown token type in tokenListDump");
                                            throw (COMPERR_UNKTOKTYPE);
                                        }
        }
    }
        
    if (!simple)
    {
        fprintf(stream, 
                        "\n\ttype = %i (%s)\n"
                        "}\n", 
                        node->type, LangKeywordsTypesStrings[node->type]);
    }
        
}

void tokenListDtor(TokenListNode* node)
{
    assert(node);
    
    while (node->prev)
    {
        node = node->prev;
    };
    
    while (node->next)
    {
        node = node->next;
        tokenDtor(node->prev);
    }
    tokenDtor(node);
}
