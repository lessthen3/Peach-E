/*******************************************************************
 *                        Peach-E v0.0.1
 *              Created by Ranyodh Mandur - 🍑 2024
 *
 *              Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *               https://opensource.org/licenses/MIT
 *
 *           Peach-E is a free open source game engine
********************************************************************/
#pragma once

///STL
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <variant>

///Peach-E
#include "../Managers/LogManager.h"

///External
#include <zlib.h>

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

namespace PeachCore {

	struct Serializer
	{
	public:
		Serializer() = default;
		~Serializer() = default;

	public:
		template<typename T>
		bool
			FromJSON
			(
				T& fp_DesiredObject,
				const string& fp_FilePath,
				LogManager* logger
			)
		{
			string f_JsonString;
			vector<Token> f_TokenizedJson;

			JSONValue f_TempJSON;

			if (not ReadJSONIntoString(fp_FilePath, &f_JsonString, logger)) //get JSON into a string
			{
				logger->LogAndPrint("Failed to Read JSON", "FromJSON", LogManager::LogLevel::Error);
				return false;
			}
			else if (not Tokenize(f_TokenizedJson, f_JsonString, logger)) //convert JSON string into a vector of tokens
			{
				logger->LogAndPrint("Failed to Lex JSON", "FromJSON", LogManager::LogLevel::Error);
				return false;
			}
			else if (not ParseJSON(f_TokenizedJson, f_TempJSON, logger)) //parse the tokens into a valid JSONValue object
			{
				logger->LogAndPrint("Failed to Parse JSON", "FromJSON", LogManager::LogLevel::Error);
				return false;
			}
			else if (not FromJSON(f_TempJSON, fp_DesiredObject)) //retrieve values and insert into fp_DesiredObject
			{
				logger->LogAndPrint(format("Failed to retrieve data values from desired JSON file: {}", fp_FilePath), "FromJSON", LogManager::LogLevel::Error);
				return false;
			}

			return true;
		}

		template<typename T>
		bool
			ToJSON
			(
				T& fp_DesiredObject,
				const string& fp_DesiredFileName,
				const string& fp_DesiredOutputDirectory,
				LogManager* logger
			)
		{
			JSONValue f_TempJSON = ToJSON(fp_DesiredObject);

			if (not WriteToJSON(fp_DesiredOutputDirectory, fp_DesiredFileName, f_TempJSON, logger))
			{
				logger->LogAndPrint(format("Failed writing to JSON file: {}, nothing was done", fp_DesiredFileName), "ToJSON", LogManager::LogLevel::Error);
				return false;
			}

			return true;
		}

	private:
		//////////////////////////////////////////////
		// Token and Token-type Definition for JSON
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
		// Utility Function for Tokenizing JSON
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
		// JSON Parsing
		//////////////////////////////////////////////
		struct JSONValue;

		using JSONObject = unordered_map<string, JSONValue>; //used for regular JSONObjects
		using JSONArray = vector<JSONValue>; //used for JSON arrays and vectors

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
		// JSON Utility Functions
		//////////////////////////////////////////////

		bool
			ToString(string* fp_JSONString, const JSONValue& fp_JSON ) //kicks off recursive creation of JSON string
			const
		{
			if (not fp_JSONString)
			{
				PrintError("Passed nullptr reference to string, ToString() is not possible exiting function call immediately");
				return false;
			}

			stringstream f_TempString;

			if (not ToStringStream(fp_JSON, f_TempString))
			{
				PrintError("Unable to stringify JSON");
				return false;
			}

			*fp_JSONString = f_TempString.str();
			return true;
		}

		bool
			ToStringStream
			(
				const JSONValue& fp_JSONValue,
				stringstream& fp_JSONString,
				const uint32_t fp_Spacing = 0
			)
			const
		{
			string f_IndentLevel(fp_Spacing, ' ');

			switch (fp_JSONValue.JSONType)
			{
				case JSONValue::Type::String:
					fp_JSONString << '"' << get<string>(fp_JSONValue.m_Value) << '"';
					break;
				case JSONValue::Type::Null:
					fp_JSONString << '"' << "null" << '"';
					break;
				case JSONValue::Type::Boolean:
					fp_JSONString << (get<bool>(fp_JSONValue.m_Value) ? "true" : "false");
					break;
				case JSONValue::Type::Integer:
					fp_JSONString << get<int64_t>(fp_JSONValue.m_Value);
					break;
				case JSONValue::Type::Float:
					fp_JSONString << get<double>(fp_JSONValue.m_Value);
					break;
				case JSONValue::Type::UnsignedInteger:
					fp_JSONString << get<uint64_t>(fp_JSONValue.m_Value);
					break;
				case JSONValue::Type::Array:
				{
					const auto& arr = get<JSONArray>(fp_JSONValue.m_Value);
					string f_ScopeIndent = f_IndentLevel + string(4, ' '); //add a 4 space indent for the scope

					fp_JSONString << "[" << "\n" << f_ScopeIndent; //start array, advance to next line, and indent for scope

					size_t f_Indexer = 0; //used for tracking if the col width is rlly long because i want pretty jsons uwu

					for (auto _it = arr.begin(); _it != arr.end(); ++_it)
					{
						ToStringStream(*_it, fp_JSONString, fp_Spacing + 4); //4 spaces for indent level

						if (next(_it) == arr.end()) //do this before adding any new lines to avoid double new lines for prettyness >w<
						{
							fp_JSONString << "\n" << f_IndentLevel << "]";
							break; //break so we dont add an extra comma after the end has been reached
						}

						fp_JSONString << ", "; //add comma until we hit the last element

						if (_it->JSONType == JSONValue::Type::Object or _it->JSONType == JSONValue::Type::Array) //XXX: we're assuming mono typed arrays so no mixing of objects/arrays and primitive types
						{
							fp_JSONString << "\n" << f_ScopeIndent; //new line for each JSONObject inside the array
						}
						else if (f_Indexer % 20 == 19) //this is an else if so that objects/arrays won't double line
						{
							fp_JSONString << "\n" << f_ScopeIndent; //newline every 20 elements for non objects
						}

						f_Indexer++;
					}
					break;
				}
				case JSONValue::Type::Object:
				{
					fp_JSONString << "{" << "\n";
					const auto& obj = get<JSONObject>(fp_JSONValue.m_Value);

					string f_ScopeIndent = f_IndentLevel + string(4, ' '); //add a 4 space indent for the scope

					for (auto it = obj.begin(); it != obj.end(); ++it)
					{
						fp_JSONString << f_ScopeIndent << '"' << it->first << '"' << ": ";

						ToStringStream(it->second, fp_JSONString, fp_Spacing + 4); //add 4 for indent level

						if (next(it) != obj.end()) //check for the end of the container
						{
							fp_JSONString << ", "; // comma after each element except the last
						}

						fp_JSONString << "\n";
					}

					fp_JSONString << f_IndentLevel << "}";
					break;
				}
			}

			return true;
		}

		void
			PrintToConsole(JSONValue& fp_JSON)
			const
		{
			string f_StringJSON;
			ToString(&f_StringJSON, fp_JSON);
			Print(f_StringJSON);
		}

		//////////////////////////////////////////////
		// Helper Function for field_names -> vector<string>
		//////////////////////////////////////////////

		constexpr inline vector<string> //XXX: is consteval since all vars are defined from compile time
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
		struct is_map<T, void_t< //XXX: used for deducing general map types
			typename T::key_type,
			typename T::mapped_type,
			decltype(declval<T>().begin()),
			decltype(declval<T>().end())
			>> : true_type {};

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
		enable_if_t<is_serializable_struct<T>::value, JSONValue>
			ToJSON(const T& obj) //IT WORKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKSSS IM SO TIRED >w< ;w; i sleep like a champion tn
		{
			JSONObject f_Object; // ✅ this is what i was missin UwU

			static_assert(is_serializable_struct<T>::value, "ToJSON() can only be used with types that use SERIALIZABLE_FIELDS");

			const vector<string> fieldNames = SplitFieldNames(T::field_names);

			uint64_t i = 0;

			obj.visit([&](auto&&... fields)
				{
					(
						[&]
						{
							const auto& key = fieldNames[i++];
							using FieldType = decay_t<decltype(fields)>; //makes things look prettier

							if constexpr (is_arithmetic_v<FieldType> or is_same_v<FieldType, string>)
							{
								//JSONValue constructor call will auto assign appropriate type since we utilize explicit constructors
								f_Object.emplace(key, fields); 
							}
							else if constexpr (is_serializable_struct<FieldType>::value)
							{
								f_Object.emplace(key, ToJSON(fields));
							}
							else if constexpr (is_map<FieldType>::value)
							{
								JSONObject mapObj;

								for (const auto& [mapKey, mapVal] : fields)
								{
									if constexpr (is_serializable_struct<decay_t<decltype(mapVal)>>::value)
									{
										mapObj.emplace(mapKey, ToJSON(mapVal));
									}
									//XXX: this is used for vector keys
									else if constexpr (is_vector<decay_t<decltype(mapVal)>>::value) //is constexpr here kosher idk, future me: yeah it is
									{
										JSONArray f_TempArray;
										ToJSONArray(f_TempArray, mapVal);
										mapObj.emplace(mapKey, f_TempArray);
									}
									else
									{
										mapObj.emplace(mapKey, mapVal);
									}
								}

								f_Object.emplace(key, mapObj);
							}
							else if constexpr (is_vector<FieldType>::value)
							{
								JSONArray arr;
								
								ToJSONArray(arr, fields);

								f_Object.emplace(key, arr);
							}
							else
							{
								static_assert(always_false_v<decltype(fields)>, "Unsupported field type in ToJSON");
							}
						}(), ...
							);
				});

			return move(JSONValue(f_Object)); //idk if move should be here but whatever
		}

		/*
		used for parsing vectors -> JSONArrays and deals with nested vectors since ToJSON requires SERIALIZABLE_FIELDS structs by default
		*/
		template<typename T_VectorObject>
		void
			ToJSONArray(JSONArray& fp_ArrayObject, const T_VectorObject& fp_SerializableObjectField) 
		{
			for (const auto& __val : fp_SerializableObjectField)
			{
				if constexpr (is_serializable_struct<decay_t<decltype(__val)>>::value)
				{
					fp_ArrayObject.emplace_back(ToJSON(__val)); //this handles custom structs 
				}
				else if constexpr (is_vector<decay_t<decltype(__val)>>::value)
				{
					JSONArray f_NestedArray;
					ToJSONArray(f_NestedArray, __val); //this handles nested vectors
					fp_ArrayObject.emplace_back(f_NestedArray);
				}
				else
				{
					fp_ArrayObject.emplace_back(__val);
				}
			}
		}

		template<typename T>
		enable_if_t<is_serializable_struct<T>::value, bool> //leverages SERIALIZE_FIELD function defs to assign values to a default constructed data struct
			FromJSON(const JSONValue& _j, T& out)
		{
			if (_j.JSONType != JSONValue::Type::Object)
			{
				PrintError("Passed invalid JSON type to FromJSON");
				return false;
			}

			static_assert(is_serializable_struct<T>::value, "FromJSON() can only be used with types that use SERIALIZABLE_FIELDS");

			const vector<string> fieldNames = SplitFieldNames(T::field_names);

			const JSONObject& json = get<JSONObject>(_j.m_Value);

			size_t i = 0;
			bool f_IsSuccessful = true; //XXX: used to track state of lambda execution

			out.visit([&](auto&... fields) {
				(
					[&] {
						const string& key = fieldNames[i++];

						try 
						{
							using FieldType = decay_t<decltype(fields)>;

							if constexpr (is_same_v<FieldType, string> or is_arithmetic_v<FieldType>)
							{
								fields = Extract<FieldType>(json.at(key));
							}
							else if constexpr (is_serializable_struct<FieldType>::value)
							{
								if(not FromJSON(json.at(key), fields))
								{
									PrintError("failed to deserialize non primitive struct inside JSON Object");
									f_IsSuccessful = false;
									return f_IsSuccessful; //end lambda expression
								}
							}
							else if constexpr (is_map<FieldType>::value)
							{
								const auto& obj = json.at(key);
								fields.clear(); //clear the map in case the user passes a map filled with values
								
								using ValType = typename FieldType::mapped_type;
								using KeyType = typename FieldType::key_type;

								for (const auto& [mapKey, __val] : get<JSONObject>(obj.m_Value))
								{
									ValType item{};
									//KeyType key{};

									if constexpr (is_serializable_struct<ValType>::value)
									{
										if (not FromJSON(__val, item))
										{
											PrintError("failed to deserialize non primitive struct inside JSON Object");
											f_IsSuccessful = false;
											return f_IsSuccessful; //End lambda execution
										}
									}
									//XXX: this is used for nested vectors
									else if constexpr(is_vector<ValType>::value and not is_same_v<ValType, string>) //is constexpr here kosher idk, future me: yeah it is
									{
										const JSONArray& arr = get<JSONArray>(__val.m_Value);

										using VecElem = typename ValType::value_type; // the vector<T>’s T
										vector<VecElem> f_TempVector;

										FromJSONArray(arr, f_TempVector);

										fields.emplace(mapKey, f_TempVector); //probably shouldnt have static cast here but idk it isnt working rn so im tryin everything
										continue; //continue here because item isnt set and we want to get to the next iter
									}
									else
									{
										item = Extract<ValType>(__val);
										//key = Extract<KeyType>(mapKey);
									}

									fields.emplace(mapKey, item);
								}
							}
							else if constexpr (is_vector<FieldType>::value) //XXX: don't need to check for string types here since we already do so at the first branch
							{
								const JSONArray& arr = get<JSONArray>(json.at(key).m_Value);

								if(not FromJSONArray(arr, fields))
								{
									PrintError(format("Failed to deserialize vector for field '{}'", key));
									f_IsSuccessful = false;
									return f_IsSuccessful;
								}
							}
						}
						catch (const exception& e)
						{
							PrintError(format("Deserialization failed for field '{}' (type: {}): {}",
								key,
								typeid(decltype(fields)).name(),
								e.what()));
							f_IsSuccessful = false;
							return f_IsSuccessful;
						}

						return f_IsSuccessful; //XXX: this doesnt need to be here, but i put it anyways because MSVC wont stop bitching ab it
					}(), ...
						);
				});

			return f_IsSuccessful;
		}

		template<typename T>
		T Extract(const JSONValue& json) //we extract and recast anything like doubles and 64 bit ints -> whatever the user defined eg vector<int>
		{
			using FieldType = decay_t<T>;

			if constexpr (is_same_v<FieldType, string>) 
			{
				return get<string>(json.m_Value);
			}
			else if constexpr (is_arithmetic_v<FieldType>) //AHHH IT FUCKING WORKS, have to wrap in this if otherwise the compiler bitches
			{
				switch (json.JSONType)
				{
					case JSONValue::Type::Integer: return static_cast<FieldType>(get<int64_t>(json.m_Value));
					case JSONValue::Type::UnsignedInteger: return static_cast<FieldType>(get<uint64_t>(json.m_Value));
					case JSONValue::Type::Float: return static_cast<FieldType>(get<double>(json.m_Value));
					case JSONValue::Type::Boolean: return get<bool>(json.m_Value);
					default: 
						PrintError("Unsupported type in FromJSON vector element for key: ");
						return false;
				}
			}
			else 
			{
				// Static error w/ full type sig
				#if defined(_MSC_VER)
					static_assert(always_false_v<T>, "Unsupported type in Extract. Check __FUNCSIG__ for details: " __FUNCSIG__);
				#else
					//static_assert(always_false_v<T>, "Unsupported type in Extract. Check __PRETTY_FUNCTION__ for details: " __PRETTY_FUNCTION__);
				#endif
			}
		}

		template<typename T_VectorObject>
		bool
			FromJSONArray(const JSONArray& fp_ArrayObject, T_VectorObject& fp_SerializableObjectField)
		{
			fp_SerializableObjectField.clear(); //clear the vector in case the user passes a vector filled with values

			using Elem = typename decay_t<decltype(fp_SerializableObjectField)>::value_type;

			for (const auto& __val : fp_ArrayObject)
			{
				Elem item{};

				try
				{
					if constexpr (is_serializable_struct<Elem>::value)
					{
						if (not FromJSON(__val, item))
						{
							PrintError("failed to deserialize non primitive struct");
							return false;
						}
					}
					else if constexpr (is_vector<Elem>::value and not is_same_v<Elem, string>) //XXX: used for nested vectors, needa check for strings since they're just char vectors
					{
						const JSONArray& f_NestedArray = get<JSONArray>(__val.m_Value); //peel back vector one layer at a time
						//call again assuming Elem reduces to a vector type and at the lowest level it will fill item with primitives or serializable structs
						if(not FromJSONArray(f_NestedArray, item))
						{
							PrintError("Failed to deserialize nested vector element in FromJSONArray");
							return false;
						}
					}
					else
					{
						item = Extract<Elem>(__val);
					}
				}
				catch (const std::exception& e)
				{
					PrintError(format("Deserialization failed in FromJSONArray (type: '{}'): {}", typeid(Elem).name(), e.what()));
					return false;
				}

				fp_SerializableObjectField.push_back(item);
			}

			return true; //success! JSONArray was deserialized >W<
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
					if (fp_CurrentToken.m_Value[0] == '-') //store any positive number as a uint64 because y not we'll recast it at deserialization
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
				JSONValue& fp_JSON,
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
					fp_JSON = move(JSONValue(f_Object));
				}
				break;
				case TokenType::OpenSquareBracket:
				{
					JSONArray f_Array;
					ParseArray(fp_Tokens, f_Array, logger);
					fp_JSON = move(JSONValue(f_Array));
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
				fp_JSON = JSONValue();
				return false;
			}

			return true;
		}

		//////////////////////////////////////////////
		// JSON File Read/Write Functions
		//////////////////////////////////////////////

		bool
			WriteToJSON
			(
				const string& fp_DesiredOutputDirectory,
				const string& fp_DesiredName,
				const JSONValue& fp_JSON,
				LogManager* logger
			)
			const
		{
			if (not logger)
			{
				PrintError("Serialization Error: Tried to pass nullptr reference to logger during WriteToJSON()");
				return false;
			}

			// Ensure directory exists
			if (not filesystem::exists(fp_DesiredOutputDirectory))
			{
				logger->LogAndPrint("Serialization Error: Tried to pass invalid write directory to WriteToJSON", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			const string f_FileName = fp_DesiredOutputDirectory + "/" + fp_DesiredName + ".json";

			ofstream file(f_FileName, ios::out);  // Open in regular string mode

			if (not file)
			{
				logger->LogAndPrint(format("Serialization Error: Failed to open file: '{}' for writing.", f_FileName), "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			string f_JSONString;

			if (not ToString(&f_JSONString, fp_JSON))
			{
				logger->LogAndPrint(format("Serialization Error: Failed to stringify JSON -> file: '{}' for writing.", f_FileName), "Serializer", LogManager::LogLevel::Error);
				file.close(); //close the file since writing failed
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
			if (not logger)
			{
				PrintError("Serialization Error: Tried to pass nullptr reference to logger during ReadJSONIntoString()");
				return false;
			}

			//check for nullptr
			if (not fp_SourceCode)
			{
				logger->LogAndPrint("Serialization Error: Nullptr reference passed to ReadJSONIntoString", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			// Ensure directory exists
			if (not filesystem::exists(fp_ScriptFilePath))
			{
				logger->LogAndPrint("Serialization Error: Tried to pass invalid filepath to ReadJSONIntoString", "Serializer", LogManager::LogLevel::Error);
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

			ifstream f_FileStream(fp_ScriptFilePath, ios::in);

			if (not f_FileStream)
			{
				logger->LogAndPrint("Serialization Error: Failed to open JSON for reading.", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			stringstream f_StringBuffer;
			f_StringBuffer << f_FileStream.rdbuf();
			*fp_SourceCode = f_StringBuffer.str();

			return true;
		}

		//////////////////////////////////////////////
		// Binary File Read/Write Functions
		//////////////////////////////////////////////

		bool
			WriteToBinary
			(
				const string& fp_DesiredOutputDirectory,
				const string& fp_DesiredName,
				const vector<uint8_t>& fp_Binary,
				LogManager* logger
			)
		{
			if (not logger)
			{
				PrintError("Serialization Error: Tried to pass nullptr reference to logger during WriteToBinary()");
				return false;
			}

			// Ensure directory exists
			if (not filesystem::exists(fp_DesiredOutputDirectory))
			{
				logger->LogAndPrint(format("Serialization Error: Tried to pass invalid write directory: '{}' to WriteToBinary", fp_DesiredOutputDirectory), "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			if (fp_Binary.empty()) //check if the byte vector is empty uwu
			{
				logger->LogAndPrint(format("Serialization Error: Tried passing empty byte vector for writing to file name: '{}', nothing was done.", fp_DesiredName), "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			const string f_FileName = fp_DesiredOutputDirectory + "/" + fp_DesiredName + ".json";

			ofstream file(f_FileName, ios::binary);  // Open in regular string mode

			if (not file)
			{
				logger->LogAndPrint(format("Serialization Error: Failed to open file: '{}' for writing.", f_FileName), "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			// Write the entire contents of the vector -> peach binary
			file.write(reinterpret_cast<const char*>(fp_Binary.data()), fp_Binary.size());
			// Close the file
			file.close();
			
			return true; //success! wrote byte vector -> peach binary
		}

		bool
			ReadBinaryIntoVector //i think this'll work lmfao
			(
				const string& fp_ScriptFilePath,
				vector<uint8_t>& fp_Binary,
				LogManager* logger
			)
		{
			if (not logger) //check for nullptr ref passed to ReadBinaryIntoVector
			{
				PrintError("Serialization Error: Tried to pass nullptr reference to logger during ReadBinaryIntoVector()");
				return false;
			}

			// Ensure directory exists
			if (not filesystem::exists(fp_ScriptFilePath))
			{
				logger->LogAndPrint("Serialization Error: Tried to pass invalid directory to ReadBinaryIntoVector", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			if (not fp_Binary.empty()) //check if the byte vector is empty before reading data into it OwO
			{
				logger->LogAndPrint(format("Serialization Error: Tried passing non-empty byte vector for reading to file name: '{}', nothing was done.", fp_ScriptFilePath), "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			// Extract file extension assuming format "filename.ext"
			size_t lastDotIndex = fp_ScriptFilePath.rfind('.');

			if (lastDotIndex == string::npos)
			{
				logger->LogAndPrint("Serialization Error: No file extension found for Peach-E Binary", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			string f_FileExtension = fp_ScriptFilePath.substr(lastDotIndex);

			if (f_FileExtension != ".peachbin") //file extension for peach-e binary encoding, get it? it's like a bin of peaches >w<
			{
				logger->LogAndPrint("Serialization Error: Attempted to read from a file that isn't a valid Peach-E Binary", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			ifstream f_BinaryStream(fp_ScriptFilePath, ios::binary); //open in binary mode

			if (not f_BinaryStream) //check if the file opened properly
			{
				logger->LogAndPrint("Serialization Error: Failed to open Peach-E Binary for reading.", "Serializer", LogManager::LogLevel::Error);
				return false;
			}

			// Get the size of the file
			f_BinaryStream.seekg(0, ios::end);
			size_t f_Size = f_BinaryStream.tellg();
			f_BinaryStream.seekg(0, ios::beg);

			// Resize the vector to the size of the file
			fp_Binary.resize(f_Size);

			// Read the entire file into the vector
			f_BinaryStream.read(reinterpret_cast<char*>(fp_Binary.data()), f_Size);
			// Close the file
			f_BinaryStream.close(); 

			return true; //success! file read into vector
		}
	};
}



//uwu 1069