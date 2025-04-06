/*******************************************************************
 *                                             Peach-E v0.0.1
 *                           Created by Ranyodh Mandur - � 2024
 *
 *                         Licensed under the MIT License (MIT).
 *                  For more details, see the LICENSE file or visit:
 *                        https://opensource.org/licenses/MIT
 *
 *                         Peach-E is an open-source game engine
********************************************************************/
#pragma once

#include <algorithm>
#include <cctype>

#include "Managers/LogManager.h"

/// Magic

#define SERIALIZABLE_FIELDS(...) \
	template <typename F> \
	void visit(F&& f) const { f(__VA_ARGS__); } \
	static constexpr const char* field_names[] = { #__VA_ARGS__ };

template<typename>
inline constexpr bool always_false_v = false;

/// back to reality >W<

using namespace std;

namespace PeachCore {

	struct Serializer
	{
	public:
		Serializer() = default;
		~Serializer() = default;

		struct JSON; //c++ is a dumb fucking language

	public:
		bool
			ReadJSON
			(
				const string& fp_FilePath,
				JSON& fp_JSON,
				LogManager* logger
			)
		{
			string f_JsonString;
			vector<Token> f_TokenizedJson;

			if (not ReadJSONIntoString(fp_FilePath, &f_JsonString, logger))
			{
				logger->LogAndPrint("Failed to Read JSON", "ReadJSON", LogManager::LogLevel::Error);
				return false;
			}
			else if (not Tokenize(f_TokenizedJson, f_JsonString, logger))
			{
				logger->LogAndPrint("Failed to Lex JSON", "Tokenize", LogManager::LogLevel::Error);
				return false;
			}
			else if (not ParseJSON(f_TokenizedJson, fp_JSON, logger))
			{
				logger->LogAndPrint("Failed to Parse JSON", "ParseJSON", LogManager::LogLevel::Error);
				return false;
			}

			return true;
		}

	private:
		//////////////////////////////////////////////
		// Token and Token-type Definition
		//////////////////////////////////////////////

		enum class TokenType
		{
			//////////////////// GOATS ////////////////////

			IntLiteral,
			FloatLiteral,
			StringLiteral,
			NullLiteral,
			BoolLiteral,

			//////////////////// Bracket Types ////////////////////

			OpenBracket,
			CloseBracket,

			OpenSquareBracket,
			CloseSquareBracket,

			//////////////////// Symbols ////////////////////

			DoubleDot,
			Comma,

			//////////////////// End Of File ////////////////////

			ENDF
		};

		struct Token
		{
			string m_Value;
			TokenType m_Type;
			int m_SourceCodeLineNumber;

			explicit Token(const string& fp_Value, const TokenType fp_Type, const int fp_SourceCodeLineNumber)
			{
				m_Value = fp_Value;
				m_Type = fp_Type;
				m_SourceCodeLineNumber = fp_SourceCodeLineNumber;
			}

			explicit Token(const char& fp_Value, const TokenType fp_Type, const int fp_SourceCodeLineNumber)
			{
				m_Value = fp_Value;
				m_Type = fp_Type;
				m_SourceCodeLineNumber = fp_SourceCodeLineNumber;
			}
		};

		//////////////////////////////////////////////
		// Utility Functions
		//////////////////////////////////////////////

		[[nodiscard]] char
			ShiftForward(string & fp_Src)
		{
			if (fp_Src.empty())
			{
				return '\0';
			}

			char _c = fp_Src[0];
			fp_Src.erase(fp_Src.begin());

			return _c;
		}

		//////////////////////////////////////////////
		// Tokenize Function
		//////////////////////////////////////////////

		bool
			Tokenize
			(
				vector<Token>& fp_Tokens,
				string& fp_SourceCode,
				LogManager* logger
			)
		{
			size_t f_CurrentLineNumber = 1;

			char f_CurrentChar;

			bool f_ShouldShift = true;
			bool f_IsCurrentlyInsideComment = false;

			while (fp_SourceCode.size() > 0)
			{
				//////////////////// Iterate Current Character ////////////////////

				if (f_ShouldShift)
				{
					f_CurrentChar = ShiftForward(fp_SourceCode);
				}
				else
				{
					f_ShouldShift = true; //reset , only triggered for once loop iteration since while loops always step one character over their functioning bounds
				}

				//////////////////// Handle Spaces, New-Lines, and Comments ////////////////////

				if (f_CurrentChar == '\n') //used to keep track of what line number we're at in the source code, we only have single line comments, so this is sufficient
				{
					f_IsCurrentlyInsideComment = false;
					f_CurrentLineNumber++;
					continue; //we can shift forwards confidently since we're currently on the newline character
				}
				else if (f_IsCurrentlyInsideComment or isspace(f_CurrentChar))
				{
					continue;
				}

				//////////////////// Handle Digits or Alphabetic Characters ////////////////////

				if (isdigit(f_CurrentChar) or f_CurrentChar == '-') //used for finding floats and ints defined inside the JSON
				{
					string f_Number; // >w<

					if (f_CurrentChar == '-')
					{
						f_Number += '-';
						f_CurrentChar = ShiftForward(fp_SourceCode);

						if (not isdigit(f_CurrentChar))
						{
							logger->LogAndPrint(format("Unexpected symbol following character: '-', looks like you've input a non-numeric symbol: '{}' while defining a negative number at line number: {}", f_CurrentChar, f_CurrentLineNumber), "Lexer", LogManager::LogLevel::Error);
							fp_SourceCode.clear(); //dump the source code vector, so that the compiler will stop processing the source code
							return false;
						}
					}

					while (fp_SourceCode.size() > 0 and isdigit(f_CurrentChar))
					{
						f_Number += f_CurrentChar;
						f_CurrentChar = ShiftForward(fp_SourceCode); //shift to next character
					}

					if (f_CurrentChar == '.') //used for handling decimal numbers eg. "let x->float = 3.14;"
					{
						f_Number += f_CurrentChar; //add the decimal so we're at: "69. (rest to be parsed)" currently
						f_CurrentChar = ShiftForward(fp_SourceCode); //shift to next character

						if (not isdigit(f_CurrentChar))
						{
							logger->LogAndPrint(format("Unexpected symbol following a '.' brother!, looks like you've input a non-numeric symbol: '{}' while defining a decimal number at line number: {}", f_CurrentChar, f_CurrentLineNumber), "Lexer", LogManager::LogLevel::Error);
							fp_SourceCode.clear(); //dump the source code vector, so that the compiler will stop processing the source code
							return false;
						}

						while (fp_SourceCode.size() > 0 and isdigit(f_CurrentChar))
						{
							f_Number += f_CurrentChar;
							f_CurrentChar = ShiftForward(fp_SourceCode); //shift to next character
						}
						//push a float
						fp_Tokens.emplace_back(f_Number, TokenType::FloatLiteral, f_CurrentLineNumber); //No need for a continue here since the current character isnt a digit
					}
					else
					{	//push an int
						fp_Tokens.emplace_back(f_Number, TokenType::IntLiteral, f_CurrentLineNumber); //No need for a continue here since the current character isnt a digit
					}

					f_ShouldShift = false; //ensures we don't skip any crucial branch-logic for the over-stepped character
					continue; //move to next iteration
				}
				else if (isalpha(f_CurrentChar)) //used for finding bools and null literals inside the JSON
				{
					string f_Identifier; //start with NOTHING

					while (fp_SourceCode.size() > 0 and isalpha(f_CurrentChar))
					{
						f_Identifier += f_CurrentChar;
						f_CurrentChar = ShiftForward(fp_SourceCode); //shift to next character, this will overstep a character as an exit condition for the while-loop
					}

					if (f_Identifier == "true" or f_Identifier == "false")
					{
						fp_Tokens.emplace_back(f_Identifier, TokenType::BoolLiteral, f_CurrentLineNumber);
					}
					else if (f_Identifier == "null")
					{
						fp_Tokens.emplace_back(f_Identifier, TokenType::NullLiteral, f_CurrentLineNumber);
					}
					else
					{
						logger->LogAndPrint(format("Lexing Error: Invalid JSON identifier: '{}', found at line number: {}", f_Identifier, f_CurrentLineNumber), "Lexer", LogManager::LogLevel::Error);
						fp_SourceCode.clear();
						return false;
					}

					f_ShouldShift = false; //ensures we don't skip any crucial branch-logic for the over-stepped character
					continue; //move to next iteration since
				}

				switch (f_CurrentChar)
				{
				case ':':
					fp_Tokens.emplace_back(f_CurrentChar, TokenType::DoubleDot, f_CurrentLineNumber);
					break;
				case ',':
					fp_Tokens.emplace_back(f_CurrentChar, TokenType::Comma, f_CurrentLineNumber);
					break;

				case '{':
					fp_Tokens.emplace_back(f_CurrentChar, TokenType::OpenBracket, f_CurrentLineNumber);
					break;
				case '}':
					fp_Tokens.emplace_back(f_CurrentChar, TokenType::CloseBracket, f_CurrentLineNumber);
					break;

				case '[':
					fp_Tokens.emplace_back(f_CurrentChar, TokenType::OpenSquareBracket, f_CurrentLineNumber);
					break;
				case ']':
					fp_Tokens.emplace_back(f_CurrentChar, TokenType::CloseSquareBracket, f_CurrentLineNumber);
					break;

				case '.':
				{
					f_CurrentChar = ShiftForward(fp_SourceCode); //shift forward and look for a number definition

					if (not isdigit(f_CurrentChar))
					{
						logger->LogAndPrint(format("Lexing Error: Invalid JSON identifier: '{}', found at line number: {}", f_CurrentChar, f_CurrentLineNumber), "Lexer", LogManager::LogLevel::Error);
						fp_SourceCode.clear();
						return false;
					}

					string f_Number = "0."; // >w<

					while (fp_SourceCode.size() > 0 and isdigit(f_CurrentChar))
					{
						f_Number += f_CurrentChar;
						f_CurrentChar = ShiftForward(fp_SourceCode); //shift to next character
					}

					fp_Tokens.emplace_back(f_Number, TokenType::FloatLiteral, f_CurrentLineNumber); //push a float
				}
				f_ShouldShift = false; //ensures we don't skip any crucial branch-logic for the over-stepped character
				break;

				case '"': //VERY IMPORTANT THAT WE PROCESS THIS BEFORE '/' otherwise '/' mentioned inside of strings might be ignored
				{
					string f_CurrentStringLiteral = "";

					// Shift to the next character to start capturing the string, not the opening quote
					f_CurrentChar = ShiftForward(fp_SourceCode);

					bool f_IsEscapeCharacter = false;

					while (fp_SourceCode.size() > 0 and f_CurrentChar != '"')
					{
						if (f_CurrentChar == '\\')
						{
							f_CurrentChar = ShiftForward(fp_SourceCode); //shift to next character

							switch (f_CurrentChar)
							{
								case 'n':
									f_CurrentStringLiteral += '\n'; // Add a newline character
									break;
								case 't':
									f_CurrentStringLiteral += '\t'; // Add a tab character
									break;
								case '\\':
									f_CurrentStringLiteral += '\\'; // Add a literal backslash
									break;
								case '"':
									f_CurrentStringLiteral += '"'; // Add a literal double quote
									break;
								default:
									// Handle unknown escape sequences or add a fallback behavior
									f_CurrentStringLiteral += '\\'; // Re-add the backslash as it was part of the input
									f_CurrentStringLiteral += f_CurrentChar; // Add the unknown character as is
									break;
							}
						}
						else
						{
							f_CurrentStringLiteral += f_CurrentChar;
						}

						f_CurrentChar = ShiftForward(fp_SourceCode); //shift to next character
					}

					// Check if we've ended on the closing quotation mark
					if (f_CurrentChar == '"')
					{
						// Push the final string token without the quotes
						fp_Tokens.emplace_back(f_CurrentStringLiteral, TokenType::StringLiteral, f_CurrentLineNumber);
						continue; //move to next iteration, lexer will shift on next iteration, f_CurrentChar is pointing -> ' " ' 
					}
					else // Handle error: Unterminated string literal, and exit program execution
					{
						logger->LogAndPrint("Unterminated string literal, brother! Error occured at line number: " + to_string(f_CurrentLineNumber), "Lexer", LogManager::LogLevel::Error);
						fp_SourceCode.clear();
						return false;
					}
				}
				break;
				default:
					logger->LogAndPrint(format("Lexing Error: Unrecognized character found: [{}], found at line number: {}", f_CurrentChar, f_CurrentLineNumber), "Lexer", LogManager::LogLevel::Error);
					fp_SourceCode.clear(); //dump the source code vector, so that the compiler will stop processing the source code
					return false;
				}

			}

			fp_Tokens.emplace_back("", TokenType::ENDF, f_CurrentLineNumber); //label the end of the file i guess for some reason

			return true; //fuck C++
		}

		//////////////////////////////////////////////
		// Parsing
		//////////////////////////////////////////////

		struct JSONValue;

		using JSONObject = unordered_map<string, JSONValue>;
		using JSONArray = vector<JSONValue>;

		struct JSONValue
		{
			enum class Type
			{
				Object,
				Array,
				String,
				Integer,
				Float,
				Boolean,
				Null
			} JSONType;

			variant<JSONObject, JSONArray, string, int64_t, double, bool> m_Value;

			JSONValue() : JSONType(Type::Null), m_Value(false) {}

			explicit JSONValue(JSONObject __obj) : JSONType(Type::Object), m_Value(move(__obj)) {}
			explicit JSONValue(JSONArray __arr) : JSONType(Type::Array), m_Value(move(__arr)) {}
			explicit JSONValue(string __str) : JSONType(Type::String), m_Value(move(__str)) {}
			explicit JSONValue(int64_t __i) : JSONType(Type::Integer), m_Value(__i) {}
			explicit JSONValue(double __d) : JSONType(Type::Float), m_Value(__d) {}
			explicit JSONValue(bool __b) : JSONType(Type::Boolean), m_Value(__b) {}
		};

		//////////////////////////////////////////////
		// JSON interface
		//////////////////////////////////////////////
	public:
		struct JSON
		{
			JSON() = default;
			JSON(const JSONValue& root) : m_Root(root) {}

			JSONValue m_Root;

			JSON 
				operator[](const string& key) 
				const
			{
				if (m_Root.JSONType != JSONValue::Type::Object)
				{
					throw runtime_error("Accessed key on non-object");
				}

				const auto& obj = get<JSONObject>(m_Root.m_Value);
				auto it = obj.find(key);

				if (it == obj.end())
				{
					throw runtime_error("Key not found: " + key);
				}

				return JSON{ it->second };
			}

			JSON 
				operator[](size_t index) 
				const
			{
				if (m_Root.JSONType != JSONValue::Type::Array)
				{
					throw runtime_error("Attempted array access on non-array");
				}

				const auto& arr = get<JSONArray>(m_Root.m_Value);

				if (index >= arr.size())
				{
					throw out_of_range("Array index out of bounds: " + to_string(index));
				}

				return JSON{ arr[index] };
			}

			void
				PrintToConsole()
				const
			{
				PrintJSON(m_Root);
			}

			bool
				Has(const string& key)
				const
			{
				if (m_Root.JSONType != JSONValue::Type::Object)
				{
					return false;
				}

				const auto& obj = get<JSONObject>(m_Root.m_Value);
				return obj.find(key) != obj.end();
			}

			size_t
				Size()
				const
			{
				if (m_Root.JSONType == JSONValue::Type::Array)
				{
					return get<JSONArray>(m_Root.m_Value).size();
				}
				if (m_Root.JSONType == JSONValue::Type::Object)
				{
					return get<JSONObject>(m_Root.m_Value).size();
				}

				return 0;
			}

			bool
				Empty()
				const
			{
				return Size() == 0;
			}

			//auto Begin()
			//{
			//	if (m_Root.JSONType == JSONValue::Type::Array)
			//		return get<JSONArray>(m_Root.m_Value).begin();
			//	if (m_Root.JSONType == JSONValue::Type::Object)
			//		return get<JSONObject>(m_Root.m_Value).begin();
			//	throw runtime_error("Iteration on non-iterable JSON type");
			//}

			//auto end() 
			//{
			//	if (m_Root.JSONType == JSONValue::Type::Array)
			//		return get<JSONArray>(m_Root.m_Value).end();
			//	if (m_Root.JSONType == JSONValue::Type::Object)
			//		return get<JSONObject>(m_Root.m_Value).end();
			//	throw runtime_error("Iteration on non-iterable JSON type");
			//}

			template<typename T>
			operator
				T()
				const
			{
				if constexpr (is_same_v<T, string>)
				{
					if (m_Root.JSONType != JSONValue::Type::String)
					{
						throw bad_cast();
					}

					return get<string>(m_Root.m_Value);
				}
				else if constexpr (is_same_v<T, int64_t>)
				{
					if (m_Root.JSONType != JSONValue::Type::Integer)
					{
						throw bad_cast();
					}

					return get<int64_t>(m_Root.m_Value);
				}
				else if constexpr (is_same_v<T, double>)
				{
					if (m_Root.JSONType != JSONValue::Type::Float)
					{
						throw bad_cast();
					}

					return get<double>(m_Root.m_Value);
				}
				else if constexpr (is_same_v<T, bool>)
				{
					if (m_Root.JSONType != JSONValue::Type::Boolean)
					{
						throw bad_cast();
					}

					return get<bool>(m_Root.m_Value);
				}
				else
				{
					static_assert(sizeof(T) == 0, "Unsupported type for JSONValue conversion");
				}
			}

			JSON(initializer_list<pair<string, JSON>> list)
			{
				JSONObject obj;

				for (const auto& [k, v] : list)
				{
					obj[k] = v.m_Root;
				}

				m_Root = JSONValue(move(obj));
			}

			JSON(initializer_list<JSON> list)
			{
				JSONArray arr;

				for (const auto& val : list)
				{
					arr.push_back(val.m_Root);
				}

				m_Root = JSONValue(move(arr));
			}

			template<typename T, typename = enable_if_t<is_arithmetic_v<T> || is_same_v<T, string>>>
			JSON(const T& val)
			{
				if constexpr (is_same_v<T, string>)
				{
					m_Root = JSONValue(val);
				}
				else if constexpr (is_integral_v<T>)
				{
					m_Root = JSONValue(static_cast<int64_t>(val));
				}
				else if constexpr (is_floating_point_v<T>)
				{
					m_Root = JSONValue(static_cast<double>(val));
				}
				else
				{
					static_assert(always_false_v<T>, "Unsupported type for JSON conversion");
				}
			}
		};

		static vector<string> 
			SplitFieldNames(const char* raw)
		{
			string src = raw;
			vector<string> result;

			size_t start = 0;
			while (true)
			{
				size_t comma = src.find(',', start);
				string token = src.substr(start, comma - start);

				// Trim leading/trailing whitespace
				token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());

				if (!token.empty())
					result.push_back(token);

				if (comma == string::npos) break;
				start = comma + 1;
			}

			return result;
		}

		template<typename T>
		JSON
			ToJSON(const T& obj) //IT WORKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKSSS IM SO TIRED >w< ;w; i sleep like a champion tn
		{
			JSONObject f_Object; // ✅ this is what i was missin UwU

			const vector<string> fieldNames = SplitFieldNames(T::field_names[0]);

			size_t i = 0;

			obj.visit([&](auto&&... fields) { 
				((f_Object.emplace(fieldNames[i++], fields)), ...);
				});

			return JSON(JSONValue(f_Object));
		}

	private:
		//////////////////////////////////////////////
		// Parsing Utilities
		//////////////////////////////////////////////

		[[nodiscard]] Token
			ShiftForward(vector<Token>&fp_TokenVector)
		{
			if (fp_TokenVector.empty())
			{
				return Token("\0", TokenType::ENDF, -1); //return escape char when source code is done being read
			}

			Token f_FirstElement = fp_TokenVector.front();
			fp_TokenVector.erase(fp_TokenVector.begin());

			return f_FirstElement;
		}

		//////////////////////////////////////////////
		// Parsing Functions
		//////////////////////////////////////////////

		bool
			ParseObject
			(
				vector<Token>&fp_Tokens,
				JSONObject& fp_JSONObject, //current list containing the entire parsed JSON up to this point
				LogManager* logger
			)
		{
			string f_CurrentKey;

			Token f_CurrentToken = ShiftForward(fp_Tokens); //shift forwards one and check for a string key, assuming the last token was '{'

			while (f_CurrentToken.m_Type != TokenType::CloseBracket)
			{
				if (f_CurrentToken.m_Type != TokenType::StringLiteral)
				{
					logger->LogAndPrint(format("Parsing Error: found '{}', when string literal was expected as JSON key inside object at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseObject", LogManager::LogLevel::Error);
					return false;
				}

				f_CurrentKey = move(f_CurrentToken.m_Value);
				f_CurrentToken = ShiftForward(fp_Tokens); //look for ':'

				if (f_CurrentToken.m_Type != TokenType::DoubleDot)
				{
					logger->LogAndPrint(format("Parsing Error: found '{}', when ':' was expected after JSON key inside object at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseObject", LogManager::LogLevel::Error);
					return false;
				}

				f_CurrentToken = ShiftForward(fp_Tokens); //look for value associated with key

				if (not ParseValue(f_CurrentToken, fp_JSONObject, f_CurrentKey, fp_Tokens, logger))
				{
					logger->LogAndPrint(format("Parsing Error: Invalid JSON object: '{}', at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseObject", LogManager::LogLevel::Error);
					return false;
				}

				f_CurrentToken = ShiftForward(fp_Tokens); //look for comma or close bracket

				if (f_CurrentToken.m_Type == TokenType::CloseBracket)
				{
					break;
				}

				if (f_CurrentToken.m_Type != TokenType::Comma)
				{
					logger->LogAndPrint(format("Parsing Error: found '{}', when ',' was expected after JSON value inside object at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseObject", LogManager::LogLevel::Error);
					return false;
				}

				f_CurrentToken = ShiftForward(fp_Tokens); // consume comma, and look for next key value pair
			}

			if (f_CurrentToken.m_Type != TokenType::CloseBracket)
			{
				logger->LogAndPrint(format("Parsing Error: Unexpected token: [{}], found inside array definition at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseObject", LogManager::LogLevel::Error);
				return false;
			}

			return true;
		}

		bool
			ParseArray
			(
				vector<Token>&fp_Tokens,
				JSONArray& fp_JSONArray, //current list containing the entire parsed JSON up to this point
				LogManager* logger
			)
		{
			Token f_CurrentToken = ShiftForward(fp_Tokens); //assuming the most recent token was '[' called from ParseJSON

			while (f_CurrentToken.m_Type != TokenType::CloseSquareBracket)
			{
				if (not ParseValue(f_CurrentToken, fp_JSONArray, fp_Tokens, logger))
				{
					logger->LogAndPrint("Parsing Error: invalid value found while parsing an Array", "ParseArray", LogManager::LogLevel::Error);
					return false;
				}

				f_CurrentToken = ShiftForward(fp_Tokens); //shift to find comma

				if (f_CurrentToken.m_Type == TokenType::CloseSquareBracket)
				{
					break;
				}

				if (f_CurrentToken.m_Type != TokenType::Comma)
				{
					logger->LogAndPrint(format("Parsing Error: expected ',' after value inside JSON array but found '{}' instead at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseArray", LogManager::LogLevel::Error);
					return false;
				}

				f_CurrentToken = ShiftForward(fp_Tokens); //shift past the comma to find the next value
			}

			if (f_CurrentToken.m_Type != TokenType::CloseSquareBracket)
			{
				logger->LogAndPrint(format("Parsing Error: Expected ']' but found '{}' instead, found inside array definition at line number: {}", f_CurrentToken.m_Value, f_CurrentToken.m_SourceCodeLineNumber), "ParseArray", LogManager::LogLevel::Error);
				return false;
			}

			return true;
		}

		bool
			ParseValue
			(
				Token fp_CurrentToken,
				JSONArray& fp_Array,
				vector<Token>& fp_Tokens,
				LogManager* logger
			)
		{
			switch (fp_CurrentToken.m_Type)
			{
				case TokenType::StringLiteral:
					fp_Array.emplace_back(fp_CurrentToken.m_Value);
					break;
				case TokenType::IntLiteral:
					fp_Array.emplace_back(stoll(fp_CurrentToken.m_Value));
					break;
				case TokenType::FloatLiteral:
					fp_Array.emplace_back(stod(fp_CurrentToken.m_Value));
					break;
				case TokenType::BoolLiteral:
					fp_Array.emplace_back(fp_CurrentToken.m_Value == "true");
					break;
				case TokenType::NullLiteral:
					fp_Array.emplace_back(); //lmfao this looks so dumb but works
					break;

				case TokenType::OpenBracket:
				{
					JSONObject f_TempObject;
					ParseObject(fp_Tokens, f_TempObject, logger);
					fp_Array.emplace_back(f_TempObject);
				}
				break;
				case TokenType::OpenSquareBracket:
				{
					JSONArray f_TempArray;
					ParseArray(fp_Tokens, f_TempArray, logger);
					fp_Array.emplace_back(f_TempArray);
				}
				break;
				default:
					logger->LogAndPrint(format("Parsing Error: found '{}' inside array, when integral type was expected at line number: {}", fp_CurrentToken.m_Value, fp_CurrentToken.m_SourceCodeLineNumber), "ParseValue", LogManager::LogLevel::Error);
					return false;
			}

			return true;
		}

		bool
			ParseValue
			(
				Token fp_CurrentToken,
				JSONObject& fp_JSONObject,
				string& fp_ValueKey,
				vector<Token>& fp_Tokens,
				LogManager* logger
			)
		{
			switch (fp_CurrentToken.m_Type)
			{
				case TokenType::StringLiteral:
					fp_JSONObject.emplace(fp_ValueKey, fp_CurrentToken.m_Value);
					break;
				case TokenType::IntLiteral:
					fp_JSONObject.emplace(fp_ValueKey, stoll(fp_CurrentToken.m_Value));
					break;
				case TokenType::FloatLiteral:
					fp_JSONObject.emplace(fp_ValueKey, stod(fp_CurrentToken.m_Value));
					break;
				case TokenType::BoolLiteral:
					fp_JSONObject.emplace(fp_ValueKey, fp_CurrentToken.m_Value == "true");
					break;
				case TokenType::NullLiteral:
					fp_JSONObject.emplace(fp_ValueKey, JSONValue());
					break;

				case TokenType::OpenBracket:
				{
					JSONObject f_TempObject;
					ParseObject(fp_Tokens, f_TempObject, logger);
					fp_JSONObject.emplace(fp_ValueKey, f_TempObject);
				}
				break;
				case TokenType::OpenSquareBracket:
				{
					JSONArray f_TempArray;
					ParseArray(fp_Tokens, f_TempArray, logger);
					fp_JSONObject.emplace(fp_ValueKey, f_TempArray);
				}
				break;
				default:
					logger->LogAndPrint(format("Parsing Error: found '{}' inside object, when integral type was expected at line number: {}", fp_CurrentToken.m_Value, fp_CurrentToken.m_SourceCodeLineNumber), "ParseValue", LogManager::LogLevel::Error);
					return false;
			}

			return true;
		}

		bool
			ParseJSON
			(
				vector<Token>& fp_Tokens,
				JSON& fp_JSON,
				LogManager* logger
			)
		{
			Token f_CurrentToken = ShiftForward(fp_Tokens); //get first val

			switch (f_CurrentToken.m_Type) //should only need to do this once for a valid JSON
			{
			case TokenType::OpenBracket:
			{
				JSONObject f_Object;
				ParseObject(fp_Tokens, f_Object, logger);
				fp_JSON.m_Root = move(JSONValue(f_Object));
			}
			break;
			case TokenType::OpenSquareBracket:
			{
				JSONArray f_Array;
				ParseArray(fp_Tokens, f_Array, logger);
				fp_JSON.m_Root = move(JSONValue(f_Array));
			}
			break;
			default:
				logger->LogAndPrint("Parsing Error: ill-formed JSON found, parsing failed", "ParseJSON", LogManager::LogLevel::Error);
				return false;
			}

			f_CurrentToken = ShiftForward(fp_Tokens); //check for ENDF

			if (f_CurrentToken.m_Type != TokenType::ENDF)
			{
				logger->LogAndPrint("Parsing Error: parser failed to find end of file, something bad happened and I have 0 clue why lmfao. JSONValue isn't properly formed", "ParseJSON", LogManager::LogLevel::Error);
				fp_JSON.m_Root = JSONValue();
				return false;
			}

			return true;
		}

		//////////////////////////////////////////////
		// Print JSON to Console
		//////////////////////////////////////////////

		static void
			PrintJSON(const JSONValue& value, int indent = 0)
		{
			string spacing(indent, ' ');

			switch (value.JSONType)
			{
			case JSONValue::Type::Null:
				cout << spacing << "null" << endl;
				break;
			case JSONValue::Type::Boolean:
				cout << spacing << (get<bool>(value.m_Value) ? "true" : "false") << endl;
				break;
			case JSONValue::Type::Integer:
				cout << spacing << get<int64_t>(value.m_Value) << endl;
				break;
			case JSONValue::Type::Float:
				cout << spacing << get<double>(value.m_Value) << endl;
				break;
			case JSONValue::Type::String:
				cout << spacing << "\"" << get<string>(value.m_Value) << "\"" << endl;
				break;
			case JSONValue::Type::Array:
			{
				cout << spacing << "[" << endl;
				for (const auto& v : get<JSONArray>(value.m_Value))
				{
					PrintJSON(v, indent + 2);
				}
				cout << spacing << "]" << endl;
				break;
			}
			case JSONValue::Type::Object:
			{
				cout << spacing << "{" << endl;
				const auto& obj = get<JSONObject>(value.m_Value);

				for (auto it = obj.begin(); it != obj.end(); ++it)
				{
					cout << spacing << "  \"" << it->first << "\": ";

					PrintJSON(it->second, indent + 2);

					if (next(it) != obj.end())
					{
						cout << ","; // comma after each element except the last
					}

					cout << endl;
				}

				cout << spacing << "}";
				break;
			}
			}
		}

		//////////////////////////////////////////////
		// JSON Read/Write Functions
		//////////////////////////////////////////////

		void
			WriteToJSON
			(
				const string& fp_DesiredOutputDirectory,
				const string& fp_DesiredName,
				const vector<uint8_t>& fp_ByteCode, //XXX: Nuke this and replace with proper json serialization
				LogManager* logger
			)
		{
			//logger->LogAndPrint("Bytecode size: " + to_string(fp_ByteCode.size()), "Compiler", "info", "cyan");

			if (fp_ByteCode.empty())
			{
				logger->LogAndPrint("Serialization Error: Failed to write " + fp_DesiredName + " ", "Serializer", LogManager::LogLevel::Error);
			}

			string f_FileName;

			if (fp_DesiredOutputDirectory == "./")
			{
				f_FileName = "./" + fp_DesiredName + ".json";
			}
			else
			{
				f_FileName = fp_DesiredOutputDirectory + "/" + fp_DesiredName + ".json";
			}

			ofstream file(f_FileName, ios::binary);  // Open in binary mode

			if (!file)
			{
				logger->LogAndPrint("Serialization Error: Failed to open " + f_FileName + " for writing.", "Serializer", LogManager::LogLevel::Error);
				return;
			}

			// Write the entire contents of the vector to the file
			file.write(reinterpret_cast<const char*>(fp_ByteCode.data()), fp_ByteCode.size());

			file.close();  // Close the file
		}

		bool
			ReadJSONIntoString
			(
				const string& fp_ScriptFilePath,
				string* fp_SourceCode,
				LogManager* logger
			)
		{
			if (not fp_SourceCode)
			{
				logger->LogAndPrint("Serialization Error: Nullptr reference passed to ReadJSONIntoString", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			// Extract file extension assuming format "filename.ext"
			size_t lastDotIndex = fp_ScriptFilePath.rfind('.');

			if (lastDotIndex == string::npos)
			{
				logger->LogAndPrint("Serialization Error: No file extension found", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			string f_FileExtension = fp_ScriptFilePath.substr(lastDotIndex);

			if (f_FileExtension != ".json")
			{
				logger->LogAndPrint("Serialization Error: Attempted to read from a file that isn't a JSON", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			ifstream f_FileStream(fp_ScriptFilePath);

			if (!f_FileStream)
			{
				logger->LogAndPrint("Serialization Error: Failed to open JSON for reading.", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			stringstream f_StringBuffer;
			f_StringBuffer << f_FileStream.rdbuf();
			*fp_SourceCode = f_StringBuffer.str();

			return true;
		}
	};
}



//uwu 1069