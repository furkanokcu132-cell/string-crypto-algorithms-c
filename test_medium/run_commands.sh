# ──────────────────────────────────────────────────────────
# Test commands for MEDIUM difficulty
# ──────────────────────────────────────────────────────────
# Build
gcc main.c -o enigma -lm

# Task 1: Caesar decrypt with known word
./enigma caesar-known /Users/denizakkoc/Desktop/spring26_assignment1/test_medium/input/message1_encrypted.txt RETREAT output/message1_decrypted.txt

# Task 2a: Vigenere encrypt
./enigma vigenere-encrypt /Users/denizakkoc/Desktop/spring26_assignment1/test_medium/input/message2_plaintext.txt CIPHER output/message2_encrypted.txt

# Task 2b: Vigenere decrypt
./enigma vigenere-decrypt /Users/denizakkoc/Desktop/spring26_assignment1/test_medium/input/message3_encrypted.txt CIPHER output/message3_decrypted.txt

# Task 3: Caesar frequency analysis
./enigma caesar-freq /Users/denizakkoc/Desktop/spring26_assignment1/test_medium/input/message4_encrypted.txt output/message4_decrypted.txt output/message4_results.txt

# Task 4: Vigenere key recovery
./enigma vigenere-freq /Users/denizakkoc/Desktop/spring26_assignment1/test_medium/input/message5_encrypted.txt 6 output/message5_decrypted.txt output/message5_results.txt

# Task 5: Caesar word scoring
./enigma caesar-words /Users/denizakkoc/Desktop/spring26_assignment1/test_medium/input/message6_encrypted.txt /Users/denizakkoc/Desktop/spring26_assignment1/test_medium/input/common_words.txt output/message6_decrypted.txt output/message6_results.txt
