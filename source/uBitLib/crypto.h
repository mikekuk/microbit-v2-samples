ManagedString generateSalt();
ManagedString generateSHA256(ManagedString plain);
void encryptMessageECB(ManagedString plaintext, uint8_t key[32], uint8_t * buffer);