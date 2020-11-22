#!/usr/bin/env bash

clean_dir=$(pwd)
params="-pedantic -O2 -Wall -Werror -std=c99 -D_GNU_SOURCE"
repo_name="caesar-utils"
tests_directory="data"

if ! ls ./"$tests_directory" 1> /dev/null
then
  echo Cannot find tests directory \"$tests_directory\"
  exit 2
fi

mkdir tmp
cd tmp || (echo Cannot create tmp directory && exit 3)

gcc $params ../"main.c" -o $repo_name
for input_file in ../"$tests_directory"/*.in; do
  output_file="${input_file%.in}.out"
  my_error_file="${input_file#../data/}"
  my_error_file="${my_error_file%.in}.err"
  my_output_file="${output_file#../data/}"
  execute_args=$(head -n1 < "$input_file")
  (tail -n+2 < "${input_file}") | ./$repo_name $execute_args | grep -v "Enter"  1> "$my_output_file" 2> "$my_error_file"

  if [ -s "$my_error_file" ];
  then
    echo Found error during testing "$input_file"
    echo Error output:
    cat "$my_error_file"
  fi

  if diff "$my_output_file" "$output_file" &> /dev/null;
  then
    echo Test \""${input_file}"\" has been passed
  else
    echo Test \""${input_file}"\" has been failed
    diff "$my_output_file" "$output_file"
  fi
done

clean_tmp () {
  echo That\'s all, exiting..
  cd "$clean_dir" || exit 1
  if [ -d tmp ];
  then
    rm -r tmp
  fi
  exit 0
}

trap clean_tmp INT TERM

clean_tmp