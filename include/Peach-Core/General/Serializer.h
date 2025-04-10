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
	template <typename F> \
	void visit(F&& f) { f(__VA_ARGS__); } \
	static constexpr const char* field_names = { #__VA_ARGS__ };

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

			DoubleDot, // ':'
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
		public:
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
				UnsignedInteger,
				Float,
				Boolean,
				Null
			} JSONType;

			variant //idk im lazy and sick of using templates
			<
				JSONObject, 
				JSONArray, 
				string, 
				bool,

				int64_t, 
				uint64_t,
				double 
			> m_Value;

			JSONValue() : JSONType(Type::Null), m_Value(false) {}

			explicit JSONValue(JSONObject __obj) : JSONType(Type::Object), m_Value(move(__obj)) {}
			explicit JSONValue(JSONArray __arr) : JSONType(Type::Array), m_Value(move(__arr)) {}
			explicit JSONValue(string __str) : JSONType(Type::String), m_Value(move(__str)) {}
			explicit JSONValue(bool __b) : JSONType(Type::Boolean), m_Value(__b) {}

			template<typename I, enable_if_t<is_integral_v<I>&& is_signed_v<I>, int> = 0>
			JSONValue(I __i) : JSONType(Type::Integer), m_Value(static_cast<int64_t>(__i)) {}

			template<typename I, enable_if_t<is_integral_v<I>&& is_unsigned_v<I>, int> = 0>
			JSONValue(I __i) : JSONType(Type::UnsignedInteger), m_Value(static_cast<uint64_t>(__i)) {}

			JSONValue(double __f) : JSONType(Type::Float), m_Value(__f) {} //not explicit to implicitly cast float -> double
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

			auto 
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

				return it->second;
			}

			auto 
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

				return arr[index];
			}

			bool
				ToString(string* fp_JSONString) //kicks off recursive creation of JSON string
				const
			{
				if (not fp_JSONString)
				{

					return false;
				}

				stringstream f_TempString;

				if (not ToStringStream(m_Root, f_TempString))
				{

					return false;
				}

				*fp_JSONString = f_TempString.str();
				return true;
			}

			void
				ValueToString()
			{

			}

			bool
				ToStringStream
				(
					const JSONValue& fp_JSONValue,
					stringstream& fp_JSONString, 
					const uint32_t indent = 0,
					const bool fp_IsKeyValue = false
				)
				const
			{
				string f_IndentLevel(indent, ' ');

				switch (m_Root.JSONType)
				{
					case JSONValue::Type::Null:
						fp_JSONString << f_IndentLevel << "null";
						break;
					case JSONValue::Type::Boolean:
						fp_JSONString << f_IndentLevel << (get<bool>(m_Root.m_Value) ? "true" : "false");
						break;
					case JSONValue::Type::Integer:
						fp_JSONString << f_IndentLevel << get<int64_t>(m_Root.m_Value);
						break;
					case JSONValue::Type::Float:
						fp_JSONString << f_IndentLevel << get<double>(m_Root.m_Value);
						break;
					case JSONValue::Type::String:
						if(fp_IsKeyValue)
						{
							fp_JSONString << f_IndentLevel << get<string>(m_Root.m_Value) << "," << endl; //string as value
						}
						else
						{
							fp_JSONString << f_IndentLevel << get<string>(m_Root.m_Value) << ": "; //no endl and : for key
						}
						break;
					case JSONValue::Type::UnsignedInteger:
						fp_JSONString << f_IndentLevel << get<uint64_t>(m_Root.m_Value);
						break;
					case JSONValue::Type::Array:
					{
						fp_JSONString << f_IndentLevel << "[";
						string f_ListOffset = 0;
						for (const auto& v : get<JSONArray>(m_Root.m_Value))
						{
							//ToStringStream(v, 0);
						}
						fp_JSONString << f_IndentLevel << "]" << ", " << endl;
						break;
					}
					case JSONValue::Type::Object:
					{
						fp_JSONString << f_IndentLevel << "{" << endl;
						const auto& obj = get<JSONObject>(m_Root.m_Value);

						for (auto it = obj.begin(); it != obj.end(); ++it)
						{
							fp_JSONString << f_IndentLevel << "  \"" << it->first << "\": ";

							//ToStringStream(it->second, indent + 4);

							if (next(it) != obj.end())
							{
								fp_JSONString << ","; // comma after each element except the last
							}

							fp_JSONString << endl;
						}

						fp_JSONString << f_IndentLevel << "}";
						break;
					}
				}

				if (fp_IsKeyValue)
				{
					fp_JSONString << ", ";
				}

				return true;
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

		//////////////////////////////////////////////
		// Helper Function for field_names -> vector<string>
		//////////////////////////////////////////////

		inline vector<string> 
			SplitFieldNames(const string& raw) //used because field_names gets all member names stuffed into a single string
		{
			vector<string> result;
			string token;

			for (char c : raw)
			{
				if (c == ',')
				{
					if (not token.empty()) 
					{
						result.push_back(token);
					}
					token.clear();
				}
				else if (c != ' ')
				{
					token += c;
				}
			}

			if (not token.empty()) 
			{
				result.push_back(token);
			}

			return result;
		}

		//////////////////////////////////////////////
		// Helper Templates
		//////////////////////////////////////////////
		/*
		These templates are used for detecting data structs that have SERIALIZABLE_FIELDS implemented since serialization in this library completely relies on the defined functions
		provided by the macro to serialize data.

		As well as checking for map/vector structs since serializing them is pretty clean in JSON and honestly are used widely enough that not being able to serialize maps/structs feels
		like a major downside.
		*/

		template<typename T, typename = void>
		struct is_map : false_type {};

		template<typename T>
		struct is_map<T, void_t<
			typename T::key_type,
			typename T::mapped_type,
			decltype(declval<T>().begin()),
			decltype(declval<T>().end())
			>> : bool_constant<
			is_same_v<typename T::key_type, string>
			> {};

		template<typename T, typename = void>
		struct is_vector : false_type {};

		template<typename T>
		struct is_vector<T, void_t<
			typename T::value_type,
			decltype(declval<T>().begin()),
			decltype(declval<T>().end())
			>> : true_type {};

		template<typename T, typename = void>
		struct is_serializable_struct : false_type {};

		template<typename T>
		struct is_serializable_struct<T, void_t<decltype(T::field_names), decltype(declval<T>().visit(declval<void(*)(int)>()))>> : true_type {};

		//////////////////////////////////////////////
		// Main (De)Serialization Functions
		//////////////////////////////////////////////
		/*
		These functions should be used on POD structs, serializing more complex data types is fine, however serialization for basic types is only supported since I dont really see any reason
		for serializing more complex types, since it just amounts to serializing everything down to integral types since that's what computers at their foundation understand + string because
		strings are super common so we deal with those uwu.

		For any external types, just create a POD struct that encapsulates fields relevant to reconstructing the external type and assign them manually.

		This is the case since JSON expects and does things just fine using basic types + strings + lists/POD structs.
		*/

		template<typename T>
		enable_if_t<is_serializable_struct<T>::value, JSON>
			ToJSON(const T& obj) //IT WORKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKSSS IM SO TIRED >w< ;w; i sleep like a champion tn
		{
			JSONObject f_Object; // ✅ this is what i was missin UwU

			static_assert(is_serializable_struct<T>::value, "ToJSON() can only be used with types that use SERIALIZABLE_FIELDS");

			const vector<string> fieldNames = SplitFieldNames(T::field_names);

			size_t i = 0;

			obj.visit([&](auto&&... fields)
				{
					(
						[&]
						{
							const auto& field = fields;
							const auto& key = fieldNames[i++];

							if constexpr (is_arithmetic_v<decay_t<decltype(field)>> || is_same_v<decay_t<decltype(field)>, string>)
							{
								f_Object.emplace(key, field);
							}
							else if constexpr (is_serializable_struct<decay_t<decltype(field)>>::value)
							{
								f_Object.emplace(key, ToJSON(field).m_Root);
							}
							else if constexpr (is_map<decay_t<decltype(field)>>::value)
							{
								JSONObject mapObj;
								for (const auto& [mapKey, mapVal] : field)
								{
									if constexpr (is_serializable_struct<decay_t<decltype(mapVal)>>::value)
									{
										mapObj.emplace(mapKey, ToJSON(mapVal).m_Root);
									}
									else
									{
										mapObj.emplace(mapKey, mapVal);
									}
								}
								f_Object.emplace(key, mapObj);
							}
							else if constexpr (is_vector<decay_t<decltype(field)>>::value)
							{
								JSONArray arr;

								for (const auto& val : field)
								{
									if constexpr (is_serializable_struct<decay_t<decltype(val)>>::value)
									{
										arr.emplace_back(ToJSON(val).m_Root);
									}
									else
									{
										arr.emplace_back(val);
									}
								}

								f_Object.emplace(key, arr);
							}
							else
							{
								static_assert(always_false_v<decltype(field)>, "Unsupported field type in ToJSON");
							}
						}(), ...
							);
				});

			return JSON(JSONValue(f_Object));
		}

		template<typename T>
		enable_if_t<is_serializable_struct<T>::value, bool> //leverages SERIALIZE_FIELD function defs to assign values to a default constructed data struct
			FromJSON(const JSONValue& _j, T& out)
		{
			if (_j.JSONType != JSONValue::Type::Object)
			{
				return false;
			}

			static_assert(is_serializable_struct<T>::value, "FromJSON() can only be used with types that use SERIALIZABLE_FIELDS");

			const vector<string> fieldNames = SplitFieldNames(T::field_names);

			const JSONObject& json = get<JSONObject>(_j.m_Value);

			size_t i = 0;
			bool success = true;

			out.visit([&](auto&... fields) {
				(
					[&] {
						const string& key = fieldNames[i++];

						try 
						{
							using FieldType = decay_t<decltype(fields)>;

							if constexpr (is_same_v<FieldType, string>) 
							{
								fields = get<string>(json.at(key).m_Value);
							}
							else if constexpr (is_same_v<FieldType, bool>) 
							{
								fields = get<bool>(json.at(key).m_Value);
							}
							else if constexpr (is_floating_point_v<FieldType>) 
							{
								fields = static_cast<FieldType>(get<double>(json.at(key).m_Value));
							}
							else if constexpr (is_integral_v<FieldType> && is_signed_v<FieldType>) 
							{
								fields = static_cast<FieldType>(get<int64_t>(json.at(key).m_Value));
							}
							else if constexpr (is_integral_v<FieldType> && is_unsigned_v<FieldType>) 
							{
								fields = static_cast<FieldType>(get<uint64_t>(json.at(key).m_Value));
							}
							else if constexpr (is_serializable_struct<decay_t<decltype(fields)>>::value)
							{
								FromJSON(json.at(key), fields);
							}
							else if constexpr (is_map<decay_t<decltype(fields)>>::value)
							{
								const auto& obj = json.at(key);
								fields.clear(); //clear the map in case the user passes a map filled with values
								
								for (const auto& [mapKey, __val] : get<JSONObject>(obj.m_Value))
								{
									using ValType = typename decay_t<decltype(fields)>::mapped_type;
									ValType item{};

									if constexpr (is_serializable_struct<ValType>::value)
									{
										FromJSON(__val, item);
									}
									else
									{
										item = Extract<ValType>(__val);
									}

									fields[mapKey] = item;
								}
							}
							else if constexpr (is_vector<decay_t<decltype(fields)>>::value)
							{
								const auto& arr = get<JSONArray>(json.at(key).m_Value);
								fields.clear(); //clear the vector in case the user passes a vector filled with values
								Print(format("got to vector with key name: {}, with array size: {}", key, arr.size()));

								for (const auto& __val : arr)
								{
									using Elem = typename decay_t<decltype(fields)>::value_type;
									Elem item{};

									if constexpr (is_serializable_struct<Elem>::value) 
									{
										FromJSON(__val, item);
									}
									else 
									{
										item = Extract<Elem>(__val);
									}

									fields.push_back(item);
								}
							}
						}
						catch (const exception& e)
						{
							PrintError(format("Deserialization failed for field '{}' (type: {}): {}",
								key,
								typeid(decltype(fields)).name(),
								e.what()));
							success = false;
						}
					}(), ...
						);
				});

			return success;
		}

		template<typename T>
		T Extract(const JSONValue& json) 
		{
			using FieldType = decay_t<T>;

			std::cout << "Extracting: expected " << typeid(FieldType).name()
				<< ", JSONValue::Type = " << static_cast<int>(json.JSONType)
				<< ", variant index = " << json.m_Value.index() << std::endl;


			if constexpr (is_same_v<FieldType, string>) 
			{
				return get<string>(json.m_Value);
			}
			else if constexpr (is_same_v<FieldType, bool>)
			{
				return get<bool>(json.m_Value);
			}
			else if constexpr (is_floating_point_v<FieldType>) 
			{
				return static_cast<FieldType>(get<double>(json.m_Value));
			}
			else if constexpr (is_integral_v<FieldType> && is_signed_v<FieldType>) 
			{
				return static_cast<FieldType>(get<int64_t>(json.m_Value));
			}
			else if constexpr (is_integral_v<FieldType> && is_unsigned_v<FieldType>) 
			{
				return static_cast<FieldType>(get<uint64_t>(json.m_Value));
			}
			else 
			{
				static_assert(always_false_v<T>, "Unsupported type in Extract");
			}
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
				return Token("EOF", TokenType::ENDF, -1); //return escape char when source code is done being read
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

			while (f_CurrentToken.m_Type != TokenType::CloseBracket) //this will break out of the loop if it parses towards ENDF for invalid JSONS in the worst cases
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

			while (f_CurrentToken.m_Type != TokenType::CloseSquareBracket) //this will break out of the loop if it parses towards ENDF for invalid JSONS in the worst cases
			{
				if (not ParseValue(f_CurrentToken, fp_JSONArray, fp_Tokens, logger))
				{
					logger->LogAndPrint("Parsing Error: invalid value found while parsing an Array", "ParseArray", LogManager::LogLevel::Error);
					return false;
				}

				f_CurrentToken = ShiftForward(fp_Tokens); //shift to find comma

				if (f_CurrentToken.m_Type == TokenType::CloseSquareBracket) // check for end of array before we check for comma
				{
					break;
				}

				if (f_CurrentToken.m_Type != TokenType::Comma) //throw error if a separating comma is not found between array elements
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
			ParseValue //used for parsing values inside an array
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
					if (fp_CurrentToken.m_Value[0] == '-')
					{
						fp_Array.emplace_back(static_cast<int64_t>(stoll(fp_CurrentToken.m_Value))); // signed
					}
					else
					{
						fp_Array.emplace_back(static_cast<uint64_t>(stoull(fp_CurrentToken.m_Value))); // unsigned
					}
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

				case TokenType::OpenBracket: //check for nested objects
				{
					JSONObject f_TempObject;
					ParseObject(fp_Tokens, f_TempObject, logger);
					fp_Array.emplace_back(f_TempObject);
				}
				break;
				case TokenType::OpenSquareBracket: //check for nested arrays
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
			ParseValue //used for parsing values inside a regular JSON object
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
					if (fp_CurrentToken.m_Value[0] == '-') //XXX: this is used to handle container sizing issues coming from values serialized as a large uint64 vs a regular int64
					{
						fp_JSONObject.emplace(fp_ValueKey, static_cast<int64_t>(stoll(fp_CurrentToken.m_Value))); // signed
					}
					else
					{
						fp_JSONObject.emplace(fp_ValueKey, static_cast<uint64_t>(stoull(fp_CurrentToken.m_Value))); // unsigned
					}
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

		bool //XXX: this function assumes that the JSON is structured such that it has one top level object denoted by a "{ . . . . }"
			ParseJSON //function call that kicks off the recursive parse chain
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
				case JSONValue::Type::UnsignedInteger:
					cout << spacing << "\"" << get<uint64_t>(value.m_Value) << "\"" << endl;
					break;
				case JSONValue::Type::Array:
				{
					cout << spacing << "[" << endl;
					for (const auto& v : get<JSONArray>(value.m_Value))
					{
						PrintJSON(v, indent + 2);
						cout << ",";
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

		bool
			WriteToJSON
			(
				const string& fp_DesiredOutputDirectory,
				const string& fp_DesiredName,
				const JSON& fp_JSON,
				LogManager* logger
			)
		{
			if (not logger)
			{

				return false;
			}

			string f_FileName;

			f_FileName = fp_DesiredOutputDirectory + "/" + fp_DesiredName + ".json";

			ofstream file(f_FileName, ios::binary);  // Open in binary mode

			if (!file)
			{
				logger->LogAndPrint(format("Serialization Error: Failed to open file: '{}' for writing.", f_FileName), "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			string f_JSONString;

			if (not fp_JSON.ToString(&f_JSONString))
			{

				return false;
			}

			// Write JSON string -> .json file
			file.write(f_JSONString.c_str(), f_JSONString.size());
			// Close the file
			file.close(); 

			return true;
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