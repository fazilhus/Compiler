$$
\text{Tatar language ruleset} \\
\begin{align}
    [\text{Prog}] &\to [\text{Stmt}]^* \\
    [\text{Stmt}] &\to 
    \begin{cases}
        \text{Exit}([\text{Expr}]); \\
        \text{let}\space [ident] = [\text{Expr}]; \\
        [ident] = [\text{Expr}]; \\
        \text{if}([\text{Expr}]) \{[\text{Stmt}];...\}
    \end{cases} \\
    [\text{Expr}] &\to 
    \begin{cases}
        [\text{Term}] \\
        [\text{BinExpr}] 
    \end{cases} \\
    [\text{Term}] &\to
    \begin{cases}
        [\text{int\_lit}] \\
        [\text{ident}] 
    \end{cases} \\
    [\text{BinExpr}] &\to
    \begin{cases}
        [\text{Expr}] * [\text{Expr}] & \text{prec} = 2 \\
        [\text{Expr}] / [\text{Expr}] & \text{prec} = 2 \\
        [\text{Expr}] + [\text{Expr}] & \text{prec} = 1 \\
        [\text{Expr}] - [\text{Expr}] & \text{prec} = 1 \\
        [\text{Expr}] < [\text{Expr}] & \text{prec} = 0 \\
        [\text{Expr}] <= [\text{Expr}] & \text{prec} = 0 \\
        [\text{Expr}] > [\text{Expr}] & \text{prec} = 0 \\
        [\text{Expr}] >= [\text{Expr}] & \text{prec} = 0 \\
        [\text{Expr}] == [\text{Expr}] & \text{prec} = 0 \\
    \end{cases}
\end{align}
$$