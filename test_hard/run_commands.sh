# ──────────────────────────────────────────────────────────
# Test commands for HARD difficulty
# ──────────────────────────────────────────────────────────
# Build
gcc main.c -o enigma -lm

# Task 1: Caesar decrypt with known word
./enigma caesar-known /Users/denizakkoc/Desktop/spring26_assignment1/test_hard/input/message1_encrypted.txt ENIGMA output/message1_decrypted.txt

# Task 2a: Vigenere encrypt
./enigma vigenere-encrypt /Users/denizakkoc/Desktop/spring26_assignment1/test_hard/input/message2_plaintext.txt BLETCHLEY output/message2_encrypted.txt

# Task 2b: Vigenere decrypt
./enigma vigenere-decrypt /Users/denizakkoc/Desktop/spring26_assignment1/test_hard/input/message3_encrypted.txt BLETCHLEY output/message3_decrypted.txt

# Task 3: Caesar frequency analysis
./enigma caesar-freq /Users/denizakkoc/Desktop/spring26_assignment1/test_hard/input/message4_encrypted.txt output/message4_decrypted.txt output/message4_results.txt

# Task 4: Vigenere key recovery
./enigma vigenere-freq /Users/denizakkoc/Desktop/spring26_assignment1/test_hard/input/message5_encrypted.txt 6 output/message5_decrypted.txt output/message5_results.txt

# Task 5: Caesar word scoring
./enigma caesar-words /Users/denizakkoc/Desktop/spring26_assignment1/test_hard/input/message6_encrypted.txt /Users/denizakkoc/Desktop/spring26_assignment1/test_hard/input/common_words.txt output/message6_decrypted.txt output/message6_results.txt
