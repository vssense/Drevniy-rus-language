# Programming language of drevniy rus  :scream_cat:

### Grammar :+1:

>``` ruby
>Abbreviation:
>       Gram | grammar
>       Prog | program
>       Defi | definition
>       FDec | function declaration
>       Comp | compound
>       Stat | statement
>       Expr | expression
>       Simp | simple expression
>       Term | term
>       Prim | primary grammar(factor)
>       Call | function call
>       Init | initialization
>       Assg | assignment
>       Jump | jump
>       Cond | conditional statement
>       Loop | loop
>       Var  | variable (char*)
>       Num  | number(constant)
>    
>    
>    Rules:
>       Gram ::= Prog 'EOF'
>       Prog ::= {Defi}+
>       Defi ::= FDec Comp
>       FDec ::= 'возьмем' Var '(' ')' | Var '(' Var  { ',' Var }* ')'
>       Comp ::= 'начнем' Stat* 'закончим'
>       Stat ::= [Expr, Init, Assg, Jump] ';' | Cond | Loop
>       Expr ::= Simp {['<', '>', '==', '!=', '<=', '>='] Simp}*
>       Simp ::= Term {['+', '-'] Term}*
>       Term ::= Prim {['*', '/'] Prim}*
>       Prim ::= '(' Expr ')' | Num | Var | Call
>       Call ::= Var '(' ')' | Var '(' Expr { ',' Expr}* ')'
>       Init ::= 'возьмем' Var 'зомбируем' Expr
>       Assg ::= Var 'зомбируем' Expr
>       Jump ::= 'положим' Expr
>       Cond ::= 'в_случае' '(' Expr ')' Comp | 'в_случае' '(' Expr ')' Comp 'иначе' Comp
>       Loop ::= 'зомбирование_идет' '(' Expr ')' Comp
>       Var  ::= ['A' - 'Z', 'a' - 'z', '_'] ['A' - 'Z', 'a' - 'z', '0' - '9', '_']*
>       Num  ::= 'ноль' - 'десятичок'
>    ```

### Syntax :+1:

> There is no digits - you can use only drevniy rus arithmetics :smirk_cat: :
>
> - 0 - ноль
> - 1 - целковый
> - 2 - полушка
> - 3 - четвертушка
> - 4 - осьмушка
> - 5 - пудовичок
> - 6 - медячок
> - 7 - серебрячок
> - 8 - золотничок
> - 9 - девятичок
> - 10 - десятичок
>
> As you could already understand
>
> - loop is "**зомбирование_идет**"
> - if is "**в_случае**"
> - else is "**иначе**"
> - '=' is "**зомбируем**"
> - start of block is "**начнем**"
> - end of block is "**закончим**"
>
> To declare a function or variable use key-word "**возьмем**"

### Features :+1:

>Everything is functions - you can't use global variables
>
>There is only one variable scope - function
>
>Also you have to use block's operators in each while and if/else statement
>
>You have only one variable type (double in c++)
>
>Every function must return a value
>
>You must have "**main**" function - it will be called first
>
>Main must return 0 if implementation was finished successfully
>
>You have to finish each statement with semicolon
>
>#### Translating from another B05-032 language :
>
>Call **translate** function with name a file, in which the [tree](https://github.com/vssense/DED-MY_LANGUAGE/tree_in_written_form.txt) is stored, you will get program
>
>on my language in translated_program.txt  ​ :sparkles:
>
>#### Some std functions :
>
>1. govoru(a) - prints a
>2. a = nepravdoi() - takes a from stdin
>3. a = sqrt(a) - takes square root
>4. a = floor(a) - round to int





### Tree

>  In tree.h you can find all types of nodes and it's values (MathOp / double / char*) 
>
> How to write tree in file?
>
> { *left subroot* **NodeType | NodeValue** *right subroot*} 
>
> if subroot is empty, just write { }



##### Now some examples

###### factorial : 

```
возьмем main()
начнем
    возьмем a зомбируем ноль;
    a зомбируем nepravdoi();
    govoru(Fact(a));
    положим ноль;
закончим

возьмем Fact(a)
начнем
    в_случае (a < полушка) 
    начнем
        положим целковый;
    закончим
    иначе
    начнем
    положим a * Fact(a - целковый);
    закончим
закончим
```

###### square equation :

```
возьмем main()
начнем
    возьмем a зомбируем nepravdoi();
    возьмем b зомбируем nepravdoi();
    возьмем c зомбируем nepravdoi();

    Square(a, b, c);   
    положим ноль;
закончим

возьмем Square(a, b, c)
начнем
    в_случае (a == ноль)
    начнем
        в_случае (b == ноль)
        начнем
            в_случае (c == ноль)
            начнем
                govoru(золотничок);
            закончим
            иначе
            начнем
                govoru(ноль);
            закончим
        закончим
        иначе
        начнем
                govoru((целковый - полушка) * c / b);
        закончим
    закончим
    иначе
    начнем
            возьмем ribnikov зомбируем koren(b * b - осьмушка * a * c);
        в_случае (ribnikov == ноль)
        начнем
            govoru((целковый - полушка) * b / (полушка * a));
        закончим
        иначе
        начнем
            govoru(((целковый - полушка) * b + ribnikov) / (полушка * a));
            govoru(((целковый - полушка) * b - ribnikov) / (полушка * a));
        закончим
    закончим
    положим ноль;
закончим
```
