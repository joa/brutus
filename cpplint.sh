#!/bin/bash
filters=(
build/class
build/deprecated
build/endif_comment
build/forward_decl
build/include_order
build/printf_format
build/storage_class
readability/boost
readability/braces
readability/casting
readability/check
readability/constructors
readability/fn_size
readability/function
readability/multiline_comment
readability/multiline_string
readability/streams
readability/todo
readability/utf8
runtime/arrays
runtime/casting
runtime/deprecated_fn
runtime/explicit
runtime/int
runtime/memset
runtime/mutex
runtime/nonconf
runtime/printf
runtime/printf_format
runtime/references
runtime/rtti
runtime/sizeof
runtime/string
runtime/virtual
runtime/vlog
whitespace/blank_line
whitespace/braces
whitespace/comma
whitespace/ending_newline
whitespace/indent
whitespace/labels
whitespace/line_length
whitespace/newline
whitespace/operators
whitespace/tab
whitespace/todo
)

filter_arg="-"
cpplint="tools/cpplint.py"

for i in ${!filters[*]}
do
   filter_arg="$filter_arg,+${filters[$i]}"
done

filter_arg="--filter=${filter_arg}"

if [ $# -ne 0 ]; then
  ${cpplint} ${filter_arg} $*
else
  ${cpplint} ${filter_arg} src/*.cc src/*.h include/*.h
fi
