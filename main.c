/**
 * COMP 201 - Spring 2026
 * Assignment 1 - Strings in C: The Imitation Game
 * This file contains the main function for the cipher cracking program.
 * The main function parses command-line arguments and calls
 * the functions to be implemented by the students.
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEXT_SIZE 100000
#define MAX_LINE_LEN 2048
#define MAX_WORD_LEN 64
#define MAX_WORD_COUNT 50
#define ALPHABET_SIZE 26

/**
 * Task 1: Known-Plaintext Caesar Attack
 *
 * Decrypt a Caesar cipher when a known word appears in the plaintext.
 * Try all 26 possible shifts and check if the known word appears in the result.
 *
 * @param encrypted_path: path to the encrypted file
 * @param known_word: a word guaranteed to appear in the plaintext
 * @param output_path: path to write the decrypted text
 * @return 0 on success, non-zero on failure
 */

  // TEMP: echo arguments to verify wiring. Delete this block after testing.
  // TODO: Implement according to description.
 
  //I open the file, read it line by line, and copy each line into buf.
  //If the file can't be opened or the buffer is too small, I return -1.
  //On success I return 0.
  static int read_file(const char *path, char *buf, int buf_size) {

	  FILE *f = fopen(path, "r");
	  if (!f) return -1;

	  char line[MAX_LINE_LEN];
	  int total = 0;

	  while (fgets(line, sizeof(line), f)) {

		  int len = strlen(line);
		  
		  //If adding this line would overflow the buffer, I abort early.
		  if (total + len >= buf_size) {

			  fclose(f);
			  return -1;
		  }

		  memcpy(buf + total, line, len);
		  total += len;

	  }

	  buf[total] = '\0';
	  fclose(f);
          return 0;
}
 //I subtract the shift from the character's alphabet position (mod 26).
 //I keep uppercase and lowercase letters separate, and leave.
 //Non-alphabetic characters, I keep them unchanged.
 static char decrypt_char( char c, int shift ) {

	 if (isupper(c))
		 return (char)((c - 'A' - shift + 26) % 26 + 'A');
	 if (islower(c))
		 return (char)((c - 'a' - shift + 26) % 26 + 'a');
	 return c;

 }
 
 //I just call decrypt_char on every character in the input and write the result into out.
 static void decrypt_text( const char *in, char *out, int shift) {

	 int i = 0;

	 while ( in[i] != '\0' ) {

		 out[i] = decrypt_char( in[i], shift);
		 i++;

	 }

	 out[i] = '\0';

 }
 
 //I need this so that my word comparisons are case-insensitive.
 //I stop early if I am about to exceed buf_size to avoid overflow.
 static void to_lower( const char *in, char *out, int buf_size ) {

	 int i = 0;
	 
	 while ( in[i] != '\0' && i < buf_size - 1 ) {

		 out[i] = (char)tolower((unsigned char)in[i]);
		 i++;

	 }

	 out[i] = '\0';

 }
 
 //I first lowercase both strings, then use strstr to search.
 //strstr returns NULL if the word is not found, so I return 0 in that case.
 //Dynamic memory allocation is not allowed so I used these static buffers.
 static int contains_word( const char *text, const char *word ) {

	 static char text_lower[MAX_TEXT_SIZE];
	 static char word_lower[MAX_WORD_LEN];

	 to_lower(text, text_lower, MAX_TEXT_SIZE);
	 to_lower(word, word_lower, MAX_WORD_LEN);

	 return strstr(text_lower, word_lower) != NULL;

 }
 //Basically writing strings into a file.
 static int write_file( const char *path, const char *buf ) {

	 FILE *f = fopen(path, "w");
	 if (!f) return -1;
	 fputs(buf, f);
	 fclose(f);
	 return 0;

 }
 //Main function that ties everything together.
 //I read the encrypted file, try all 26 shifts one by one, and write the decrypted text to output_path when I find the shift that produces a text containing known_word. 
 //If no shift works, I return -1.
 int caesar_known_word(const char *encrypted_path, const char *known_word,
                      const char *output_path) {


	int shift;
	char encrypted[MAX_TEXT_SIZE];
	char decrypted[MAX_TEXT_SIZE];

	if ( read_file(encrypted_path, encrypted, MAX_TEXT_SIZE ) != 0 )
		return -1;

	for ( shift = 1; shift <= 25; shift++ ) {

		decrypt_text( encrypted, decrypted, shift);

		if (contains_word( decrypted, known_word )) {

			return write_file(output_path, decrypted);

		}
	}

	return -1;

}



/**
 * Task 2a: Vigenère Encryption
 *
 * Encrypt plaintext using the Vigenère cipher with a given key.
 *
 * @param plaintext_path: path to the plaintext file
 * @param key: encryption key (case-insensitive)
 * @param output_path: path to write the encrypted text
 * @return 0 on success, non-zero on failure
 */
int vigenere_encrypt(const char *plaintext_path, const char *key,
                     const char *output_path) {
  
    // TEMP: echo arguments to verify wiring. Delete this block after testing.
    // TODO: Implement according to description.
    
    //I open both files at the start, if either fails I close whats already open    and return an error.
    FILE *in = fopen(plaintext_path, "r");
    FILE *out = fopen(output_path, "w");

    if (!in || !out) {
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }
    
    //I convert the key to uppercase so I don't have to worry about whether the     user passed "furkan" or "FURKAN".  
    int key_len = strlen(key);
    char upper_key[256];
    int i;
    for (i = 0; i < key_len; i++)
        upper_key[i] = toupper((unsigned char)key[i]);
    upper_key[key_len] = '\0';

    int key_idx = 0;
    int c;

    while ((c = fgetc(in)) != EOF) {
        if (!isalpha(c)) {
	    //If its not a letter I just write it as is spaces and punctuation d            o not get encrypted.
            fputc(c, out);
        } else {
            //I calculate the shift for this position by looking at the current             key character.
            int shift = upper_key[key_idx % key_len] - 'A';

	    //I handle upper and lowercase separetly to preserve the original ca            sing of the plaintext.
            if (isupper(c))
                fputc('A' + (c - 'A' + shift) % 26, out);
            else
                fputc('a' + (c - 'a' + shift) % 26, out);

            key_idx++;
        }
    }

    fclose(in);
    fclose(out);
    return 0;

}

/**
 * Task 2b: Vigenère Decryption
 *
 * Decrypt Vigenère ciphertext with a known key.
 *
 * @param encrypted_path: path to the encrypted file
 * @param key: decryption key (case-insensitive)
 * @param output_path: path to write the decrypted text
 * @return 0 on success, non-zero on failure
 */
int vigenere_decrypt(const char *encrypted_path, const char *key,
                     const char *output_path) {
      	
    // TEMP: echo arguments to verify wiring. Delete this block after testing.
    // TODO: Implement according to description.

    //I open the encrypted file for reading and the output file for writing, if     either one fails I close whats already open and bail out.
    FILE *in = fopen(encrypted_path, "r");
    FILE *out = fopen(output_path, "w");

    if (!in || !out) {
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }

    //Same as encrypt, I uppercase the key first so it doesn't matter how it was    passed in.
    int key_len = strlen(key);
    char upper_key[256];
    int i;
    for (i = 0; i < key_len; i++)
        upper_key[i] = toupper((unsigned char)key[i]);
    upper_key[key_len] = '\0';

    int key_idx = 0;
    int c;

    while ((c = fgetc(in)) != EOF) {
        if (!isalpha(c)) {
	    //Non-letter characters are written directly, they were never encryp            ted in the first place.
            fputc(c, out);
        } else {
	    //I get the shift value the same way as in encrypt.
            int shift = upper_key[key_idx % key_len] - 'A';

	    //The only difference from encrypt is that I subtract the shift inst            ead of adding it.
            if (isupper(c))
                fputc('A' + (c - 'A' - shift + 26) % 26, out);
	    else
                fputc('a' + (c - 'a' - shift + 26) % 26, out);  

            key_idx++;
        }
    }

    fclose(in);
    fclose(out);
    return 0;

}
/**
 * Task 3: Frequency-Based Caesar Cracking
 *
 * Crack a Caesar cipher using letter frequency analysis.
 * Try all 26 shifts and compute chi-squared statistic for each.
 * The shift with the lowest chi-squared value is most likely correct.
 *
 * @param encrypted_path: path to the encrypted file
 * @param output_path: path to write the decrypted text
 * @param results_path: path to write chi-squared scores for all shifts
 * @return 0 on success, non-zero on failure
 */
int caesar_frequency(const char *encrypted_path, const char *output_path,
                     const char *results_path) {

    //I get this from the assignment instructions to use.
    double eng_freq[] = {
        8.17, 1.29, 2.78, 4.25, 12.70, 2.23, 2.02, 6.09, 6.97, 0.15,
        0.77, 4.03, 2.41, 6.75, 7.51, 1.93, 0.10, 5.99, 6.33, 9.06,
        2.76, 0.98, 2.36, 0.15, 1.97, 0.07
    };
    
    //Static buffers
    static char text[MAX_TEXT_SIZE];
    int fsize = 0;
    int c;

    FILE *fin = fopen(encrypted_path, "r");
    if (!fin) return 1;

    while ((c = fgetc(fin)) != EOF && fsize < MAX_TEXT_SIZE - 1)
        text[fsize++] = (char)c;
    text[fsize] = '\0';
    fclose(fin);

    //I count how many times each letter appears in the text. I treat upper and     lowercase as same letter so I convert to uppercase before counting.
    int counts[26] = {0};
    int total = 0;
    for (int i = 0; i < fsize; i++) {
        if (isalpha(text[i])) {
            counts[toupper(text[i]) - 'A']++;
            total++;
        }
    }

    //I try every possible shift from 0 to 25 and compute the chi squared statis    tic for each one. Lower chi2 means it is closer to real english so thats the    one I want, it is written in instructions too.
    double chi2[26];
    int best_shift = 0;
    double best_chi2 = -1;

    for (int s = 0; s < 26; s++) {
        double chi = 0.0;
        for (int i = 0; i < 26; i++) {
            int obs = counts[(i + s) % 26];
            double expected = (eng_freq[i] / 100.0) * total;
            double diff = obs - expected;
            chi += (diff * diff) / expected;
        }
        chi2[s] = chi;
        if (best_chi2 < 0 || chi < best_chi2) {
            best_chi2 = chi;
            best_shift = s;
        }
    }

    //I write all 26 scores to the results file and mark the best one so it is e    asy to see which shift won.
    FILE *fres = fopen(results_path, "w");
    if (!fres) return 1;

    for (int s = 0; s < 26; s++) {
        if (s == best_shift)
            fprintf(fres, "Shift %d: chi2 = %.2f <-- best\n", s, chi2[s]);
        else
            fprintf(fres, "Shift %d: chi2 = %.2f\n", s, chi2[s]);
    }
    fprintf(fres, "Best shift: %d (chi2 = %.2f)\n", best_shift, best_chi2);
    fclose(fres);

    //Finally I decrypt the text using the best shift I found and write it to th    e output file.
    FILE *fout = fopen(output_path, "w");
    if (!fout) return 1;

    for (int i = 0; i < fsize; i++) {
        if (isalpha(text[i])) {
            char base = isupper(text[i]) ? 'A' : 'a';
            fputc((text[i] - base - best_shift + 26) % 26 + base, fout);
        } else {
            fputc(text[i], fout);
        }
    }

    fclose(fout);
    return 0;

}
/**
 * Task 4: Vigenère Key Recovery via Frequency Analysis
 *
 * Recover the Vigenère key when its length is known.
 * Split ciphertext into groups based on key length and apply
 * frequency analysis to each group independently.
 *
 * @param encrypted_path: path to the encrypted file
 * @param key_length: length of the encryption key
 * @param output_path: path to write the decrypted text
 * @param results_path: path to write the recovered key
 * @return 0 on success, non-zero on failure
 */
int vigenere_frequency(const char *encrypted_path, int key_length,
                       const char *output_path, const char *results_path) {

    //Same table.
    double eng_freq[] = {
        8.17, 1.29, 2.78, 4.25, 12.70, 2.23, 2.02, 6.09, 6.97, 0.15,
        0.77, 4.03, 2.41, 6.75, 7.51, 1.93, 0.10, 5.99, 6.33, 9.06,
        2.76, 0.98, 2.36, 0.15, 1.97, 0.07
    };

    //Statics.
    static char text[MAX_TEXT_SIZE];
    static char letters[MAX_TEXT_SIZE];
    char key[64];  
   
    //I read the file character by character into text.
    int fsize = 0;
    int c;
    FILE *fin = fopen(encrypted_path, "r");
    if (!fin) return 1;
    while ((c = fgetc(fin)) != EOF && fsize < MAX_TEXT_SIZE - 1)
        text[fsize++] = (char)c;
    text[fsize] = '\0';
    fclose(fin);

    //I found only letters because grouping logic only works on letters.
    int letter_count = 0;
    for (int i = 0; i < fsize; i++) {
        if (isalpha(text[i]))
            letters[letter_count++] = toupper(text[i]);
    }
    letters[letter_count] = '\0';


    //If the key lenght is F, then every F-th letter was encrypted with the same    shift, so I split into F groups and run chi squared on each group independan    tly to find that groups shift, which gives me one character of the key.

    for (int g = 0; g < key_length; g++) {
    int counts[26] = {0};
    int total = 0;

    //I pick every F-th letter starting at position g.
    for (int i = g; i < letter_count; i += key_length) {
           
	    counts[letters[i] - 'A']++;
            total++;
        }
        
        //Now I try all 26 shifts and find the one with lowest chi2, same approa        ch I used before.
        int best_shift = 0;
        double best_chi2 = -1;
        for (int s = 0; s < 26; s++) {
            double chi = 0.0;
            for (int i = 0; i < 26; i++) {
                int obs = counts[(i + s) % 26];
                double expected = (eng_freq[i] / 100.0) * total;
                double diff = obs - expected;
                chi += (diff * diff) / expected;
            }
            if (best_chi2 < 0 || chi < best_chi2) {
                best_chi2 = chi;
                best_shift = s;
            }
        }

        key[g] = 'A' + best_shift;
    }
    key[key_length] = '\0';

    //I write the recovered key to the results file.
    FILE *fres = fopen(results_path, "w");
    if (!fres) return 1;
    fprintf(fres, "Key length: %d\n", key_length);
    fprintf(fres, "Recovered key: %s\n", key);
    fclose(fres);

    //Decrypting the original text.
    FILE *fout = fopen(output_path, "w");
    if (!fout) return 1;

    int key_pos = 0;
    for (int i = 0; i < fsize; i++) {
        if (isalpha(text[i])) {
            char base = isupper(text[i]) ? 'A' : 'a';
            int shift = key[key_pos % key_length] - 'A';
            fputc((text[i] - base - shift + 26) % 26 + base, fout);
            key_pos++;
        } else {
            fputc(text[i], fout);
        }
    }
    fclose(fout);

    return 0;
}
/**
 * Task 5: Caesar Cracking via Common Word Scoring
 *
 * Crack a Caesar cipher by scoring candidate decryptions based on
 * how many common English words they contain.
 *
 * @param encrypted_path: path to the encrypted file
 * @param wordlist_path: path to the common words list with scores
 * @param output_path: path to write the decrypted text
 * @param results_path: path to write scores for all shifts
 * @return 0 on success, non-zero on failure
 */
int caesar_word_score(const char *encrypted_path, const char *wordlist_path,
                      const char *output_path, const char *results_path) { 


    static char text[MAX_TEXT_SIZE];
    static char decrypted[MAX_TEXT_SIZE];
    int fsize = 0;
    int c;

    //I read the encrypted file into text.
    FILE *fin = fopen(encrypted_path, "r");
    if (!fin) return 1;
    while ((c = fgetc(fin)) != EOF && fsize < MAX_TEXT_SIZE - 1)
        text[fsize++] = (char)c;
    text[fsize] = '\0';
    fclose(fin);

    //I load the wordlist and lowercase every word for case insensitive matching    later.
    FILE *fwl = fopen(wordlist_path, "r");
    if (!fwl) return 1;

    char wl_words[50][64];
    int  wl_scores[50];
    int  wl_count = 0;

    while (wl_count < 50 && fscanf(fwl, "%63s %d", wl_words[wl_count], &wl_scores[wl_count]) == 2) {
        for (int i = 0; wl_words[wl_count][i]; i++)
            wl_words[wl_count][i] = tolower(wl_words[wl_count][i]);
        wl_count++;
    }
    fclose(fwl);

    int best_shift = 0;
    long best_score = -1;
    long scores[26];

    for (int s = 0; s < 26; s++) {
   	for (int i = 0; i <= fsize; i++) {
            if (isalpha(text[i])) {
                char base = isupper(text[i]) ? 'A' : 'a';
                decrypted[i] = (text[i] - base - s + 26) % 26 + base;
            } else {
                decrypted[i] = text[i];
            }
        }

        //I split the decrypted text into words and add up the scores for every         word that appears in my wordlist.
	long total = 0;
        int i = 0;
        while (i <= fsize) {
            while (i <= fsize && !isalpha(decrypted[i])) i++;
            if (i > fsize) break;

            char word[256];
            int wlen = 0;
            while (i <= fsize && isalpha(decrypted[i]) && wlen < 255) {
                word[wlen++] = tolower(decrypted[i]);
                i++;
            }
            word[wlen] = '\0';

            for (int w = 0; w < wl_count; w++) {
                if (strcmp(word, wl_words[w]) == 0) {
                    total += wl_scores[w];
                    break;
                }
            }
        }

        scores[s] = total;
        if (total > best_score) {
            best_score = total;
            best_shift = s;
        }
    }

    //I write all scores and mark the best one.
    FILE *fres = fopen(results_path, "w");
    if (!fres) return 1;
    for (int s = 0; s < 26; s++) {
        if (s == best_shift)
            fprintf(fres, "Shift %d: score = %ld <-- best\n", s, scores[s]);
        else
            fprintf(fres, "Shift %d: score = %ld\n", s, scores[s]);
    }
    fprintf(fres, "Best shift: %d (score = %ld)\n", best_shift, best_score);
    fclose(fres);

    //I decrypt with the best shift and write the output. 
    FILE *fout = fopen(output_path, "w");
    if (!fout) return 1;
    for (int i = 0; i < fsize; i++) {
        if (isalpha(text[i])) {
            char base = isupper(text[i]) ? 'A' : 'a';
            fputc((text[i] - base - best_shift + 26) % 26 + base, fout);
        } else {
            fputc(text[i], fout);
        }
    }
   
    fclose(fout);
    return 0;	
   
}

void print_usage(const char *prog_name) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr,
          "  %s caesar-known <encrypted_file> <known_word> <output_file>\n",
          prog_name);
  fprintf(stderr,
          "  %s vigenere-encrypt <plaintext_file> <key> <output_file>\n",
          prog_name);
  fprintf(stderr,
          "  %s vigenere-decrypt <encrypted_file> <key> <output_file>\n",
          prog_name);
  fprintf(stderr,
          "  %s caesar-freq <encrypted_file> <output_file> <results_file>\n",
          prog_name);
  fprintf(stderr,
          "  %s vigenere-freq <encrypted_file> <key_length> <output_file> "
          "<results_file>\n",
          prog_name);
  fprintf(stderr,
          "  %s caesar-words <encrypted_file> <wordlist_file> <output_file> "
          "<results_file>\n",
          prog_name);
}

int main(int argc, char *argv[]) {
  // Check for minimum number of arguments
  if (argc < 2) {
    fprintf(stderr, "Error: No subcommand provided.\n");
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  const char *command = argv[1];

  if (strcmp(command, "caesar-known") == 0) {
    // Command: ./enigma caesar-known message1_encrypted.txt ATTACK
    // message1_decrypted.txt
    if (argc != 5) {
      fprintf(stderr, "Error: Invalid arguments for 'caesar-known'\n");
      print_usage(argv[0]);
      return EXIT_FAILURE;
    }
    return caesar_known_word(argv[2], argv[3], argv[4]);

  } else if (strcmp(command, "vigenere-encrypt") == 0) {
    // Command: ./enigma vigenere-encrypt message2_plaintext.txt ENIGMA
    // message2_encrypted.txt
    if (argc != 5) {
      fprintf(stderr, "Error: Invalid arguments for 'vigenere-encrypt'\n");
      print_usage(argv[0]);
      return EXIT_FAILURE;
    }
    return vigenere_encrypt(argv[2], argv[3], argv[4]);

  } else if (strcmp(command, "vigenere-decrypt") == 0) {
    // Command: ./enigma vigenere-decrypt message3_encrypted.txt ENIGMA
    // message3_decrypted.txt
    if (argc != 5) {
      fprintf(stderr, "Error: Invalid arguments for 'vigenere-decrypt'\n");
      print_usage(argv[0]);
      return EXIT_FAILURE;
    }
    return vigenere_decrypt(argv[2], argv[3], argv[4]);

  } else if (strcmp(command, "caesar-freq") == 0) {
    // Command: ./enigma caesar-freq message4_encrypted.txt
    // message4_decrypted.txt message4_results.txt
    if (argc != 5) {
      fprintf(stderr, "Error: Invalid arguments for 'caesar-freq'\n");
      print_usage(argv[0]);
      return EXIT_FAILURE;
    }
    return caesar_frequency(argv[2], argv[3], argv[4]);

  } else if (strcmp(command, "vigenere-freq") == 0) {
    // Command: ./enigma vigenere-freq message5_encrypted.txt 6
    // message5_decrypted.txt message5_results.txt
    if (argc != 6) {
      fprintf(stderr, "Error: Invalid arguments for 'vigenere-freq'\n");
      print_usage(argv[0]);
      return EXIT_FAILURE;
    }

    int key_length = atoi(argv[3]);
    if (key_length <= 0) {
      fprintf(stderr, "Error: Invalid key_length. Value must be positive.\n");
      return EXIT_FAILURE;
    }

    return vigenere_frequency(argv[2], key_length, argv[4], argv[5]);

  } else if (strcmp(command, "caesar-words") == 0) {
    // Command: ./enigma caesar-words message6_encrypted.txt common_words.txt
    // message6_decrypted.txt message6_results.txt
    if (argc != 6) {
      fprintf(stderr, "Error: Invalid arguments for 'caesar-words'\n");
      print_usage(argv[0]);
      return EXIT_FAILURE;
    }
    return caesar_word_score(argv[2], argv[3], argv[4], argv[5]);

  } else {
    fprintf(stderr, "Error: Unknown subcommand '%s'\n", command);
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  return 0;
}
