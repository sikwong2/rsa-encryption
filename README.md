# rsa-encryption
An implementation of RSA encryption in C using the gmp library. The implementation is able to encrypt/decrypt files and input from stdin and outputs to a file or stdout. 

# Make
To make the the encode and decode executables type `make` or `make all` in the terminal.

# Usage
## Encrypt  
Encrypts data using RSA encrption. Encrypted data is decrypted by the decrypt program.  
**USAGE**  
`./encrypt [-h] [-i infile] [-o outfile]`  
`Flags`  
    * `-h` Program usage and help.  
    * `-i [infile]` Input file to encrypt (default is stdin).  
    * `-o [outfile]` Output of encrypted data (default is stdout).  
  
## D    ecrypt
Decrypts data using RSA decryption.  
**USAGE**  
`./decrypt [-h] [-i infile] [-o outfile]`  
`Flags`  
    * `-h` Program usage and help.  
    * `-i [infile]` Input file to decrypt (default is stdin).  
    * `-o [outfile]` Output of decrypted data (default is stdout).  
  
# Cleanup
To cleanup all created files, type `make clean` into the command line.




