#!/usr/bin/env bash

clean_dir=$(pwd)
params="-pedantic -O2 -Wall -Werror -std=c99 -D_GNU_SOURCE"
repo_name="caesar-utils"
tests_directory="data"
return_code=1

if ! ls ./"$tests_directory" 1> /dev/null
then
  echo Cannot find tests directory \"$tests_directory\"
  exit 2
fi

mkdir tmp
cd tmp || (echo Cannot create tmp directory && exit 3)

gcc $params ../"main.c" -o $repo_name
while [ ! -f $repo_name ]; do sleep 0.5; done
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
    return_code=4
  fi

  if diff --strip-trailing-cr <(sort "$my_output_file") <(sort "$output_file") &> /dev/null;
  then
    echo Test \""${input_file}"\" has been passed
    if [ $return_code -eq 1 ]; then
      return_code=0
    fi
  else
    echo Test \""${input_file}"\" has been failed
    diff --strip-trailing-cr <(sort "$my_output_file") <(sort "$output_file")
    return_code=5
  fi
done

clean_tmp () {
  echo That\'s all, exiting..
  cd "$clean_dir" || exit 6
  if [ -d tmp ];
  then
    rm -r tmp
  fi
  exit $return_code
}

trap clean_tmp INT TERM

clean_tmp
