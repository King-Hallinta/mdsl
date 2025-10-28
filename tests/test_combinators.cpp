#include "../mdsl/include/mdsl/parser/Combinators.h"
#include "../mdsl/include/mdsl/parser/TokenStream.h"
#include "../mdsl/include/mdsl/lexer/Token.h"
#include "../mdsl/include/mdsl/core/SourceLocation.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace mdsl;
using namespace mdsl::parser;
using namespace mdsl::lexer;

void TestStreamToken()
{
	std::cout << "Testing streamToken..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Integer, "42", core::SourceSpan()),
		Token(TokenType::Plus, "+", core::SourceSpan()),
		Token(TokenType::Integer, "10", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);
	auto intParser = streamToken(TokenType::Integer);

	auto result = intParser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value().type == TokenType::Integer);
	assert(result.Value().lexeme == "42");

	auto plusParser = streamToken(TokenType::Plus);
	auto result2 = plusParser.Parse(stream);
	assert(result2.IsOk());
	assert(result2.Value().type == TokenType::Plus);

	std::cout << "  PASSED" << std::endl;
}

void TestOrElse()
{
	std::cout << "Testing orElse (alternatives)..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::String, "hello", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto intParser = streamToken(TokenType::Integer);
	auto strParser = streamToken(TokenType::String);
	auto combinedParser = intParser.orElse(strParser);

	auto result = combinedParser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value().type == TokenType::String);

	std::cout << "  PASSED" << std::endl;
}

void TestOrElseBacktracking()
{
	std::cout << "Testing orElse backtracking..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Integer, "42", core::SourceSpan()),
		Token(TokenType::Plus, "+", core::SourceSpan()),
		Token(TokenType::Integer, "10", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto failingParser = StreamParser<int>([](TokenStream& s) -> ParseResult<int>
	{
		auto tok1 = s.NextToken();
		if (tok1.type != TokenType::Integer)
		{
			return ParseResult<int>(core::Error(core::ErrorCode::ParserError, "Expected int"));
		}
		auto tok2 = s.NextToken();
		if (tok2.type != TokenType::Minus)
		{
			return ParseResult<int>(core::Error(core::ErrorCode::ParserError, "Expected minus"));
		}
		return ParseResult<int>(42);
	});

	auto successfulParser = StreamParser<int>([](TokenStream& s) -> ParseResult<int>
	{
		auto tok1 = s.NextToken();
		if (tok1.type != TokenType::Integer)
		{
			return ParseResult<int>(core::Error(core::ErrorCode::ParserError, "Expected int"));
		}
		auto tok2 = s.NextToken();
		if (tok2.type != TokenType::Plus)
		{
			return ParseResult<int>(core::Error(core::ErrorCode::ParserError, "Expected plus"));
		}
		return ParseResult<int>(100);
	});

	auto combinedParser = failingParser.orElse(successfulParser);
	auto result = combinedParser.Parse(stream);

	assert(result.IsOk());
	assert(result.Value() == 100);
	assert(stream.GetPosition() == 2);

	std::cout << "  PASSED" << std::endl;
}

void TestMany()
{
	std::cout << "Testing many (zero or more)..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Integer, "1", core::SourceSpan()),
		Token(TokenType::Integer, "2", core::SourceSpan()),
		Token(TokenType::Integer, "3", core::SourceSpan()),
		Token(TokenType::Plus, "+", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto intParser = streamToken(TokenType::Integer);
	auto manyParser = streamMany(intParser);

	auto result = manyParser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value().size() == 3);
	assert(result.Value()[0].lexeme == "1");
	assert(result.Value()[1].lexeme == "2");
	assert(result.Value()[2].lexeme == "3");

	auto nextToken = stream.NextToken();
	assert(nextToken.type == TokenType::Plus);

	std::cout << "  PASSED" << std::endl;
}

void TestManyEmpty()
{
	std::cout << "Testing many with no matches..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Plus, "+", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto intParser = streamToken(TokenType::Integer);
	auto manyParser = streamMany(intParser);

	auto result = manyParser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value().size() == 0);

	auto nextToken = stream.NextToken();
	assert(nextToken.type == TokenType::Plus);

	std::cout << "  PASSED" << std::endl;
}

void TestMany1()
{
	std::cout << "Testing many1 (one or more)..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Integer, "1", core::SourceSpan()),
		Token(TokenType::Integer, "2", core::SourceSpan()),
		Token(TokenType::Plus, "+", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto intParser = streamToken(TokenType::Integer);
	auto many1Parser = streamMany1(intParser);

	auto result = many1Parser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value().size() == 2);

	std::cout << "  PASSED" << std::endl;
}

void TestMany1Failure()
{
	std::cout << "Testing many1 failure (requires at least one)..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Plus, "+", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto intParser = streamToken(TokenType::Integer);
	auto many1Parser = streamMany1(intParser);

	auto result = many1Parser.Parse(stream);
	assert(result.IsError());

	std::cout << "  PASSED" << std::endl;
}

void TestOptional()
{
	std::cout << "Testing optional..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Minus, "-", core::SourceSpan()),
		Token(TokenType::Integer, "42", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto minusParser = streamToken(TokenType::Minus);
	Token defaultToken(TokenType::Plus, "+", core::SourceSpan());
	auto optionalParser = streamOptional(minusParser, defaultToken);

	auto result = optionalParser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value().type == TokenType::Minus);

	TokenStream stream2({
		Token(TokenType::Integer, "42", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	});

	auto result2 = optionalParser.Parse(stream2);
	assert(result2.IsOk());
	assert(result2.Value().type == TokenType::Plus);
	assert(stream2.GetPosition() == 0);

	std::cout << "  PASSED" << std::endl;
}

void TestSepBy()
{
	std::cout << "Testing sepBy (comma-separated list)..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Integer, "1", core::SourceSpan()),
		Token(TokenType::Comma, ",", core::SourceSpan()),
		Token(TokenType::Integer, "2", core::SourceSpan()),
		Token(TokenType::Comma, ",", core::SourceSpan()),
		Token(TokenType::Integer, "3", core::SourceSpan()),
		Token(TokenType::Semicolon, ";", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto intParser = streamToken(TokenType::Integer);
	auto commaParser = streamToken(TokenType::Comma);
	auto sepByParser = streamSepBy(intParser, commaParser);

	auto result = sepByParser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value().size() == 3);
	assert(result.Value()[0].lexeme == "1");
	assert(result.Value()[1].lexeme == "2");
	assert(result.Value()[2].lexeme == "3");

	auto nextToken = stream.NextToken();
	assert(nextToken.type == TokenType::Semicolon);

	std::cout << "  PASSED" << std::endl;
}

void TestSepByEmpty()
{
	std::cout << "Testing sepBy with empty list..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Semicolon, ";", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto intParser = streamToken(TokenType::Integer);
	auto commaParser = streamToken(TokenType::Comma);
	auto sepByParser = streamSepBy(intParser, commaParser);

	auto result = sepByParser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value().size() == 0);

	auto nextToken = stream.NextToken();
	assert(nextToken.type == TokenType::Semicolon);

	std::cout << "  PASSED" << std::endl;
}

void TestMap()
{
	std::cout << "Testing map (transform result)..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Integer, "42", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto intParser = streamToken(TokenType::Integer);
	std::function<int(const Token&)> mapper = [](const Token& tok) -> int
	{
		return std::stoi(tok.lexeme);
	};
	auto mappedParser = intParser.map<int>(mapper);

	auto result = mappedParser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value() == 42);

	std::cout << "  PASSED" << std::endl;
}

void TestSequence()
{
	std::cout << "Testing sequence (multiple parsers)..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Integer, "10", core::SourceSpan()),
		Token(TokenType::Plus, "+", core::SourceSpan()),
		Token(TokenType::Integer, "20", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto sequenceParser = StreamParser<int>([](TokenStream& s) -> ParseResult<int>
	{
		auto leftParser = streamToken(TokenType::Integer);
		auto left = leftParser.Parse(s);
		if (left.IsError())
		{
			return ParseResult<int>(left.GetError());
		}

		auto opParser = streamToken(TokenType::Plus);
		auto op = opParser.Parse(s);
		if (op.IsError())
		{
			return ParseResult<int>(op.GetError());
		}

		auto rightParser = streamToken(TokenType::Integer);
		auto right = rightParser.Parse(s);
		if (right.IsError())
		{
			return ParseResult<int>(right.GetError());
		}

		int leftVal = std::stoi(left.Value().lexeme);
		int rightVal = std::stoi(right.Value().lexeme);
		return ParseResult<int>(leftVal + rightVal);
	});

	auto result = sequenceParser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value() == 30);

	std::cout << "  PASSED" << std::endl;
}

void TestNestedAlternatives()
{
	std::cout << "Testing nested alternatives..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::String, "hello", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto intParser = streamToken(TokenType::Integer);
	auto floatParser = streamToken(TokenType::Float);
	auto strParser = streamToken(TokenType::String);

	auto numericParser = intParser.orElse(floatParser);
	auto anyLiteralParser = numericParser.orElse(strParser);

	auto result = anyLiteralParser.Parse(stream);
	assert(result.IsOk());
	assert(result.Value().type == TokenType::String);

	std::cout << "  PASSED" << std::endl;
}

void TestCheckpointRestore()
{
	std::cout << "Testing checkpoint restore..." << std::endl;

	std::vector<Token> tokens = {
		Token(TokenType::Integer, "1", core::SourceSpan()),
		Token(TokenType::Integer, "2", core::SourceSpan()),
		Token(TokenType::Integer, "3", core::SourceSpan()),
		Token(TokenType::EndOfFile, "", core::SourceSpan())
	};

	TokenStream stream(tokens);

	auto checkpoint1 = stream.SaveCheckpoint();
	stream.NextToken();
	stream.NextToken();

	auto checkpoint2 = stream.SaveCheckpoint();
	stream.NextToken();

	assert(stream.GetPosition() == 3);

	stream.RestoreCheckpoint(checkpoint2);
	assert(stream.GetPosition() == 2);

	stream.RestoreCheckpoint(checkpoint1);
	assert(stream.GetPosition() == 0);

	std::cout << "  PASSED" << std::endl;
}

int main()
{
	std::cout << "Running Parser Combinator Tests..." << std::endl << std::endl;

	TestStreamToken();
	TestOrElse();
	TestOrElseBacktracking();
	TestMany();
	TestManyEmpty();
	TestMany1();
	TestMany1Failure();
	TestOptional();
	TestSepBy();
	TestSepByEmpty();
	TestMap();
	TestSequence();
	TestNestedAlternatives();
	TestCheckpointRestore();

	std::cout << std::endl << "All tests passed!" << std::endl;
	return 0;
}
