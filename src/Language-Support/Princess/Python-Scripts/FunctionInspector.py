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
                "return_annotation": signature.return_annotation if signature.return_annotation is not inspect.Signature.empty else None
            }

        except ValueError:
            # Simplified handling for built-in functions that lack a valid signature
            return {
                "name": func.__name__,
                "doc": func.__doc__,
                "args": "unknown",  # Args not introspectable so set to unknown
                "return_annotation": None
            }
