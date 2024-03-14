unsigned char interleaveBits(unsigned char number) {
    unsigned char upperBits = (number & 0xF0) >> 4; 
    unsigned char lowerBits = number & 0x0F;

    unsigned char result = 0;
    for (int i = 0; i < 4; i++) {
        result <<= 2; 
        result |= (lowerBits & 1);  
        lowerBits >>= 1; 
        result |= (upperBits & 1) << 1;  
        upperBits >>= 1;
    }
    return result;
}