// created by jay 0.7 (c) 1998 Axel.Schreiner@informatik.uni-osnabrueck.de

#line 2 "XPathParser.jay"
using System;
using System.Xml;
using System.Xml.XPath;
using System.Diagnostics;

namespace System.Xml.XPath.Private
{
	internal class XPathParser
	{
		internal XPathParser()
		{
		}
		internal XPathExpression Parse(String path)
		{
			XPathTokenizer tokenizer = new XPathTokenizer(path);
			return (XPathExpression) yyparse(tokenizer);
		}
		// Console.WriteLine((char)27 + "[31m" + $1 + (char)27 + "[39m");
#line default

  /** error output stream.
      It should be changeable.
    */
  public System.IO.TextWriter ErrorOutput = System.Console.Out;

  /** simplified error message.
      @see <a href="#yyerror(java.lang.String, java.lang.String[])">yyerror</a>
    */
  public void yyerror (string message) {
    yyerror(message, null);
  }

  /** (syntax) error message.
      Can be overwritten to control message format.
      @param message text to be displayed.
      @param expected vector of acceptable tokens, if available.
    */
  public void yyerror (string message, string[] expected) {
    if ((expected != null) && (expected.Length  > 0)) {
      ErrorOutput.Write (message+", expecting");
      for (int n = 0; n < expected.Length; ++ n)
        ErrorOutput.Write (" "+expected[n]);
        ErrorOutput.WriteLine ();
    } else
      ErrorOutput.WriteLine (message);
  }

  /** debugging support, requires the package jay.yydebug.
      Set to null to suppress debugging messages.
    */
  protected yydebug.yyDebug debug;

  protected static  int yyFinal = 13;
  public static  string [] yyRule = {
    "$accept : Expr",
    "LocationPath : RelativeLocationPath",
    "LocationPath : AbsoluteLocationPath",
    "AbsoluteLocationPath : '/'",
    "AbsoluteLocationPath : '/' RelativeLocationPath",
    "AbsoluteLocationPath : AbbreviatedAbsoluteLocationPath",
    "RelativeLocationPath : Step",
    "RelativeLocationPath : RelativeLocationPath '/' Step",
    "RelativeLocationPath : AbbreviatedRelativeLocationPath",
    "Step : AxisTest OptPredicates",
    "Step : AbbreviatedStep",
    "AxisTest : AxisSpecifier QNAME",
    "AxisTest : AxisSpecifier WILDCARD",
    "AxisTest : AxisSpecifier WILDCARDNAME",
    "AxisTest : AxisSpecifier NODETYPE '(' OptLiteral ')'",
    "OptPredicates :",
    "OptPredicates : Predicate OptPredicates",
    "AxisSpecifier : AXISNAME AXIS_SUFFIX",
    "AxisSpecifier : AbbreviatedAxisSpecifier",
    "OptLiteral :",
    "OptLiteral : LITERAL",
    "Predicate : '[' Expr ']'",
    "AbbreviatedAbsoluteLocationPath : RECURSIVE_DESCENT RelativeLocationPath",
    "AbbreviatedRelativeLocationPath : RelativeLocationPath RECURSIVE_DESCENT Step",
    "AbbreviatedStep : '.'",
    "AbbreviatedStep : PARENT_NODE",
    "AbbreviatedAxisSpecifier :",
    "AbbreviatedAxisSpecifier : '@'",
    "Expr : OrExpr",
    "PrimaryExpr : VariableReference",
    "PrimaryExpr : '(' Expr ')'",
    "PrimaryExpr : LITERAL",
    "PrimaryExpr : NUMBER",
    "PrimaryExpr : FunctionCall",
    "VariableReference : '$' QNAME",
    "FunctionCall : FUNCTIONNAME '(' ')'",
    "FunctionCall : FUNCTIONNAME '(' ArgumentList ')'",
    "ArgumentList : Argument",
    "ArgumentList : Argument ',' ArgumentList",
    "Argument : Expr",
    "UnionExpr : PathExpr",
    "UnionExpr : UnionExpr '|' PathExpr",
    "PathExpr : LocationPath",
    "PathExpr : FilterExpr",
    "PathExpr : FilterExpr '/' RelativeLocationPath",
    "PathExpr : FilterExpr RECURSIVE_DESCENT RelativeLocationPath",
    "FilterExpr : PrimaryExpr",
    "FilterExpr : FilterExpr Predicate",
    "OrExpr : AndExpr",
    "OrExpr : OrExpr OP_OR AndExpr",
    "AndExpr : EqualityExpr",
    "AndExpr : AndExpr OP_AND EqualityExpr",
    "EqualityExpr : RelationalExpr",
    "EqualityExpr : EqualityExpr OP_EQ RelationalExpr",
    "EqualityExpr : EqualityExpr OP_NE RelationalExpr",
    "RelationalExpr : AdditiveExpr",
    "RelationalExpr : RelationalExpr OP_LT AdditiveExpr",
    "RelationalExpr : RelationalExpr OP_GT AdditiveExpr",
    "RelationalExpr : RelationalExpr OP_LE AdditiveExpr",
    "RelationalExpr : RelationalExpr OP_GE AdditiveExpr",
    "AdditiveExpr : MultiplicativeExpr",
    "AdditiveExpr : AdditiveExpr OP_PLUS MultiplicativeExpr",
    "AdditiveExpr : AdditiveExpr OP_MINUS MultiplicativeExpr",
    "MultiplicativeExpr : UnaryExpr",
    "MultiplicativeExpr : MultiplicativeExpr OP_MUL UnaryExpr",
    "MultiplicativeExpr : MultiplicativeExpr OP_DIV UnaryExpr",
    "MultiplicativeExpr : MultiplicativeExpr OP_MOD UnaryExpr",
    "UnaryExpr : UnionExpr",
    "UnaryExpr : OP_MINUS UnaryExpr",
  };
  protected static  string [] yyNames = {    
    "end-of-file",null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,"'$'",null,null,
    null,"'('","')'","'*'","'+'","','","'-'","'.'","'/'",null,null,null,
    null,null,null,null,null,null,null,null,null,"'<'","'='","'>'",null,
    "'@'",null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    "'['",null,"']'",null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,"'|'",null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    null,null,null,null,null,null,null,null,null,null,null,null,null,null,
    "ERROR","EOF","AXISNAME","\"axisname\"","OP_OR","\"or\"","OP_AND",
    "\"and\"","OP_EQ","OP_NE","\"!=\"","OP_LT","OP_GT","OP_LE","\"<=\"",
    "OP_GE","\">=\"","OP_PLUS","OP_MINUS","OP_MUL","OP_DIV","\"div\"",
    "OP_MOD","\"mod\"","LITERAL","\"quoted-string\"","NUMBER",
    "\"number\"","QNAME","\"qualified name\"","WILDCARD","WILDCARDNAME",
    "\"NCName:*\"","FUNCTIONNAME","\"function-name\"","NODETYPE",
    "\"node type\"","PARENT_NODE","\"..\"","AXIS_SUFFIX","\"::\"",
    "RECURSIVE_DESCENT","\"//\"",
  };

  /** index-checked interface to yyNames[].
      @param token single character or %token value.
      @return token name or [illegal] or [unknown].
    */
  public static string yyname (int token) {
    if ((token < 0) || (token > yyNames.Length)) return "[illegal]";
    string name;
    if ((name = yyNames[token]) != null) return name;
    return "[unknown]";
  }

  /** computes list of expected tokens on error by tracing the tables.
      @param state for which to compute the list.
      @return list of token names.
    */
  protected string[] yyExpecting (int state) {
    int token, n, len = 0;
    bool[] ok = new bool[yyNames.Length];

    if ((n = yySindex[state]) != 0)
      for (token = n < 0 ? -n : 0;
           (token < yyNames.Length) && (n+token < yyTable.Length); ++ token)
        if (yyCheck[n+token] == token && !ok[token] && yyNames[token] != null) {
          ++ len;
          ok[token] = true;
        }
    if ((n = yyRindex[state]) != 0)
      for (token = n < 0 ? -n : 0;
           (token < yyNames.Length) && (n+token < yyTable.Length); ++ token)
        if (yyCheck[n+token] == token && !ok[token] && yyNames[token] != null) {
          ++ len;
          ok[token] = true;
        }

    string [] result = new string[len];
    for (n = token = 0; n < len;  ++ token)
      if (ok[token]) result[n++] = yyNames[token];
    return result;
  }

  /** the generated parser, with debugging messages.
      Maintains a state and a value stack, currently with fixed maximum size.
      @param yyLex scanner.
      @param yydebug debug message writer implementing yyDebug, or null.
      @return result of the last reduction, if any.
      @throws yyException on irrecoverable parse error.
    */
  public Object yyparse (yyParser.yyInput yyLex, Object yyd)
				 {
    this.debug = (yydebug.yyDebug)yyd;
    return yyparse(yyLex);
  }

  /** initial size and increment of the state/value stack [default 256].
      This is not final so that it can be overwritten outside of invocations
      of yyparse().
    */
  protected int yyMax;

  /** executed at the beginning of a reduce action.
      Used as $$ = yyDefault($1), prior to the user-specified action, if any.
      Can be overwritten to provide deep copy, etc.
      @param first value for $1, or null.
      @return first.
    */
  protected Object yyDefault (Object first) {
    return first;
  }

  /** the generated parser.
      Maintains a state and a value stack, currently with fixed maximum size.
      @param yyLex scanner.
      @return result of the last reduction, if any.
      @throws yyException on irrecoverable parse error.
    */
  public Object yyparse (yyParser.yyInput yyLex)
				{
    if (yyMax <= 0) yyMax = 256;			// initial size
    int yyState = 0;                                   // state stack ptr
    int [] yyStates = new int[yyMax];	                // state stack 
    Object yyVal = null;                               // value stack ptr
    Object [] yyVals = new Object[yyMax];	        // value stack
    int yyToken = -1;					// current input
    int yyErrorFlag = 0;				// #tks to shift

    int yyTop = 0;
    goto skip;
    yyLoop:
    yyTop++;
    skip:
    for (;; ++ yyTop) {
      if (yyTop >= yyStates.Length) {			// dynamically increase
        int[] i = new int[yyStates.Length+yyMax];
        yyStates.CopyTo (i, 0);
        yyStates = i;
        Object[] o = new Object[yyVals.Length+yyMax];
        yyVals.CopyTo (o, 0);
        yyVals = o;
      }
      yyStates[yyTop] = yyState;
      yyVals[yyTop] = yyVal;
      if (debug != null) debug.push(yyState, yyVal);

      yyDiscarded: for (;;) {	// discarding a token does not change stack
        int yyN;
        if ((yyN = yyDefRed[yyState]) == 0) {	// else [default] reduce (yyN)
          if (yyToken < 0) {
            yyToken = yyLex.advance() ? yyLex.token() : 0;
            if (debug != null)
              debug.lex(yyState, yyToken, yyname(yyToken), yyLex.value());
          }
          if ((yyN = yySindex[yyState]) != 0 && ((yyN += yyToken) >= 0)
              && (yyN < yyTable.Length) && (yyCheck[yyN] == yyToken)) {
            if (debug != null)
              debug.shift(yyState, yyTable[yyN], yyErrorFlag-1);
            yyState = yyTable[yyN];		// shift to yyN
            yyVal = yyLex.value();
            yyToken = -1;
            if (yyErrorFlag > 0) -- yyErrorFlag;
            goto yyLoop;
          }
          if ((yyN = yyRindex[yyState]) != 0 && (yyN += yyToken) >= 0
              && yyN < yyTable.Length && yyCheck[yyN] == yyToken)
            yyN = yyTable[yyN];			// reduce (yyN)
          else
            switch (yyErrorFlag) {
  
            case 0:
              yyerror(String.Format ("syntax error, got token `{0}'", yyname (yyToken)), yyExpecting(yyState));
              if (debug != null) debug.error("syntax error");
              goto case 1;
            case 1: case 2:
              yyErrorFlag = 3;
              do {
                if ((yyN = yySindex[yyStates[yyTop]]) != 0
                    && (yyN += Token.yyErrorCode) >= 0 && yyN < yyTable.Length
                    && yyCheck[yyN] == Token.yyErrorCode) {
                  if (debug != null)
                    debug.shift(yyStates[yyTop], yyTable[yyN], 3);
                  yyState = yyTable[yyN];
                  yyVal = yyLex.value();
                  goto yyLoop;
                }
                if (debug != null) debug.pop(yyStates[yyTop]);
              } while (-- yyTop >= 0);
              if (debug != null) debug.reject();
              throw new yyParser.yyException("irrecoverable syntax error");
  
            case 3:
              if (yyToken == 0) {
                if (debug != null) debug.reject();
                throw new yyParser.yyException("irrecoverable syntax error at end-of-file");
              }
              if (debug != null)
                debug.discard(yyState, yyToken, yyname(yyToken),
  							yyLex.value());
              yyToken = -1;
              goto yyDiscarded;		// leave stack alone
            }
        }
        int yyV = yyTop + 1-yyLen[yyN];
        if (debug != null)
          debug.reduce(yyState, yyStates[yyV-1], yyN, yyRule[yyN], yyLen[yyN]);
        yyVal = yyDefault(yyV > yyTop ? null : yyVals[yyV]);
        switch (yyN) {
case 3:
#line 74 "XPathParser.jay"
  {
		yyVal = new RootPathExpression();
	}
  break;
case 4:
#line 78 "XPathParser.jay"
  {
		yyVal = new SlashExpression(new RootPathExpression(), (Expression) yyVals[0+yyTop]);
	}
  break;
case 7:
#line 88 "XPathParser.jay"
  {
		yyVal = new SlashExpression((Expression)yyVals[-2+yyTop] , (Expression)yyVals[0+yyTop]);
	}
  break;
case 9:
#line 97 "XPathParser.jay"
  {
		if(yyVals[0+yyTop] != null)
		{
			yyVal = new FilterExpression((Expression)yyVals[-1+yyTop], (Expression)yyVals[0+yyTop]);
		}
		else
		{
			yyVal = yyVals[-1+yyTop];
		}
	}
  break;
case 11:
#line 114 "XPathParser.jay"
  {
		yyVal = new NodeTest((XPathAxis)yyVals[-1+yyTop], 
						 	XPathNodeType.All, (XmlQualifiedName)yyVals[0+yyTop]);
	}
  break;
case 12:
#line 119 "XPathParser.jay"
  {
		yyVal = new NodeTest((XPathAxis)yyVals[-1+yyTop], 
						 	XPathNodeType.All, null);
	}
  break;
case 13:
#line 124 "XPathParser.jay"
  {
		yyVal = new NodeTest((XPathAxis)yyVals[-1+yyTop], 
						 	XPathNodeType.All, (XmlQualifiedName)yyVals[0+yyTop]);
	}
  break;
case 14:
#line 129 "XPathParser.jay"
  {
		XmlQualifiedName nodeName = null;
		if(yyVals[-1+yyTop] != null)
		{
			nodeName = new XmlQualifiedName((String)yyVals[-1+yyTop]);
		}
		if((XPathNodeType)yyVals[-3+yyTop] != XPathNodeType.ProcessingInstruction)
		{
			/* error */
		}
		
		yyVal = new NodeTest((XPathAxis)yyVals[-4+yyTop], 
						 	(XPathNodeType)yyVals[-3+yyTop], nodeName);
	}
  break;
case 15:
#line 148 "XPathParser.jay"
  {
		yyVal = null;
	}
  break;
case 16:
#line 152 "XPathParser.jay"
  {
		/* TODO predicate list */
		yyVal = yyVals[-1+yyTop];
	}
  break;
case 17:
#line 161 "XPathParser.jay"
  {
		yyVal = yyVals[-1+yyTop];
	}
  break;
case 19:
#line 169 "XPathParser.jay"
  {
		yyVal = null;
	}
  break;
case 21:
#line 178 "XPathParser.jay"
  {
		yyVal = yyVals[-1+yyTop];
	}
  break;
case 24:
#line 196 "XPathParser.jay"
  {
		yyVal = new NodeTest(XPathAxis.Self, XPathNodeType.All, null);
	}
  break;
case 25:
#line 200 "XPathParser.jay"
  {
		yyVal = new NodeTest(XPathAxis.Parent, XPathNodeType.All, null);
	}
  break;
case 26:
#line 208 "XPathParser.jay"
  {
		yyVal = XPathAxis.Child;
	}
  break;
case 27:
#line 212 "XPathParser.jay"
  {
		yyVal = XPathAxis.Attribute;
	}
  break;
case 29:
#line 224 "XPathParser.jay"
  { yyVal = null; }
  break;
case 30:
#line 225 "XPathParser.jay"
  { yyVal = null; }
  break;
case 31:
#line 226 "XPathParser.jay"
  { yyVal = new LiteralExpression((String)yyVals[0+yyTop]); }
  break;
case 32:
#line 227 "XPathParser.jay"
  { yyVal = new NumberExpression((Double)yyVals[0+yyTop]); }
  break;
case 34:
#line 233 "XPathParser.jay"
  {
		/* TODO: neede for XSL */
		yyVal = null;
	}
  break;
case 41:
#line 260 "XPathParser.jay"
  {
		yyVal = new UnionExpression((Expression)yyVals[-2+yyTop], (Expression)yyVals[0+yyTop]);
	}
  break;
case 44:
#line 270 "XPathParser.jay"
  {
		yyVal = new SlashExpression((Expression)yyVals[-2+yyTop], (Expression)yyVals[-1+yyTop]);
	}
  break;
case 45:
#line 274 "XPathParser.jay"
  {
		yyVal = null; /* TODO */
	}
  break;
case 47:
#line 283 "XPathParser.jay"
  {
		yyVal = new FilterExpression((Expression)yyVals[-1+yyTop], (Expression)yyVals[0+yyTop]);
	}
  break;
case 53:
#line 304 "XPathParser.jay"
  {
		yyVal = new EqualityExpression((Expression)yyVals[-2+yyTop], (Expression)yyVals[0+yyTop], false); 
	}
  break;
#line default
        }
        yyTop -= yyLen[yyN];
        yyState = yyStates[yyTop];
        int yyM = yyLhs[yyN];
        if (yyState == 0 && yyM == 0) {
          if (debug != null) debug.shift(0, yyFinal);
          yyState = yyFinal;
          if (yyToken < 0) {
            yyToken = yyLex.advance() ? yyLex.token() : 0;
            if (debug != null)
               debug.lex(yyState, yyToken,yyname(yyToken), yyLex.value());
          }
          if (yyToken == 0) {
            if (debug != null) debug.accept(yyVal);
            return yyVal;
          }
          goto yyLoop;
        }
        if (((yyN = yyGindex[yyM]) != 0) && ((yyN += yyState) >= 0)
            && (yyN < yyTable.Length) && (yyCheck[yyN] == yyState))
          yyState = yyTable[yyN];
        else
          yyState = yyDgoto[yyM];
        if (debug != null) debug.shift(yyStates[yyTop], yyState);
	 goto yyLoop;
      }
    }
  }

   static  short [] yyLhs  = {              -1,
    1,    1,    3,    3,    3,    2,    2,    2,    5,    5,
    7,    7,    7,    7,    8,    8,   10,   10,   11,   11,
   12,    4,    6,    9,    9,   13,   13,    0,   15,   15,
   15,   15,   15,   16,   17,   17,   18,   18,   19,   20,
   20,   21,   21,   21,   21,   22,   22,   14,   14,   23,
   23,   24,   24,   24,   25,   25,   25,   25,   25,   26,
   26,   26,   27,   27,   27,   27,   28,   28,
  };
   static  short [] yyLen = {           2,
    1,    1,    1,    2,    1,    1,    3,    1,    2,    1,
    2,    2,    2,    5,    0,    2,    2,    1,    0,    1,
    3,    2,    3,    1,    1,    0,    1,    1,    1,    3,
    1,    1,    1,    2,    3,    4,    1,    3,    1,    1,
    3,    1,    1,    3,    3,    1,    2,    1,    3,    1,
    3,    1,    3,    3,    1,    3,    3,    3,    3,    1,
    3,    3,    1,    3,    3,    3,    1,    2,
  };
   static  short [] yyDefRed = {            0,
    0,    0,   31,   32,    0,   25,    0,    0,    0,   24,
   27,    0,    0,   42,    0,    2,    5,    6,    8,    0,
   10,    0,   18,    0,   46,   29,   33,    0,   40,    0,
    0,    0,    0,    0,    0,   63,   17,   68,    0,    0,
    0,    0,   34,    0,    0,    0,    9,    0,   11,   12,
   13,    0,    0,    0,    0,    0,   47,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   35,
   39,    0,    0,   30,   23,    7,    0,   16,    0,    0,
   41,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   64,   65,   66,   36,    0,   21,   20,    0,
   38,   14,
  };
  protected static  short [] yyDgoto  = {            71,
   14,   15,   16,   17,   18,   19,   20,   47,   21,   22,
  100,   48,   23,   24,   25,   26,   27,   72,   73,   28,
   29,   30,   31,   32,   33,   34,   35,   36,
  };
  protected static  short [] yySindex = {          497,
 -291,  497,    0,    0,  -25,    0,  -42,  -42,  497,    0,
    0, -268,    0,    0,  -45,    0,    0,    0,    0,  -72,
    0, -260,    0, -240,    0,    0,    0, -101,    0,  -47,
 -237, -254, -217, -236, -207,    0,    0,    0,  -33,  -45,
  -45,   -8,    0,  -42,  -42,  497,    0,  -72,    0,    0,
    0,    3,  497,  500,  -42,  -42,    0,  497,  497,  497,
  497,  497,  497,  497,  497,  497,  497,  497,  497,    0,
    0,   -1,    5,    0,    0,    0,  -39,    0, -225, -237,
    0,  -45,  -45, -254, -217, -217, -236, -236, -236, -236,
 -207, -207,    0,    0,    0,    0,  497,    0,    0,   23,
    0,    0,
  };
  protected static  short [] yyRindex = {         -251,
    0, -251,    0,    0,    0,    0, -251,   18, -251,    0,
    0,    0,    0,    0,   46,    0,    0,    0,    0,    1,
    0,    0,    0,    6,    0,    0,    0,  148,    0,   63,
   24,   35,  452,  215,  165,    0,    0,    0, -251,   80,
   97,    0,    0, -251, -251, -251,    0,    1,    0,    0,
    0,    0, -251, -251, -251, -251,    0, -251, -251, -251,
 -251, -251, -251, -251, -251, -251, -251, -251, -251,    0,
    0,    0,   26,    0,    0,    0,    0,    0,   32,   30,
    0,  114,  131,  473,  458,  467,  260,  404,  419,  438,
  181,  196,    0,    0,    0,    0, -251,    0,    0,    0,
    0,    0,
  };
  protected static  short [] yyGindex = {           20,
    0,    2,    0,    0,   16,    0,    0,   27,    0,    0,
    0,   56,    0,    0,    0,    0,    0,   -9,    0,    0,
   37,    0,   36,   34,   25,   39,   12,   14,
  };
  protected static  short [] yyTable = {            56,
   15,   45,   12,   10,   37,   28,    9,   70,   40,   41,
   59,   60,   10,    8,   39,   38,   43,    3,   46,   13,
   53,   11,   54,   48,   49,   58,   50,   51,   42,   49,
   11,   52,   74,   26,   50,   26,   26,   65,   66,   96,
   26,   15,   79,   46,   15,    1,   28,   15,   97,   28,
   61,   62,   63,   98,   64,   99,   82,   83,    3,   75,
   76,    3,   43,  102,   48,   77,   37,   48,   67,   68,
   49,   69,   19,   49,   78,   50,   91,   92,   50,   22,
   93,   94,   95,   85,   86,   57,    1,  101,   80,    1,
   81,   84,    0,   15,    0,    0,    4,    0,   28,   87,
   88,   89,   90,   43,    0,    0,   43,    0,    0,    0,
    3,    0,    0,   45,    0,    0,   48,    0,    0,    0,
   22,    0,   49,   22,   15,    0,    0,   50,    0,    0,
   44,    0,    0,    0,    0,    0,    0,    4,    1,    0,
    4,    3,    0,    0,    0,    0,    0,   67,    0,    0,
    0,    0,    0,    0,   45,   43,    0,   45,    0,    0,
    0,    0,    0,    0,   60,    0,    0,    0,    0,    1,
    0,   44,   22,    0,   44,    0,    0,    0,    0,    0,
   61,    0,    0,    0,    0,    0,   43,    0,   67,    4,
    0,   67,    0,    0,    0,   62,    0,    0,    0,    0,
    0,    0,    0,   22,    0,   60,   45,    0,   60,    0,
    0,    0,    0,    0,   55,    0,    1,    0,    0,    0,
    4,   61,    0,   44,   61,    1,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   62,   45,    0,   62,
   67,    2,    0,    0,    0,    0,    0,    3,    0,    4,
   55,    6,   44,    0,   44,   55,    5,   60,   55,   56,
    6,   15,    0,   15,    7,   15,   15,    0,   15,   15,
   15,    0,   15,   61,   15,   15,   15,   15,    3,   15,
    3,    0,    3,    3,   48,    3,    3,    3,   62,    3,
   49,    3,    3,    3,    3,   50,    3,   50,   15,    0,
   56,    0,   26,   56,   26,   26,    1,   55,    1,   26,
    1,    1,    0,    1,    1,    1,    0,    1,    0,    1,
    1,    1,    1,   43,    1,   43,    0,   43,   43,    0,
   43,   43,   43,    0,   43,    0,   43,   43,   43,   43,
   22,   43,   22,    0,   22,   22,    0,   22,   22,   22,
    0,   22,   56,   22,   22,   22,   22,    4,   22,    4,
    0,    4,    4,    0,    4,    4,    4,    0,    4,    0,
    4,    4,    4,    4,   45,    4,   45,    0,   45,   45,
    0,   45,   45,   45,    0,   45,    0,   45,   45,   45,
   45,   44,   45,   44,    0,   44,   44,    0,   44,   44,
   44,    0,   44,   57,   44,   44,   44,   44,   67,   44,
   67,    0,   67,   67,    0,   67,   67,   67,   58,   67,
    0,   67,   67,   67,   67,   60,   67,   60,    0,   60,
   60,    0,   60,   60,   60,    0,   60,   59,   60,   60,
    0,   61,    0,   61,   57,   61,   61,   57,   61,   61,
   61,   52,   61,    0,   61,   61,   62,   53,   62,   58,
   62,   62,   58,   62,   62,   62,   54,   62,    0,   62,
   62,    0,   51,    0,    0,   55,    0,   55,   59,   55,
   55,   59,   55,   55,   55,    0,   55,    0,    0,    0,
    0,    0,   52,    0,    0,   52,   57,    0,   53,    0,
    0,   53,    0,    0,    0,    0,    0,   54,    0,    0,
   54,   58,    0,   51,    0,    0,   51,    0,    0,    0,
   56,    0,   56,    0,   56,   56,    0,   56,   56,   56,
   59,   56,   12,    0,    0,   12,    9,    0,    0,    9,
    0,    0,   10,    8,   52,   10,    8,    0,    0,    0,
   53,    0,    0,    0,    0,    0,    0,    0,    0,   54,
   11,    0,    0,   11,    0,   51,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   57,    0,   57,    0,   57,   57,
    0,   57,   57,   57,    0,   57,    0,    0,    0,   58,
    0,   58,    0,   58,   58,    0,   58,   58,   58,    0,
   58,    0,    0,    0,    0,    0,    0,    0,   59,    0,
   59,    0,   59,   59,    0,   59,   59,   59,    0,   59,
    0,    0,   52,    0,   52,    0,   52,   52,   53,    0,
   53,    0,   53,   53,    0,    0,    0,   54,    0,   54,
    0,   54,   54,   51,    0,   51,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    1,    0,    0,    1,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    2,    0,    0,    0,    0,    0,    3,    0,    4,
    3,    0,    4,    0,    0,    0,    5,    0,    0,    5,
    6,    0,    0,    6,    7,    0,    0,    7,
  };
  protected static  short [] yyCheck = {            47,
    0,   47,   36,   46,  296,    0,   40,   41,    7,    8,
  265,  266,   46,   47,   40,    2,  285,    0,   91,    0,
  261,   64,  124,    0,  285,  263,  287,  288,    9,    0,
   64,  292,   41,  285,    0,  287,  288,  274,  275,   41,
  292,   41,   40,   91,   44,    0,   41,   47,   44,   44,
  268,  269,  270,   93,  272,  281,   55,   56,   41,   44,
   45,   44,    0,   41,   41,   46,   41,   44,  276,  277,
   41,  279,   41,   44,   48,   41,   65,   66,   44,    0,
   67,   68,   69,   59,   60,   30,   41,   97,   53,   44,
   54,   58,   -1,   93,   -1,   -1,    0,   -1,   93,   61,
   62,   63,   64,   41,   -1,   -1,   44,   -1,   -1,   -1,
   93,   -1,   -1,    0,   -1,   -1,   93,   -1,   -1,   -1,
   41,   -1,   93,   44,  124,   -1,   -1,   93,   -1,   -1,
    0,   -1,   -1,   -1,   -1,   -1,   -1,   41,   93,   -1,
   44,  124,   -1,   -1,   -1,   -1,   -1,    0,   -1,   -1,
   -1,   -1,   -1,   -1,   41,   93,   -1,   44,   -1,   -1,
   -1,   -1,   -1,   -1,    0,   -1,   -1,   -1,   -1,  124,
   -1,   41,   93,   -1,   44,   -1,   -1,   -1,   -1,   -1,
    0,   -1,   -1,   -1,   -1,   -1,  124,   -1,   41,   93,
   -1,   44,   -1,   -1,   -1,    0,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  124,   -1,   41,   93,   -1,   44,   -1,
   -1,   -1,   -1,   -1,    0,   -1,  259,   -1,   -1,   -1,
  124,   41,   -1,   93,   44,  259,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   41,  124,   -1,   44,
   93,  275,   -1,   -1,   -1,   -1,   -1,  281,   -1,  283,
  298,  294,  298,   -1,  124,   41,  290,   93,   44,    0,
  294,  261,   -1,  263,  298,  265,  266,   -1,  268,  269,
  270,   -1,  272,   93,  274,  275,  276,  277,  261,  279,
  263,   -1,  265,  266,  261,  268,  269,  270,   93,  272,
  261,  274,  275,  276,  277,  261,  279,  263,  298,   -1,
   41,   -1,  285,   44,  287,  288,  261,   93,  263,  292,
  265,  266,   -1,  268,  269,  270,   -1,  272,   -1,  274,
  275,  276,  277,  261,  279,  263,   -1,  265,  266,   -1,
  268,  269,  270,   -1,  272,   -1,  274,  275,  276,  277,
  261,  279,  263,   -1,  265,  266,   -1,  268,  269,  270,
   -1,  272,   93,  274,  275,  276,  277,  261,  279,  263,
   -1,  265,  266,   -1,  268,  269,  270,   -1,  272,   -1,
  274,  275,  276,  277,  261,  279,  263,   -1,  265,  266,
   -1,  268,  269,  270,   -1,  272,   -1,  274,  275,  276,
  277,  261,  279,  263,   -1,  265,  266,   -1,  268,  269,
  270,   -1,  272,    0,  274,  275,  276,  277,  261,  279,
  263,   -1,  265,  266,   -1,  268,  269,  270,    0,  272,
   -1,  274,  275,  276,  277,  261,  279,  263,   -1,  265,
  266,   -1,  268,  269,  270,   -1,  272,    0,  274,  275,
   -1,  261,   -1,  263,   41,  265,  266,   44,  268,  269,
  270,    0,  272,   -1,  274,  275,  261,    0,  263,   41,
  265,  266,   44,  268,  269,  270,    0,  272,   -1,  274,
  275,   -1,    0,   -1,   -1,  261,   -1,  263,   41,  265,
  266,   44,  268,  269,  270,   -1,  272,   -1,   -1,   -1,
   -1,   -1,   41,   -1,   -1,   44,   93,   -1,   41,   -1,
   -1,   44,   -1,   -1,   -1,   -1,   -1,   41,   -1,   -1,
   44,   93,   -1,   41,   -1,   -1,   44,   -1,   -1,   -1,
  261,   -1,  263,   -1,  265,  266,   -1,  268,  269,  270,
   93,  272,   36,   -1,   -1,   36,   40,   -1,   -1,   40,
   -1,   -1,   46,   47,   93,   46,   47,   -1,   -1,   -1,
   93,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   93,
   64,   -1,   -1,   64,   -1,   93,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,  261,   -1,  263,   -1,  265,  266,
   -1,  268,  269,  270,   -1,  272,   -1,   -1,   -1,  261,
   -1,  263,   -1,  265,  266,   -1,  268,  269,  270,   -1,
  272,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  261,   -1,
  263,   -1,  265,  266,   -1,  268,  269,  270,   -1,  272,
   -1,   -1,  261,   -1,  263,   -1,  265,  266,  261,   -1,
  263,   -1,  265,  266,   -1,   -1,   -1,  261,   -1,  263,
   -1,  265,  266,  261,   -1,  263,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,  259,   -1,   -1,  259,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,  275,   -1,   -1,   -1,   -1,   -1,  281,   -1,  283,
  281,   -1,  283,   -1,   -1,   -1,  290,   -1,   -1,  290,
  294,   -1,   -1,  294,  298,   -1,   -1,  298,
  };

#line 341 "XPathParser.jay"
}
#line default
namespace yydebug {
        using System;
	 public interface yyDebug {
		 void push (int state, Object value);
		 void lex (int state, int token, string name, Object value);
		 void shift (int from, int to, int errorFlag);
		 void pop (int state);
		 void discard (int state, int token, string name, Object value);
		 void reduce (int from, int to, int rule, string text, int len);
		 void shift (int from, int to);
		 void accept (Object value);
		 void error (string message);
		 void reject ();
	 }
	 
	 class yyDebugSimple : yyDebug {
		 void println (string s){
			 Console.Error.WriteLine (s);
		 }
		 
		 public void push (int state, Object value) {
			 println ("push\tstate "+state+"\tvalue "+value);
		 }
		 
		 public void lex (int state, int token, string name, Object value) {
			 println("lex\tstate "+state+"\treading "+name+"\tvalue "+value);
		 }
		 
		 public void shift (int from, int to, int errorFlag) {
			 switch (errorFlag) {
			 default:				// normally
				 println("shift\tfrom state "+from+" to "+to);
				 break;
			 case 0: case 1: case 2:		// in error recovery
				 println("shift\tfrom state "+from+" to "+to
					     +"\t"+errorFlag+" left to recover");
				 break;
			 case 3:				// normally
				 println("shift\tfrom state "+from+" to "+to+"\ton error");
				 break;
			 }
		 }
		 
		 public void pop (int state) {
			 println("pop\tstate "+state+"\ton error");
		 }
		 
		 public void discard (int state, int token, string name, Object value) {
			 println("discard\tstate "+state+"\ttoken "+name+"\tvalue "+value);
		 }
		 
		 public void reduce (int from, int to, int rule, string text, int len) {
			 println("reduce\tstate "+from+"\tuncover "+to
				     +"\trule ("+rule+") "+text);
		 }
		 
		 public void shift (int from, int to) {
			 println("goto\tfrom state "+from+" to "+to);
		 }
		 
		 public void accept (Object value) {
			 println("accept\tvalue "+value);
		 }
		 
		 public void error (string message) {
			 println("error\t"+message);
		 }
		 
		 public void reject () {
			 println("reject");
		 }
		 
	 }
}
// %token constants
 class Token {
  public const int ERROR = 257;
  public const int EOF = 258;
  public const int AXISNAME = 259;
  public const int axisname = 260;
  public const int OP_OR = 261;
  public const int or = 262;
  public const int OP_AND = 263;
  public const int and = 264;
  public const int OP_EQ = 265;
  public const int OP_NE = 266;
  public const int OP_LT = 268;
  public const int OP_GT = 269;
  public const int OP_LE = 270;
  public const int OP_GE = 272;
  public const int OP_PLUS = 274;
  public const int OP_MINUS = 275;
  public const int OP_MUL = 276;
  public const int OP_DIV = 277;
  public const int div = 278;
  public const int OP_MOD = 279;
  public const int mod = 280;
  public const int LITERAL = 281;
  public const int NUMBER = 283;
  public const int number = 284;
  public const int QNAME = 285;
  public const int WILDCARD = 287;
  public const int WILDCARDNAME = 288;
  public const int FUNCTIONNAME = 290;
  public const int NODETYPE = 292;
  public const int PARENT_NODE = 294;
  public const int AXIS_SUFFIX = 296;
  public const int RECURSIVE_DESCENT = 298;
  public const int yyErrorCode = 256;
 }
 namespace yyParser {
  using System;
  /** thrown for irrecoverable syntax errors and stack overflow.
    */
  public class yyException : System.Exception {
    public yyException (string message) : base (message) {
    }
  }

  /** must be implemented by a scanner object to supply input to the parser.
    */
  public interface yyInput {
    /** move on to next token.
        @return false if positioned beyond tokens.
        @throws IOException on input error.
      */
    bool advance (); // throws java.io.IOException;
    /** classifies current token.
        Should not be called if advance() returned false.
        @return current %token or single character.
      */
    int token ();
    /** associated with current token.
        Should not be called if advance() returned false.
        @return value for token().
      */
    Object value ();
  }
 }
} // close outermost namespace, that MUST HAVE BEEN opened in the prolog
