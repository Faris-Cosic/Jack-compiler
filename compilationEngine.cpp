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
  output << "<class>\n";
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

  output << "</class>\n";
}

void compilationEngine::compileClassVarDec() {
  output << "<classVarDec>\n";
  compileVar();
  output << "</classVarDec>\n";
}

void compilationEngine::compileSubroutine() {
  output << "<subroutineDec>\n";
  writeTag(); // function type
  writeTag(); // return type
  writeTag(); // function name
  writeTag(); // (
  compileParameterList();
  writeTag(); // )
  compileSubroutineBody();
  output << "</subroutineDec>\n";
}

void compilationEngine::compileParameterList() {
  output << "<parameterList>\n";
  while (!(tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
           tokenizer.symbol() == ')')) {
    writeTag();
  }
  output << "</parameterList>\n";
}

void compilationEngine::compileSubroutineBody() {
  output << "<subroutineBody>\n";
  writeTag(); // {
  while (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
         tokenizer.keyword() == jackTokenizer::Keyword::Var) {
    compileVarDec();
  }
  compileStatements();
  writeTag(); // }
  output << "</subroutineBody>\n";
}

void compilationEngine::compileVarDec() {
  output << "<varDec>\n";
  compileVar();
  output << "</varDec>\n";
}

void compilationEngine::compileStatements() {
  output << "<statements>\n";
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
  output << "</statements>\n";
}

void compilationEngine::compileLet() {
  output << "<letStatement>\n";
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
  output << "</letStatement>\n";
}

void compilationEngine::compileIf() {
  output << "<ifStatement>\n";
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
  output << "</ifStatement>\n";
}

void compilationEngine::compileWhile() {
  output << "<whileStatement>\n";
  writeTag();
  writeTag();
  compileExpression();
  writeTag();
  writeTag();
  compileStatements();
  writeTag();
  output << "</whileStatement>\n";
}

void compilationEngine::compileDo() {
  output << "<doStatement>\n";
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
  output << "</doStatement>\n";
}

void compilationEngine::compileReturn() {
  output << "<returnStatement>\n";
  writeTag();
  if (!(tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
        tokenizer.symbol() == ';'))
    compileExpression();
  writeTag();
  output << "</returnStatement>\n";
}

void compilationEngine::compileExpression() {
  output << "<expression>\n";
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
  output << "</expression>\n";
}

void compilationEngine::compileTerm() {
  output << "<term>\n";
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
  output << "</term>\n";
}

int compilationEngine::compileExpressionList() {
  output << "<expressionList>\n";
  if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
      tokenizer.symbol() == ')') {

    output << "</expressionList>\n";
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
  output << "</expressionList>\n";
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
  output << std::endl;
  if (tokenizer.hasMoreTokens())
    tokenizer.advance();
}
