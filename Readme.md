# 🚀 Novex Language Compiler

**Novex** is a custom programming language and compiler built using **Flex**, **Bison**, and **LLVM**. Designed as a compiler project, it demonstrates end-to-end compilation — from lexical analysis to code generation via LLVM IR.

---

## 🧰 Features

Novex supports:

- ✅ Variable declarations (with types)
- ✅ Integer and string literals
- ✅ Arithmetic operations (`+`, `-`, `*`)
- ✅ `PRINT` statements for output
- ✅ `READ` statements for input
- ✅ Assignment operations (`copy`)
- ✅ User-defined functions
- ✅ Function calls with `run`
- ✅ Scoped symbol table
- ✅ LLVM-based IR generation and execution

---

## 🛠️ Requirements

Make sure the following are installed on your system:

- `flex`
- `bison`
- `clang++`
- `llvm` (with `llvm-config`)
- `make`

You can check for them with:

```bash
flex --version
bison --version
clang++ --version
llvm-config --version
```

---

## 🧪 Building the Compiler

To compile Novex, just run:

```bash
make run
```

This will:

- Generate the parser (`novex.tab.c`) and lexer (`lex.yy.c`)
- Compile all source files
- Link against LLVM
- Produce an executable named `final_exec`

---

## 🏃 Running a Program

Once built, run your Novex program like this:

```bash
./final_exec 
```


---

## 📁 Project Structure

| File | Purpose |
|------|---------|
| `novex.l` | Flex lexer for Novex |
| `novex.y` | Bison grammar for parsing |
| `AST.h/.cpp` | Abstract Syntax Tree node definitions |
| `IR.h/.cpp` | LLVM IR generation logic |
| `symbol_table.h/.cpp` | Scope-aware symbol table |
| `input.novex` | Test Code File |
| `Makefile` | Automates build steps |

---

## 🔤 Example Novex Code

```novex
make int:a.
make int:b.
make str:name.
make int:decision.
//addition//[
    local int:result.
    print:::_$Enter a and b:$_.
    read:::a.
    read:::b.
    operation sum,result,a,b.
    print:::_$Addition Answer:$_.
    print:::result.
]
//subtraction//[
    local int:result.
    print:::_$Enter a and b:$_.
    read:::a.
    read:::b.
    operation sub,result,a,b.
    print:::_$Subtraction Answer:$_.
    print:::result.
]
//multiplication//[
    local int:result.
    print:::_$Enter a and b:$_.
    read:::a.
    read:::b.
    operation product,result,a,b.
    print:::_$Product Answer:$_.
    print:::result.
]
//division//[
    local int:result.
    print:::_$Enter a and b:$_.
    read:::a.
    read:::b.
    operation div,result,a,b.
    print:::_$Division Answer:$_.
    print:::result.
]
//calculator//[
    if [equals?,decision,1][
        run //addition//.
    ]
    else[
        if [equals?,decision,2][
            run //subtraction//.
        ]
        else[
            if [equals?,decision,3][
                run //multiplication//.
            ]
            else[
                if[equals?,decision,4][
                    run //division//.
                ]
            ]
        ]
    ]
]

//check//
[
    local int: a.
    local int: result.
    read:::a.

    print::: _$Comparing input a with 20$_.
    
    print::: _$a>20$_.
    gt result,a,20.
    print:::result.
    
    print::: _$a<20$_.
    lt result,a,20.
    print:::result.

    print::: _$a>=20$_.
    gte result,a,20.
    print:::result.

    print::: _$a<=20$_.
    lte result,a,20.
    print:::result.


   
]
//logical//[
   if [equals?, 10, 10 or not equals?, 10, 10 and not equals?, 10, 8][
        print::: _$True$_.
    ]
    else[
        print::: _$False$_.
    ]
]

//starter//[
    print:::_$Enter Your Name: $_.
    read:::name.
    print:::_$Hello $_.
    print:::name.
]
//main//[
     #testing all functionalities#    
    run //starter//.
    local int:i.
    for i,copy,0. go 3 ++ 1[
        print:::_$Enter 1 for addition,
        2 for Subtraction,
        3 for Multiplication,
        4 for Division:$_.
        read:::decision.
        run //calculator//.
    ]
    run //check//.

    run //logical//. 
]
```

---

## 🧠 How It Works

- **Lexical Analysis**: Done using `flex` via `novex.l`.
- **Parsing**: Grammar rules in `novex.y` use `bison` to construct the AST.
- **AST Construction**: Custom C++ classes like `FuncAST`, `AssignmentAST`, `FuncCallAST`, etc.
- **Symbol Table**: A scoped symbol table tracks variable types and locations.
- **LLVM Codegen**: AST nodes emit LLVM IR for actual execution.

---
