#ifdef LANG_MATH_OPERATORS 

//  LANG_MATH_OP( name, char, priority )
//┌───────────────────────────────────────────┐
    LANG_MATH_OP( ln    , 'l', 0            )
//├───────────────────────────────────────────┤
    LANG_MATH_OP( deg   , '^', 1            )
//├───────────────────────────────────────────┤
    LANG_MATH_OP( mult  , '*', 2            )
    LANG_MATH_OP( div   , '/', 2            )
//├───────────────────────────────────────────┤
    LANG_MATH_OP( plus  , '+', 3            )
    LANG_MATH_OP( minus , '─', 3            )
//└───────────────────────────────────────────┘
#endif

#ifdef LANG_KEYWORDS
//┌────────────────────────────────────────────┐
//  LANG_KEYWORD( keyword,  keyword_type     )
//├────────────────────────────────────────────┤
    LANG_KEYWORD( begin,    operator_bracket )
    LANG_KEYWORD( end,      operator_bracket )  
//├────────────────────────────────────────────┤
    LANG_KEYWORD( if,       conditional      )
    LANG_KEYWORD( else,     conditional      )
    LANG_KEYWORD( switch,   conditional      )
    LANG_KEYWORD( case,     conditional      )  
    LANG_KEYWORD( default,  conditional      )  
//├────────────────────────────────────────────┤
    LANG_KEYWORD( scan,     io               ) 
    LANG_KEYWORD( print,    io               )
//├────────────────────────────────────────────┤
    LANG_KEYWORD( program,  new_program      )  
//├────────────────────────────────────────────┤
    LANG_KEYWORD( function, new_function     )  
    LANG_KEYWORD( pass,     new_function     ) 
//├────────────────────────────────────────────┤
    LANG_KEYWORD( declare,  new_variable     )  
//├────────────────────────────────────────────┤
    LANG_KEYWORD( return,   return           )  
//├────────────────────────────────────────────┤
    LANG_KEYWORD( assign,   assign           ) 
//├────────────────────────────────────────────┤
    LANG_KEYWORD( while,    loop             )
//└────────────────────────────────────────────┘ 
#endif

#ifdef LANG_TYPES
//┌───────────────────────────────┐
//  LANG_TYPE( type             )
//├───────────────────────────────┤
    LANG_TYPE( double           )
//├───────────────────────────────┤
    LANG_TYPE( mathOperator     )
    LANG_TYPE( num_bracket      )
//├───────────────────────────────┤
    LANG_TYPE( operator_bracket )
//├───────────────────────────────┤
    LANG_TYPE( conditional      )
    LANG_TYPE( loop             )
//├───────────────────────────────┤
    LANG_TYPE( io               )
//├───────────────────────────────┤
    LANG_TYPE( new_program      )
    LANG_TYPE( new_function     )
    LANG_TYPE( new_variable     )
//├───────────────────────────────┤
    LANG_TYPE( return           )
//├───────────────────────────────┤
    LANG_TYPE( assign           )
//├───────────────────────────────┤
    LANG_TYPE( id               )
//├───────────────────────────────┤
    LANG_TYPE( nil              )
//├───────────────────────────────┤
    LANG_TYPE( delim            )
//├───────────────────────────────┤
    LANG_TYPE( string           )
//├───────────────────────────────┤
    LANG_TYPE( function         )
//└───────────────────────────────┘
#endif
