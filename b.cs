void PushOperator(ParseContext context)
{
    // Flush higher or equal precedence
    if (context.Token.Associativity == Associativity.LeftToRight)
    {
        var precedence = context.Token.Precedence;
        while (context.Operators.Count > 0)
        {
            var topOperator = context.Operators.Peek();
            if (precedence <= topOperator.Precedence &&
                topOperator.Kind != TokenKind.StartGroup &&     // Unless top is "(" logical grouping
                topOperator.Kind != TokenKind.StartIndex &&     // or unless top is "["
                topOperator.Kind != TokenKind.StartParameters &&// or unless top is "(" function call
                topOperator.Kind != TokenKind.Separator)        // or unless top is ","
            {
                FlushTopOperator(context);
                continue;
            }

            break;
        }
    }

    // Push the operator
    context.Operators.Push(context.Token);

    // Process closing operators now, since context.LastToken is required
    // to accurately process TokenKind.EndParameters
    switch (context.Token.Kind)
    {
        case TokenKind.EndGroup:        // ")" logical grouping
        case TokenKind.EndIndex:        // "]"
        case TokenKind.EndParameters:   // ")" function call
            FlushTopOperator(context);
            break;
    }
}

void FlushTopOperator(ParseContext context)
{
    // Special handling for closing operators
    switch (context.Operators.Peek().Kind)
    {
        case TokenKind.EndIndex:        // "]"
            FlushTopEndIndex(context);
            return;

        case TokenKind.EndGroup:        // ")" logical grouping
            FlushTopEndGroup(context);
            return;

        case TokenKind.EndParameters:   // ")" function call
            FlushTopEndParameters(context);
            return;
    }

    // Pop the operator
    var @operator = context.Operators.Pop();

    // Create the node
    var node = (Container)@operator.ToNode();

    // Pop the operands, add to the node
    var operands = PopOperands(context, @operator.OperandCount);
    foreach (var operand in operands)
    {
        // Flatten nested And
        if (node is And)
        {
            if (operand is And nestedAnd)
            {
                foreach (var nestedParameter in nestedAnd.Parameters)
                {
                    node.AddParameter(nestedParameter);
                }

                continue;
            }
        }
        // Flatten nested Or
        else if (node is Or)
        {
            if (operand is Or nestedOr)
            {
                foreach (var nestedParameter in nestedOr.Parameters)
                {
                    node.AddParameter(nestedParameter);
                }

                continue;
            }
        }

        node.AddParameter(operand);
    }

    // Push the node to the operand stack
    context.Operands.Push(node);
}