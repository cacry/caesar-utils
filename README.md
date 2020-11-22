# Caesar utils
## Description

Customizable Caesar cipher encrypting/decrypting CLI tool written in C.
Supports custom dictionaries, finding the correct variant by the known part of a string (implemented using [Levenshtein distance](https://en.wikipedia.org/wiki/Levenshtein_distance) and [Wagner–Fischer](https://en.wikipedia.org/wiki/Wagner%E2%80%93Fischer_algorithm) algorithm).
&nbsp;
## Usage
```
usage: ./caesar-utils [-sehavc]

default: decode Caesar cipher knowing original message with some replaced symbols.
-s Decode Caesar cipher knowing substring of the original string.
-e Encrypt text at offset n.
-c Use custom alphabet (default: a-Z0-9).
-a Print all the possible variants.
-h Print this message.
-v Print debug information.
```
&nbsp;
## Download && Build

```
git clone https://github.com/cacry/caesar-utils
cd caesar-utils
cmake .
make
```
&nbsp;
## Run tests
```
chmod +x test.sh
./test.sh
```
&nbsp;
## Examples

- `./caesar-utils`
```
Enter the encrypted string:
efbecffg
Enter the corrupted original string:
somebeef
```
 **->**
`deadbeef`

&nbsp;

- `./caesar-utils -a -c`
```
Enter the custom alphabet:
1234
Enter the encrypted string:
23
```
 **->**
```
23
34
41
12
```

&nbsp;

- `./caesar-utils -s`
```
Enter the encrypted string:
mkoCkB
Enter the corrupted part or substring of the original string:
aes
```
 **->**
`caesar`

&nbsp;

- `./caesar-utils -e -c`
```
Enter the custom alphabet:
abcdefghijklmnopqrstuvwxyz
Enter the original string to encrypt:
secret
Enter shift offset:
2
```
 **->**
`ugetgv`

&nbsp;
## License
- [MIT]((https://github.com/cacry/caesar-utils/blob/main/license))
