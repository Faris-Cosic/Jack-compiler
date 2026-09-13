#include "compilationEngine.hpp"
#include "jackTokenizer.hpp"

void compilationEngine::compileVar() {
  writeTag();
  writeTag();
  writeTag();
  while (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
         tokenizer.symbol() == ',') {
    writeTag();
    writeTag();
  }
  writeTag();
}

void compilationEngine::compileClass() {
  output << "<class>";
  tokenizer.advance();
  writeTag();
  writeTag();
  writeTag();

  while (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
         (tokenizer.keyword() == jackTokenizer::Keyword::Field ||
          tokenizer.keyword() == jackTokenizer::Keyword::Static)) {
    compileClassVarDec();
  }

  while (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
         (tokenizer.keyword() == jackTokenizer::Keyword::Constructor ||
          tokenizer.keyword() == jackTokenizer::Keyword::Function ||
          tokenizer.keyword() == jackTokenizer::Keyword::Method)) {
    compileSubroutine();
  }

  writeTag();

  output << "</class>";
}

void compilationEngine::compileClassVarDec() {
  output << "<classVarDec>";
  compileVar();
  output << "</classVarDec>";
}

void compilationEngine::compileSubroutine() {
  output << "<subroutineDec>";
  writeTag(); // function type
  writeTag(); // return type
  writeTag(); // function name
  writeTag(); // (
  compileParameterList();
  writeTag(); // )
  compileSubroutineBody();
  output << "</subroutineDec>";
}

void compilationEngine::compileParameterList() {
  output << "<parameterList>";
  while (!(tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
           tokenizer.symbol() == ')')) {
    writeTag();
  }
  output << "</parameterList>";
}

void compilationEngine::compileSubroutineBody() {
  output << "<subroutineBody>";
  writeTag(); // {
  while (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
         tokenizer.keyword() == jackTokenizer::Keyword::Var) {
    compileVarDec();
  }
  compileStatements();
  writeTag(); // }
  output << "</subroutineBody>";
}

void compilationEngine::compileVarDec() {
  output << "<varDec>";
  compileVar();
  output << "</varDec>";
}

void compilationEngine::compileStatements() {
  output << "<statements>";
  while (!(tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
           tokenizer.symbol() == '}')) {
    if (tokenizer.keyword() == jackTokenizer::Keyword::Let)
      compileLet();
    else if (tokenizer.keyword() == jackTokenizer::Keyword::If)
      compileIf();
    else if (tokenizer.keyword() == jackTokenizer::Keyword::While)
      compileWhile();
    else if (tokenizer.keyword() == jackTokenizer::Keyword::Do)
      compileDo();
    else if (tokenizer.keyword() == jackTokenizer::Keyword::Return)
      compileReturn();
  }
  output << "</statements>";
}

void compilationEngine::compileLet() {
  output << "<letStatement>";
  writeTag();
  writeTag();
  if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
      tokenizer.symbol() == '[') {
    writeTag();
    compileExpression();
    writeTag();
  }
  writeTag();
  compileExpression();
  writeTag();
  output << "</letStatement>";
}

void compilationEngine::compileIf() {
  output << "<ifStatement>";
  writeTag();
  writeTag();
  compileExpression();
  writeTag();
  writeTag();
  compileStatements();
  writeTag();
  if (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
      tokenizer.keyword() == jackTokenizer::Keyword::Else) {
    writeTag();
    writeTag();
    compileStatements();
    writeTag();
  }
  output << "</ifStatement>";
}

void compilationEngine::compileWhile() {
  output << "<whileStatement>";
  writeTag();
  writeTag();
  compileExpression();
  writeTag();
  writeTag();
  compileStatements();
  writeTag();
  output << "</whileStatement>";
}

void compilationEngine::compileDo() {
  output << "<doStatement>";
  writeTag();
  writeTag();
  if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
      tokenizer.symbol() == '(') {
    writeTag();
    compileExpressionList();
    writeTag();
  } else {
    writeTag();
    writeTag();
    writeTag();
    compileExpressionList();
    writeTag();
  }

  writeTag();
  output << "</doStatement>";
}

void compilationEngine::compileReturn() {
  output << "<returnStatement>";
  writeTag();
  if (!(tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
        tokenizer.symbol() == ';'))
    compileExpression();
  writeTag();
  output << "</returnStatement>";
}

void compilationEngine::compileExpression() {
  output << "<expression>";
  compileTerm();
  while (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
         ((tokenizer.symbol() == '+' || tokenizer.symbol() == '-' ||
           tokenizer.symbol() == '*' || tokenizer.symbol() == '/' ||
           tokenizer.symbol() == '&' || tokenizer.symbol() == '|' ||
           tokenizer.symbol() == '<' || tokenizer.symbol() == '>' ||
           tokenizer.symbol() == '='))) {
    writeTag();
    compileTerm();
  }
  output << "</expression>";
}

void compilationEngine::compileTerm() {
  output << "<term>";
  if (tokenizer.tokenType() == jackTokenizer::Token::Identifier) {
    writeTag();
    if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
        tokenizer.symbol() == '.') {
      writeTag();
      writeTag();
      writeTag();
      compileExpressionList();
      writeTag();
    } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
               tokenizer.symbol() == '(') {
      writeTag();
      compileExpressionList();
      writeTag();
    } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
               tokenizer.symbol() == '[') {
      writeTag();
      compileExpression();
      writeTag();
    }
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
             tokenizer.symbol() == '(') {
    writeTag();
    compileExpression();
    writeTag();
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
             (tokenizer.symbol() == '-' || tokenizer.symbol() == '~')) {
    writeTag();
    compileTerm();
  }

  else
    writeTag();
  output << "</term>";
}

int compilationEngine::compileExpressionList() {
  output << "<expressionList>";
  if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
      tokenizer.symbol() == ')') {

    output << "</expressionList>";
    return 0;
  }

  compileExpression();
  size_t counter = 1;
  while (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
         tokenizer.symbol() == ',') {
    writeTag();
    compileExpression();
    counter++;
  }
  output << "</expressionList>";
  return counter;
}

void compilationEngine::writeTag() {
  if (tokenizer.tokenType() == jackTokenizer::Token::Keyword) {
    output << "<keyword>" << tokenizer.keywordString() << "</keyword>";
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol) {
    output << "<symbol>";

    if (tokenizer.symbol() == '<')
      output << "&lt;";
    else if (tokenizer.symbol() == '>')
      output << "&gt;";
    else if (tokenizer.symbol() == '&')
      output << "&amp;";
    else
      output << tokenizer.symbol();

    output << "</symbol>";
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Identifier) {
    output << "<identifier>" << tokenizer.identifier() << "</identifier>";
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Int_Const) {
    output << "<integerConstant>" << tokenizer.intVal() << "</integerConstant>";
  } else if (tokenizer.tokenType() == jackTokenizer::Token::String_Const) {
    output << "<stringConstant>" << tokenizer.stringVal()
           << "</stringConstant>";
  }
  if (tokenizer.hasMoreTokens())
    tokenizer.advance();
}
