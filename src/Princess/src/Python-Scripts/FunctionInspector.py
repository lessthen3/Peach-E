"""
	This class is used to extract function information for Princess to autopopulate code blocks with appropriate function parameters and names hopefully >w<
	Copyright(c) 2024-present Ranyodh Singh Mandur.

"""


import ast
import inspect
from types import BuiltinFunctionType, FunctionType, ModuleType
from typing import Dict, Any, Union

class FunctionInspector:
    def __init__(self, target: Union[FunctionType, BuiltinFunctionType, ModuleType]):
        self.target = target

    def get_functions(self) -> Dict[str, Any]:
        
        functions = {}
        
        if isinstance(self.target, ModuleType):
            # Handle both Python functions and built-in functions
            items = inspect.getmembers(self.target, lambda x: inspect.isfunction(x) or inspect.isbuiltin(x))
            for name, obj in items:
                functions[name] = self.extract_function_details(obj)
                
        elif isinstance(self.target, (FunctionType, BuiltinFunctionType)):
            functions[self.target.__name__] = self.extract_function_details(self.target)
            
        return functions

    def extract_function_details(self, func) -> Dict[str, Any]:
        try:
            signature = inspect.signature(func)
            has_return_statement = self.has_return_statement(func)
            return {
                "name": func.__name__,
                "doc": func.__doc__,
                "args": [
                    {
                        "name": name,
                        "default": param.default if param.default is not inspect.Parameter.empty else None,
                        "annotation": param.annotation if param.annotation is not inspect.Parameter.empty else None
                    }
                    for name, param in signature.parameters.items()
                ],
                "return_annotation": signature.return_annotation if signature.return_annotation is not inspect.Signature.empty else None,
                "returns_something": has_return_statement
            }

        except ValueError:
            # Simplified handling for built-in functions that lack a valid signature
            return {
                "name": func.__name__,
                "doc": func.__doc__,
                "args": "unknown",  # Args not introspectable so set to unknown
                "return_annotation": None,
                "returns_something": "idfk"
            }
        
    def has_return_statement(self, func) -> bool:
        try:
            if isinstance(func, (BuiltinFunctionType,)):
                # Skip built-in functions, as they can't be inspected for Python source code
                return "Built-in-Function"
        
            source_lines, _ = inspect.getsourcelines(func)
            source_code = ''.join(source_lines)
            parsed = ast.parse(source_code)

            for node in ast.walk(parsed):
                if isinstance(node, ast.Return):
                    return "yes"

            return "no"

        except (OSError, TypeError, IndentationError) as e:
            # Handle specific exceptions related to source retrieval or parsing
            print(f"Error while inspecting function: {e}")
            return "something bad happened"

        except Exception as e:
            # Handle any unexpected errors
            print(f"Unexpected error: {e}")
            return "something bad happened"