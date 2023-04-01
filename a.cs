IExpressionNode CreateTree(ParseContext context)
{
    // Push the tokens
    while (context.LexicalAnalyzer.TryGetNextToken(ref context.Token))
    {
        // Unexpected
        if (context.Token.Kind == TokenKind.Unexpected)
        {
            throw new ParseException(ParseExceptionKind.UnexpectedSymbol, context.Token, context.Expression);
        }
        // Operator
        else if (context.Token.IsOperator)
        {
            PushOperator(context);
        }
        // Operand
        else
        {
            PushOperand(context);
        }

        context.LastToken = context.Token;
    }

    // No tokens
    if (context.LastToken == null)
    {
        return null;
    }

    // Check unexpected end of expression
    if (context.Operators.Count > 0)
    {
        var unexpectedLastToken = false;
        switch (context.LastToken.Kind)
        {
            case TokenKind.EndGroup:        // ")" logical grouping
            case TokenKind.EndIndex:        // "]"
            case TokenKind.EndParameters:   // ")" function call
                // Legal
                break;
            case TokenKind.Function:
                // Illegal
                unexpectedLastToken = true;
                break;
            default:
                unexpectedLastToken = context.LastToken.IsOperator;
                break;
        }

        if (unexpectedLastToken || context.LexicalAnalyzer.UnclosedTokens.Any())
        {
            throw new ParseException(ParseExceptionKind.UnexpectedEndOfExpression, context.LastToken, context.Expression);
        }
    }

    // Flush operators
    while (context.Operators.Count > 0)
    {
        FlushTopOperator(context);
    }

    // Check max depth
    var result = context.Operands.Single();
    CheckMaxDepth(context, result);

    return result;
}