#include <stdio.h>

#include <stdlib.h>

#include <limits.h>
#include <unistd.h>


#define CHECK_ALLOC(P) if (P == NULL){fprintf(stderr, "Allocation error."); exit(1);}
#define MAX(X, Y) ((X>Y)?(X):(Y))
#define MIN(X, Y) ((X<Y)?(X):(Y))
#define MIN3(X, Y, Z) MIN(MIN(X,Y),Z)

int DEBUG_OUTPUT = 0;

// read shift length
int read_offset() {
   int offset;
   printf("Enter shift offset:\n");
   if(scanf("%d", &offset) != 1) { // TODO -1 offset test
      fprintf(stderr, "Error: Incorrect offset!\n");
      exit(102);
   }
   return offset;
}


// get the length of the string
short len_str(const char *str) {
   short len = -1;
   while(str[++len] != '\0')
      continue;
   return len;
}

// read dynamically allocated string from the input stream
short read_str(char **dst, const char *message) {
   short str_len = 0,
         allocated = 10;
   *dst = malloc(allocated);
   CHECK_ALLOC(*dst)
   
   const int message_len = len_str(message);
   printf("%.*s", message_len, message);
   fflush(stdout);
   
   char c;
   while(scanf("%c", &c) == 1 && c != '\n') {
      if(str_len + 2 > allocated) { // reserving two times more memory
         allocated *= 2;
         *dst = realloc(*dst, allocated);
         CHECK_ALLOC(*dst)
      }
      if(!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
         fprintf(stderr, "Error: Character is not from the charset!\n");
         exit(100);
      }
      (*dst)[str_len] = c;
      str_len++;
   }
   (*dst)[str_len] = '\0';
   
   if(!str_len) {
      fprintf(stderr, "Error: Empty string has been entered.\n");
      exit(104);
   }
   
   return str_len;
}

// print zero-terminated string
void print_str(const char *str) {
   for(int i = 0; str[i] != '\0'; i++)
      printf("%c", str[i]);
   printf("\n");
}


// get a number of different letters between strings
int compare_str(const char *str1,
                const char *str2,
                int different_chars) {
   
   for(int i = 0; str1[i] != '\0'; i++) {
      if(str1[i] == str2[i])
         different_chars--;
   }
   
   return different_chars;
}

// allocate memory for dp
void alloc_dp(int ***dp, int n, int m) {
   *dp = malloc(sizeof(int **) * n);
   CHECK_ALLOC(*dp)
   
   for(int i = 0; i < n; ++i) {
      (*dp)[i] = malloc(sizeof(int *) * m);
      CHECK_ALLOC((*dp)[i])
   }
}

// deallocate memory for dp
void dealloc_dp(int ***dp, int n) {
   for(int i = 0; i < n; ++i)
      free((*dp)[i]);
   
   free(*dp);
}

// calculate Levenshtein distance between two strings using DP
int levenshtein_distance(const char *s1,
                         const char *s2,
                         int s1_len,
                         int s2_len) {
   int **dp;
   s1_len++; // allocating row and column more for zero values
   s2_len++;
   alloc_dp(&dp, s1_len, s2_len);
   
   
   for(int i = 0; i < s1_len; ++i) {
      for(int j = 0; j < s2_len; ++j) {
         if(!i || !j)
            dp[i][j] = !i ? j : i;
         else {
            //printf("len1: %d - i: %d, len2: %d - j: %d\n", s1_len, i - 1, s2_len , j - 1);
            if(s1[i - 1] == s2[j - 1]) // minus one because we have zero values
               dp[i][j] = dp[i - 1][j - 1];
            else {
               dp[i][j] = MIN3(dp[i - 1][j - 1] + 1, dp[i - 1][j] + 1, dp[i][j - 1] + 1);
            }
         }
      }
   }
   
   if(DEBUG_OUTPUT == 2) {
      printf("Current DP array:\n");
      for(int i = 0; i < s1_len; ++i) {
         for(int j = 0; j < s2_len; ++j) {
            printf("%d ", dp[i][j]);
         }
         printf("\n");
      }
   }
   
   const int difference = dp[s1_len - 1][s2_len - 1];
   dealloc_dp(&dp, s1_len);
   
   return difference;
}

// returns to dst shift of src string by the given offset
void shift(const char *src,
           char *dst, int offset,
           const char *alphabet,
           const int alphabet_len,
           const int *hashmap) {
   
   for(int i = 0; src[i] != '\0'; ++i) {
      if(offset > 0)
         dst[i] = alphabet[(hashmap[(int) (src[i])] + offset) % alphabet_len];
      else
         dst[i] = alphabet[(hashmap[(int) (src[i])] + (alphabet_len + offset)) % alphabet_len];
   }
}

// create a zero-terminated string
char *alloc_str(int length) {
   char *str = malloc((length + 1));
   CHECK_ALLOC(str)
   str[length] = '\0';
   
   return str;
}

// find the most similar string
int solve_caesar(const char *result,
                 const char *intercepted,
                 const int is_different_strings_length,
                 const char *alphabet,
                 const short alphabet_len,
                 const int *hashmap,
                 char **results) {
   
   char *shifted_str;
   int result_len = len_str(result);
   int intercepted_len = len_str(intercepted);
   
   shifted_str = alloc_str(result_len);
   
   int min_difference = INT_MAX;
   int *possible_offsets = malloc(MAX(alphabet_len + 1, 1) * sizeof(int)); // assure the compiler
   CHECK_ALLOC(possible_offsets)
   int possible_offsets_count = 0;
   
   for(int offset = 0; offset < alphabet_len; offset++) {
      int current_difference;
      shift(result, shifted_str, offset, alphabet, alphabet_len, hashmap);
      if(is_different_strings_length)
         current_difference = levenshtein_distance(
               intercepted, shifted_str, intercepted_len, result_len
         );
      else
         current_difference = compare_str(intercepted, shifted_str, result_len);
      if(DEBUG_OUTPUT) {
         if(DEBUG_OUTPUT == 2)
            printf("Offset %d: ", offset);
         print_str(shifted_str);
         if(DEBUG_OUTPUT == 2) {
            printf(" original string: ");
            print_str(intercepted);
            printf(" > %d different letters\n", current_difference);
         }
      }
      
      if(current_difference < min_difference) {
         min_difference = current_difference;
         possible_offsets[0] = offset;
         possible_offsets_count = 1;
      }
      else {
         if(current_difference == min_difference) {
            possible_offsets[possible_offsets_count] = offset;
            possible_offsets_count++;
         }
      }
   }
   
   //get all the possible original strings
   for(int i = 0; i < possible_offsets_count; ++i) {
      results[i] = alloc_str(result_len);
      shift(result, results[i], possible_offsets[i], alphabet, alphabet_len, hashmap);
   }
   
   free(shifted_str);
   free(possible_offsets);
   
   return possible_offsets_count;
}

// create a hashmap, where hash[ascii] == index in the alphabet
int *create_alphabet_hashmap(const char *alphabet,
                             const short alphabet_len) {
   short max_ascii_code = SHRT_MIN;
   
   for(int i = 0; i < alphabet_len; ++i)
      max_ascii_code = MAX(max_ascii_code, (short) (alphabet[i]));
   
   int *hashmap = malloc(MAX(max_ascii_code + 1, 1)); // assure the compiler
   CHECK_ALLOC(hashmap)
   
   for(int i = 0; i < alphabet_len; ++i)
      hashmap[(int) (alphabet[i])] = i;
   
   return hashmap;
}

void print_help(FILE *s, char *argv[]) {
   fprintf(s, "Usage: %s [-sehavc]\n"
              "default: decode Caesar cipher knowing original message with some replaced symbols.\n"
              "-s Decode Caesar cipher knowing substring of the original string.\n"
              "-e Encrypt text at offset n.\n"
              "-c Use custom alphabet (default: a-Z0-9).\n"
              "-a Print all the possible variants.\n"
              "-h Print this message.\n"
              "-v Print debug information.\n", // output Levenshtein distance debug info
           argv[0]);
}

int main(int argc, char *argv[]) {
   int opt;
   enum {
      SAME_LENGTH_MODE, SHIFTED_MODE, ENCRYPT_MODE
   } mode = SAME_LENGTH_MODE;
   char *alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890\0";
   short alphabet_len = len_str(alphabet);
   int is_custom_alphabet = 0;
   
   while((opt = getopt(argc, argv, "sehavc")) != -1) {
      switch(opt) {
         case 'c':
            is_custom_alphabet = 1;
            break;
         case 'v':
            DEBUG_OUTPUT = 2;
            break;
         case 'a':
            DEBUG_OUTPUT = 1;
            break;
         case 's':
            mode = SHIFTED_MODE;
            break;
         case 'e':
            mode = ENCRYPT_MODE;
            break;
         case 'h':
            print_help(stdout, argv);
            return 0;
         default:
            print_help(stderr, argv);
            exit(EXIT_FAILURE);
      }
   }
   
   if(is_custom_alphabet)
      alphabet_len = read_str(&alphabet, "Enter the custom alphabet:\n");
   int *hashmap = create_alphabet_hashmap(alphabet, alphabet_len);
   
   
   char *str1;
   
   char **possible_variants = malloc(sizeof(char *) * alphabet_len);
   int variants_count;
   if(mode != ENCRYPT_MODE) {
      int len_str1 = read_str(&str1, "Enter the encrypted string:\n");
      char *str2;
      if(DEBUG_OUTPUT == 1) {
         str2 = str1;
      }
      else {
         if(mode == SHIFTED_MODE)
            read_str(&str2, "Enter the corrupted part or substring of the original string:\n");
         else {
            int len_str2 = read_str(&str2, "Enter the corrupted original string:\n");
            if(len_str1 != len_str2) {
               fprintf(stderr, "Error: The lengths of the two strings are different.\n"
                               "If it was intended, use -s instead.");
               exit(101);
            }
         }
      }
      
      variants_count = solve_caesar(str1, str2, mode, alphabet, alphabet_len, hashmap, possible_variants);
      free(str2);
   }
   else {
      int len_str1 = read_str(&str1, "Enter the original string to encrypt:\n");
      int offset = read_offset();
      possible_variants[0] = alloc_str(len_str1);
      variants_count = 1;
      shift(str1, possible_variants[0], offset, alphabet, alphabet_len, hashmap);
   }
   
   free(hashmap);
   if(DEBUG_OUTPUT != 1) { // str1 == str2 in -a case
      free(str1);
      while(variants_count--) {
         print_str(possible_variants[variants_count]);
         free(possible_variants[variants_count]);
      }
   }
   if(is_custom_alphabet)
      free(alphabet);
   free(possible_variants);
   
   return 0;
}
